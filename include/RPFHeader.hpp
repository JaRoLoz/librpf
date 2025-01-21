#pragma once

#include <cstdint>
#include <cstring>
#include <cassert>
#include <array>
#include <string>
#include "endian.hpp"

#define RPF_HEADER_SIZE 0x10
#define RPF_MAGIC "7FPR"

typedef enum RPFEncryption
{
    ENCRYPTION_NONE = 0x0,
    ENCRYPTION_AES = 0x0FFFFFF9,
    ENCRYPTION_NG = 0x0FEFFFFF
};

class RPFHeader
{
public:
    RPFHeader(RPFEncryption encryption, uint32_t entryCount, uint32_t nameTableSize);
    static RPFHeader s_FromBytes(const uint8_t *bytes);
    std::string ToString() const;

private:
    const RPFEncryption m_Encryption;
    const uint32_t m_EntryCount;
    const uint32_t m_NameTableSize;

public:
    const RPFEncryption &Encryption = m_Encryption;
    const uint32_t &EntryCount = m_EntryCount;
    const uint32_t &NameTableSize = m_NameTableSize;
};