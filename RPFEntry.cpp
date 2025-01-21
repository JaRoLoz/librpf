#include "RPFEntry.hpp"
#include "RPFFileEntry.hpp"
#include "RPFDirEntry.hpp"

RPFEntry *RPFEntry::s_FromBytes(const uint8_t* bytes)
{
    uint32_t offset = SWAP_ENDIAN_32(MAKE_UINT32(bytes[5], bytes[6], bytes[7], 0));
    return offset == RPF_DIR_ENTRY_OFFSET
               ? static_cast<RPFEntry*>(RPFDirEntry::s_FromBytes(bytes))
               : static_cast<RPFEntry*>(RPFFileEntry::s_FromBytes(bytes));
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

void RPFEntry::s_FreeEntryVector(std::vector<RPFEntry*>& entries)
{
	for (auto entry : entries)
	{
		delete entry;
	}
}