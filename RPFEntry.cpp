#include "RPFEntry.hpp"
#include "RPFFileEntry.hpp"
#include "RPFDirEntry.hpp"

RPFEntry *RPFEntry::s_FromBytes(const uint8_t* bytes, const uint8_t* data, const uint8_t* nameTable, const uint8_t* entriesData)
{
    uint32_t offset = SWAP_ENDIAN_32(MAKE_UINT32(bytes[5], bytes[6], bytes[7], 0));
    return offset == RPF_DIR_ENTRY_OFFSET
               ? static_cast<RPFEntry*>(RPFDirEntry::s_FromBytes(bytes, nameTable))
               : static_cast<RPFEntry*>(RPFFileEntry::s_FromBytes(bytes, data, nameTable, entriesData));
}

uint32_t RPFEntry::GetNameOffset() const
{
	return 0;
}

RPFEntryType RPFEntry::GetType() const
{
	return RPFEntryType::ENTRY_INVALID;
}

std::string RPFEntry::ToString() const
{
	return "RPFEntry {}";
}

const std::string_view& RPFEntry::GetName() const
{
	return "";
}

void RPFEntry::s_FreeEntryVector(std::vector<RPFEntry*>& entries)
{
	for (auto entry : entries)
	{
		delete entry;
	}
}

std::string RPFEntry::s_GetEntryNameFromTable(const uint8_t* nameTable, uint16_t nameOffset)
{
	const uint8_t* entryNameStart = nameTable + nameOffset;
	std::string name;

	while (true)
	{
		uint8_t c = *entryNameStart++;
		if (c == '\0')
			break;
		name += c;
	}

	return name;
}