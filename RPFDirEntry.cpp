#include "RPFDirEntry.hpp"

RPFDirEntry::RPFDirEntry(uint32_t firstEntryIndex, uint32_t entryCount, uint16_t nameOffset)
	: m_FirstEntryIndex(firstEntryIndex), m_EntryCount(entryCount), m_NameOffset(nameOffset)
{
}

RPFDirEntry *RPFDirEntry::s_FromBytes(const uint8_t* bytes)
{
	uint32_t offset = SWAP_ENDIAN_32(MAKE_UINT32(bytes[5], bytes[6], bytes[7], 0));
	assert(offset == RPF_DIR_ENTRY_OFFSET);

	uint16_t name_offset = SWAP_ENDIAN_16(MAKE_UINT16(bytes[0], bytes[1]));
	uint32_t first_entry_index = SWAP_ENDIAN_32(MAKE_UINT32(bytes[8], bytes[9], bytes[10], bytes[11]));
	uint32_t entry_count = SWAP_ENDIAN_32(MAKE_UINT32(bytes[12], bytes[13], bytes[14], bytes[15]));

	return new RPFDirEntry(first_entry_index, entry_count, name_offset);
}

uint32_t RPFDirEntry::GetNameOffset() const
{
	return this->NameOffset;
}

RPFEntryType RPFDirEntry::GetType() const
{
	return RPFEntryType::ENTRY_DIRECTORY;
}

std::string RPFDirEntry::ToString() const
{
	return "RPFDirEntry: { FirstEntryIndex: " + std::to_string(this->FirstEntryIndex) + ", EntryCount: " + std::to_string(this->EntryCount) + ", NameOffset: " + std::to_string(this->NameOffset) + " }";
}