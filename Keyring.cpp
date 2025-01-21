#include "Keyring.hpp"

Keyring::Keyring(const uint8_t* aesKey, const uint8_t* ngKeys, const uint8_t* lut, const uint32_t* decryptTables)
{
	memcpy(this->AesKey, aesKey, AES_KEY_SIZE);
	memcpy(this->NGKeys, ngKeys, NG_KEYS_SIZE1 * NG_KEYS_SIZE2);
	memcpy(this->LUT, lut, LUT_SIZE);
	memcpy(this->DecryptTables, decryptTables, DECRYPT_TABLES_SIZE1 * DECRYPT_TABLES_SIZE2 * DECRYPT_TABLES_SIZE3 * sizeof(uint32_t));
}

uint8_t* Keyring::DecryptNG(const uint8_t *encryptedData, const size_t dataSize, const std::string_view& name, const size_t entrySize)
{
	return NG::Decrypt(this->LUT, this->DecryptTables, this->NGKeys, encryptedData, dataSize, name, entrySize);
}
