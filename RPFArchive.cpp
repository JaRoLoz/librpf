#include "RPFArchive.hpp"

RPFArchive::RPFArchive(const std::filesystem::path& path, const std::string_view& name, const std::shared_ptr<Keyring> keyring) :
	m_EntriesData(nullptr),
	m_NameTable(nullptr),
	m_Map(MMap::s_FromFileH(path)),
	m_Header(RPFHeader::s_FromBytes((uint8_t*)m_Map->Data)),
	m_Name(name),
	m_Keyring(keyring)
{
	this->GetEntries();
	this->GetNameTable();
	this->Root = RPFDirEntry::s_FromBytes(this->m_EntriesData, this->m_NameTable);
}

RPFArchive::RPFArchive(const MMap* mmap, const std::string_view& name, const std::shared_ptr<Keyring> keyring) :
	m_EntriesData(nullptr),
	m_NameTable(nullptr),
	m_Map(mmap),
	m_Header(RPFHeader::s_FromBytes((uint8_t*)m_Map->Data)),
	m_Name(name),
	m_Keyring(keyring)
{
	this->GetEntries();
	this->GetNameTable();
	this->Root = RPFDirEntry::s_FromBytes(this->m_EntriesData, this->m_NameTable);
}

RPFArchive::~RPFArchive()
{
	// if the archive is encrypted the entries and the name table are allocated on the heap because they needed to be decrypted
	switch (this->m_Header.Encryption)
	{
	case ENCRYPTION_NG:
		[[fallthrough]];
	case ENCRYPTION_AES:
	{
		delete[] this->m_EntriesData;
		delete[] this->m_NameTable;
		break;
	}
	}
	delete this->Root;
	delete this->m_Map;
}

std::string RPFArchive::ToString() const
{
	std::string str;
	str += "RPFArchive: {";
	str += " Name: " + std::string(this->Name) + ",";
	str += " Header: " + this->Header.ToString() + ",";
	str += " Root: " + this->Root->ToString() + " }";
	return str;
}

std::vector<RPFEntry*> RPFArchive::GetDirEntries(const RPFDirEntry* entry) const
{
	std::vector<RPFEntry*> entries;
	entries.reserve(entry->EntryCount);

	uint8_t* firstEntryPtr = this->m_EntriesData + entry->FirstEntryIndex * RPF_ENTRY_SIZE;
	for (size_t i = 0; i < entry->EntryCount; i++)
	{
		uint8_t* entryPtr = firstEntryPtr + i * RPF_ENTRY_SIZE;
		entries.push_back(RPFEntry::s_FromBytes(entryPtr, static_cast<const uint8_t*>(this->m_Map->Data), this->m_NameTable, this->m_EntriesData));
	}

	return entries;
}

void RPFArchive::GetEntries()
{
	uint8_t *entriesPtr = (uint8_t*)this->m_Map->Data + RPF_HEADER_SIZE;
	switch (this->m_Header.Encryption)
	{
	case ENCRYPTION_NG:
	{
		size_t entriesSize = this->m_Header.EntryCount * RPF_ENTRY_SIZE;
		this->m_EntriesData = this->m_Keyring.get()->DecryptNG(entriesPtr, entriesSize, this->m_Name, this->m_Map->Size);
		break;
	}
	case ENCRYPTION_AES:
		std::cout << "AES encryption is not supported." << std::endl;
		[[fallthrough]];
	default:
	{
		// no need to copy the data, just point to it because it's already mapped and the map will be destroyed when the archive is destroyed
		this->m_EntriesData = entriesPtr;
		break;
	}
	}
}

void RPFArchive::GetNameTable()
{
	uint8_t *namesPtr = (uint8_t*)this->m_Map->Data + RPF_HEADER_SIZE + this->m_Header.EntryCount * RPF_ENTRY_SIZE;
	switch (this->m_Header.Encryption)
	{
	case ENCRYPTION_NG:
	{
		this->m_NameTable = this->m_Keyring.get()->DecryptNG(namesPtr, this->m_Header.NameTableSize, this->m_Name, this->m_Map->Size);
		break;
	}
	case ENCRYPTION_AES:
		std::cout << "AES encryption is not supported." << std::endl;
		[[fallthrough]];
	default:
	{
		// same applies here, no need to copy the data just point to it	
		this->m_NameTable = namesPtr;
		break;
	}
	}
}

