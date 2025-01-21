#pragma once

#include <cstdint>
#include <string>
#include "keys.hpp"
#include "NG.hpp"

class Keyring
{
public:
    Keyring(const uint8_t *aesKey, const uint8_t *ngKeys, const uint8_t *lut, const uint32_t *decryptTables);
    uint8_t *DecryptNG(const uint8_t *encryptedData, const size_t dataSize, const std::string_view &name, const size_t entrySize);

    uint8_t AesKey[AES_KEY_SIZE];
    uint8_t NGKeys[NG_KEYS_SIZE1][NG_KEYS_SIZE2];
    uint8_t LUT[LUT_SIZE];
    uint32_t DecryptTables[DECRYPT_TABLES_SIZE1][DECRYPT_TABLES_SIZE2][DECRYPT_TABLES_SIZE3];
};