#include "NG.hpp"

uint32_t NG::CalculateHash(const uint8_t lut[LUT_SIZE], const std::string_view& name)
{
	uint32_t result = 0;
	for (size_t i = 0; i < name.size(); i++)
	{
		uint8_t lutIndex = name[i];
		uint32_t temp = static_cast<uint32_t>(1025) * (static_cast<uint32_t>(lut[lutIndex]) + result);
		result = (temp >> 6) ^ temp & 0xFFFFFFFF;
	}

	return static_cast<uint32_t>(32769) * (((static_cast<uint32_t>(9) * result) >> 11) ^ (static_cast<uint32_t>(9) * result));
}

size_t NG::GetNGKeyIndex(const uint8_t lut[LUT_SIZE], const std::string_view& name, const size_t size)
{
	size_t hash = (size_t) CalculateHash(lut, name);
	size_t index = (hash + size + ((size_t)101 - 40)) % 0x65;
	return index;
}

void NG::DecryptRoundA
(
	const uint32_t decryptTable[DECRYPT_TABLES_SIZE2][DECRYPT_TABLES_SIZE3],
	const uint32_t ngKey[NG_KEY_SIZE_UINT32],
	uint8_t buffer[NG_BLOCK_SIZE]
)
{
	uint32_t x0 = decryptTable[0][buffer[0]]
		^ decryptTable[1][buffer[1]]
		^ decryptTable[2][buffer[2]]
		^ decryptTable[3][buffer[3]]
		^ ngKey[0];
	uint32_t x1 = decryptTable[4][buffer[4]]
		^ decryptTable[5][buffer[5]]
		^ decryptTable[6][buffer[6]]
		^ decryptTable[7][buffer[7]]
		^ ngKey[1];
	uint32_t x2 = decryptTable[8][buffer[8]]
		^ decryptTable[9][buffer[9]]
		^ decryptTable[10][buffer[10]]
		^ decryptTable[11][buffer[11]]
		^ ngKey[2];
	uint32_t x3 = decryptTable[12][buffer[12]]
		^ decryptTable[13][buffer[13]]
		^ decryptTable[14][buffer[14]]
		^ decryptTable[15][buffer[15]]
		^ ngKey[3];

	uint32_t* buffer32 = reinterpret_cast<uint32_t*>(buffer);
	buffer32[0] = x0;
	buffer32[1] = x1;
	buffer32[2] = x2;
	buffer32[3] = x3;
}

void NG::DecryptRoundB
(
	const uint32_t decryptTable[DECRYPT_TABLES_SIZE2][DECRYPT_TABLES_SIZE3],
	const uint32_t ngKey[NG_KEY_SIZE_UINT32],
	uint8_t buffer[NG_BLOCK_SIZE]
)
{
	uint32_t x0 = decryptTable[0][buffer[0]]
		^ decryptTable[7][buffer[7]]
		^ decryptTable[10][buffer[10]]
		^ decryptTable[13][buffer[13]]
		^ ngKey[0];
	uint32_t x1 = decryptTable[1][buffer[1]]
		^ decryptTable[4][buffer[4]]
		^ decryptTable[11][buffer[11]]
		^ decryptTable[14][buffer[14]]
		^ ngKey[1];
	uint32_t x2 = decryptTable[2][buffer[2]]
		^ decryptTable[5][buffer[5]]
		^ decryptTable[8][buffer[8]]
		^ decryptTable[15][buffer[15]]
		^ ngKey[2];
	uint32_t x3 = decryptTable[3][buffer[3]]
		^ decryptTable[6][buffer[6]]
		^ decryptTable[9][buffer[9]]
		^ decryptTable[12][buffer[12]]
		^ ngKey[3];

	uint32_t* buffer32 = reinterpret_cast<uint32_t*>(buffer);
	buffer32[0] = x0;
	buffer32[1] = x1;
	buffer32[2] = x2;
	buffer32[3] = x3;
}

void NG::DecryptBlock
(
	const uint32_t decryptTables[DECRYPT_TABLES_SIZE1][DECRYPT_TABLES_SIZE2][DECRYPT_TABLES_SIZE3], 
	const uint32_t ngKey[NG_KEY_SIZE_UINT32], 
	uint8_t buffer[NG_BLOCK_SIZE]
)
{
	uint32_t subKeys[17][4] = { 0 };
	for (size_t i = 0; i < 17; i++)
	{
		subKeys[i][0] = ngKey[i * 4];
		subKeys[i][1] = ngKey[i * 4 + 1];
		subKeys[i][2] = ngKey[i * 4 + 2];
		subKeys[i][3] = ngKey[i * 4 + 3];
	}

	DecryptRoundA(decryptTables[0], subKeys[0], buffer);
	DecryptRoundA(decryptTables[1], subKeys[1], buffer);

	for (size_t i = 2; i < 16; i++)
	{
		DecryptRoundB(decryptTables[i], subKeys[i], buffer);
	}

	DecryptRoundA(decryptTables[16], subKeys[16], buffer);
}

uint8_t* NG::Decrypt
(
	const uint8_t lut[LUT_SIZE],
	const uint32_t decryptTables[DECRYPT_TABLES_SIZE1][DECRYPT_TABLES_SIZE2][DECRYPT_TABLES_SIZE3],
	const uint8_t ngKeys[NG_KEYS_SIZE1][NG_KEYS_SIZE2],
	const uint8_t* data,
	const size_t dataSize,
	const std::string_view& name,
	const size_t entrySize
)
{
	size_t ngKeyIndex = GetNGKeyIndex(lut, name, entrySize);
	const uint32_t* ngKey = reinterpret_cast<const uint32_t*>(ngKeys[ngKeyIndex]); // size uint32_t[68]
	uint8_t* decryptedData = new uint8_t[dataSize];
	memcpy(decryptedData, data, dataSize);

	for (size_t i = 0; i < (dataSize / NG_BLOCK_SIZE); i++)
	{
		size_t offset = i * NG_BLOCK_SIZE;
		uint8_t* decryptedPtr = decryptedData + offset;

		DecryptBlock(decryptTables, ngKey, decryptedPtr);
	}

	return decryptedData;
}