uint8_t* RPFArchive::ExtractEntry(const RPFFileEntry* entry) const
{
	uint8_t* entryDataStart = (uint8_t*)this->m_Map->Data + 512 * entry->Offset;
	size_t internalSize = entry->CompressedSize;
	uint8_t* entryData = nullptr;

	if (entry->Name.ends_with(".rpf"))
	{
		entryData = new uint8_t[entry->Size];
		memcpy(entryData, entryDataStart, entry->Size);
		return entryData;
	}

	if (entry->IsFrameworkFile)
	{
		entryDataStart += FRAMEWORK_FILE_OFFSET;
		internalSize -= FRAMEWORK_FILE_OFFSET;
	}

	if (!entry->IsFrameworkFile) // apparently framework files arent encrypted kekw
	{
		switch (this->m_Header.Encryption)
		{
		case ENCRYPTION_NG:
		{
			entryData = this->m_Keyring->DecryptNG(entryDataStart, internalSize, entry->Name, entry->Size);
			break;
		}
		case ENCRYPTION_AES:
		{
			std::cout << "AES encryption is not supported." << std::endl;
			return nullptr;
		}
		default:
		{
			entryData = new uint8_t[internalSize];
			memcpy(entryData, entryDataStart, internalSize);
			break;
		}
		}
	}
	else
	{
		entryData = new uint8_t[internalSize];
		memcpy(entryData, entryDataStart, internalSize);
	}

	if (entry->IsCompressed && (!entry->IsFrameworkFile || (entry->IsFrameworkFile && INFLATE_FRAMEWORK_FILES)))
	{
		size_t inflatedSize = entry->Size;

		if (entry->IsFrameworkFile)
		{
			inflatedSize = static_cast<const RPFFrameworkFileEntry*>(entry)->SystemSize;
		}

		z_stream stream;
		memset(&stream, 0, sizeof(stream));
		uint8_t* inflatedData = new uint8_t[inflatedSize];

		stream.next_in = entryData;
		stream.avail_in = internalSize;
		stream.next_out = inflatedData;
		stream.avail_out = inflatedSize;

		int init_result = inflateInit2(&stream, -MAX_WBITS);
		if (init_result != Z_OK)
		{
			delete[] inflatedData;
			delete[] entryData;
			return nullptr;
		}

		int inflate_result = 0;
		do {
			inflate_result = inflate(&stream, Z_NO_FLUSH);

			if (inflate_result < 0) {
				inflateEnd(&stream);
				delete[] inflatedData;
				delete[] entryData;
				return nullptr;
			}
		} while (inflate_result != Z_STREAM_END);

		inflateEnd(&stream);

		delete[] entryData;
		entryData = inflatedData;
	}

	return entryData;
}

RPFArchive* RPFArchive::GetChildArchive(const RPFFileEntry* entry) const
{
	size_t archiveOffset = 512 * entry->Offset;
	MMap* mmap = this->m_Map->Reduce(archiveOffset, entry->Size);
	return new RPFArchive(mmap, entry->GetName(), this->m_Keyring);
}


std::optional<RPFArchive*> RPFArchive::GetArchiveWithPath(std::initializer_list<std::string_view> path, RPFDirEntry* rootDir) const
{
	std::vector<std::string_view> pathVec(path);
	std::span<std::string_view> pathSpan(pathVec);
	return this->GetArchiveWithPath(pathSpan, rootDir);
}

std::optional<RPFArchive*> RPFArchive::GetArchiveWithPath(std::span<std::string_view> path, RPFDirEntry* rootDir) const
{
	assert(path.size() > 0);
	assert(path[path.size() - 1].ends_with(".rpf"));
	auto entry = this->GetEntryWithPath<RPFFileEntry>(path, rootDir);

	if (!entry.has_value())
	{
		return std::nullopt;
	}

	auto archive = this->GetChildArchive(entry.value());
	delete entry.value();
	return archive;
}