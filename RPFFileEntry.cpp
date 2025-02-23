#include "RPFFileEntry.hpp"
#include "RPFFrameworkFileEntry.hpp"

RPFFileEntry::RPFFileEntry(uint32_t flags, uint32_t offset, uint32_t size, uint16_t nameOffset, const std::string name) :
	m_Flags(flags),
	m_Offset(offset),
	m_Size(size),
	m_NameOffset(nameOffset),
	m_Name(std::move(name)),
	m_IsFrameworkFile(false)
{
}

RPFFileEntry *RPFFileEntry::s_FromBytes(const uint8_t* bytes, const uint8_t* data, const uint8_t* nameTable, const uint8_t* entriesData)
{
	uint32_t rawFlags = SWAP_ENDIAN_32(MAKE_UINT32(bytes[4], bytes[5], bytes[6], bytes[7]));
	uint16_t nameOffset = SWAP_ENDIAN_16(MAKE_UINT16(bytes[0], bytes[1]));
	uint32_t bytes8to11 = SWAP_ENDIAN_32(MAKE_UINT32(bytes[8], bytes[9], bytes[10], bytes[11]));
	std::string name = RPFEntry::s_GetEntryNameFromTable(nameTable, nameOffset);

	if (CHECK_FRAMEWORK_FILE_OFFSET_FLAG(rawFlags))
	{
		uint32_t size = SWAP_ENDIAN_32(MAKE_UINT32(bytes[2], bytes[3], bytes[4], 0));
		uint32_t offset = SWAP_ENDIAN_32(MAKE_UINT32(bytes[5], bytes[6], bytes[7], 0));
		uint32_t graphicsFlags = SWAP_ENDIAN_32(MAKE_UINT32(bytes[12], bytes[13], bytes[14], bytes[15]));

		if (size == 0xFFFFFF) // real file size >= 0xFFFFFF (haven't seen any case where it happens, but just in case)
		{
			const uint8_t* entryDataStart = data + 512 * offset;
			size = MAKE_UINT32(entryDataStart[2], entryDataStart[5], entryDataStart[14], entryDataStart[7]);
		}
		return new RPFFrameworkFileEntry(nameOffset, name, offset, size, bytes8to11, graphicsFlags);
	}
	else
	{
		uint32_t flags = SWAP_ENDIAN_32(MAKE_UINT32(bytes[2], bytes[3], bytes[4], 0));
		uint32_t offset = SWAP_ENDIAN_32(MAKE_UINT32(bytes[5], bytes[6], bytes[7], 0)) & 0x7FFFFF;
		return new RPFFileEntry(flags, offset, bytes8to11, nameOffset, name);
	}

}

uint32_t RPFFileEntry::GetNameOffset() const
{
	return this->NameOffset;
}

RPFEntryType RPFFileEntry::GetType() const
{
	return RPFEntryType::ENTRY_FILE;
}

const std::string_view& RPFFileEntry::GetName() const
{
	return this->Name;
}

std::string RPFFileEntry::ToString() const
{
	return "RPFFileEntry: { Flags: " + std::to_string(this->Flags) + ", Offset: " + std::to_string(this->Offset) + ", Size: " + std::to_string(this->Size) + ", NameOffset: " + std::to_string(this->NameOffset) + " }";
}
