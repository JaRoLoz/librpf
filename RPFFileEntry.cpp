#include "RPFFileEntry.hpp"

RPFFileEntry::RPFFileEntry(uint32_t flags, uint32_t offset, uint32_t size, uint16_t nameOffset)
	: m_Flags(flags), m_Offset(offset), m_Size(size), m_NameOffset(nameOffset)
{
}

RPFFileEntry *RPFFileEntry::s_FromBytes(const uint8_t* bytes)
{
	uint16_t name_offset = SWAP_ENDIAN_16(MAKE_UINT16(bytes[0], bytes[1]));
	uint32_t flags = SWAP_ENDIAN_32(MAKE_UINT32(bytes[2], bytes[3], bytes[4], 0));
	uint32_t offset = SWAP_ENDIAN_32(MAKE_UINT32(bytes[5], bytes[6], bytes[7], 0));
	uint32_t size = SWAP_ENDIAN_32(MAKE_UINT32(bytes[8], bytes[9], bytes[10], bytes[11]));

	return new RPFFileEntry(flags, offset & 0x7FFFFF, size, name_offset);
}

uint32_t RPFFileEntry::GetNameOffset() const
{
	return this->NameOffset;
}

RPFEntryType RPFFileEntry::GetType() const
{
	return RPFEntryType::ENTRY_FILE;
}

bool RPFFileEntry::IsCompressed() const
{
	return this->Flags != 0;
}

size_t RPFFileEntry::InternalSize() const
{
	return this->IsCompressed() ? this->Flags : this->Size;
}

std::string RPFFileEntry::ToString() const
{
	return "RPFFileEntry: { Flags: " + std::to_string(this->Flags) + ", Offset: " + std::to_string(this->Offset) + ", Size: " + std::to_string(this->Size) + ", NameOffset: " + std::to_string(this->NameOffset) + " }";
}