#include "RPFHeader.hpp"

RPFHeader::RPFHeader(RPFEncryption encryption, uint32_t entryCount, uint32_t nameTableSize)
	: m_Encryption(encryption), m_EntryCount(entryCount), m_NameTableSize(nameTableSize)
{
}

RPFHeader RPFHeader::s_FromBytes(const uint8_t* bytes)
{
	assert(memcmp(bytes, RPF_MAGIC, 4) == 0);
	uint32_t entry_count = SWAP_ENDIAN_32(MAKE_UINT32(bytes[4], bytes[5], bytes[6], bytes[7]));
	uint32_t names_table_size = SWAP_ENDIAN_32(MAKE_UINT32(bytes[8], bytes[9], bytes[10], bytes[11]));
	RPFEncryption encryption = RPFEncryption::ENCRYPTION_NONE;
	switch (SWAP_ENDIAN_32(MAKE_UINT32(bytes[12], bytes[13], bytes[14], bytes[15])))
	{
	case RPFEncryption::ENCRYPTION_AES:
		encryption = RPFEncryption::ENCRYPTION_AES;
		break;
	case RPFEncryption::ENCRYPTION_NG:
		encryption = RPFEncryption::ENCRYPTION_NG;
		break;
	}

	return RPFHeader(encryption, entry_count, names_table_size);
}

std::string RPFHeader::ToString() const
{
	std::string encryption;
	switch (m_Encryption)
	{
	case RPFEncryption::ENCRYPTION_AES:
		encryption = "AES";
		break;
	case RPFEncryption::ENCRYPTION_NG:
		encryption = "NG";
		break;
	case RPFEncryption::ENCRYPTION_NONE:
		encryption = "NONE";
		break;
	}

	return "RPFHeader { Encryption: " + encryption + ", EntryCount: " + std::to_string(m_EntryCount) + ", NameTableSize: " + std::to_string(m_NameTableSize) + " }";
}	