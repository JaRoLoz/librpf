#pragma once

#include <cmath>
#include <cstdint>
#include <string>
#include "keys.hpp"

#define NG_BLOCK_SIZE 0x10
#define NG_KEY_SIZE_UINT32 68

namespace NG
{
    uint32_t CalculateHash(const uint8_t lut[LUT_SIZE], const std::string_view &name);
    size_t GetNGKeyIndex(const uint8_t lut[LUT_SIZE], const std::string_view &name, const size_t size);
    uint8_t *Decrypt(const uint8_t lut[LUT_SIZE], const uint32_t decryptTables[DECRYPT_TABLES_SIZE1][DECRYPT_TABLES_SIZE2][DECRYPT_TABLES_SIZE3], const uint8_t ngKeys[NG_KEYS_SIZE1][NG_KEYS_SIZE2], const uint8_t *data, const size_t dataSize, const std::string_view &name, const size_t entrySize);
    void DecryptBlock(const uint32_t decryptTables[DECRYPT_TABLES_SIZE1][DECRYPT_TABLES_SIZE2][DECRYPT_TABLES_SIZE3], const uint32_t ngKey[NG_KEY_SIZE_UINT32], uint8_t buffer[NG_BLOCK_SIZE]);
    void DecryptRoundA(const uint32_t decryptTable[DECRYPT_TABLES_SIZE2][DECRYPT_TABLES_SIZE3], const uint32_t ngKey[NG_KEY_SIZE_UINT32], uint8_t buffer[NG_BLOCK_SIZE]);
    void DecryptRoundB(const uint32_t decryptTable[DECRYPT_TABLES_SIZE2][DECRYPT_TABLES_SIZE3], const uint32_t ngKey[NG_KEY_SIZE_UINT32], uint8_t buffer[NG_BLOCK_SIZE]);
};