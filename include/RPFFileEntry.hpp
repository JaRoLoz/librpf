#pragma once

#include <cstdint>
#include <array>
#include "endian.hpp"
#include "RPFEntry.hpp"

#define RPF_FILE_ENTRY_SIZE 0x10

class RPFFileEntry final : public RPFEntry
{
public:
    RPFFileEntry(uint32_t flags, uint32_t offset, uint32_t size, uint16_t nameOffset);
    static RPFFileEntry *s_FromBytes(const uint8_t *bytes);
    uint32_t GetNameOffset() const override;
    std::string ToString() const override;
    RPFEntryType GetType() const override;
    bool IsCompressed() const;
    size_t InternalSize() const;

private:
    uint32_t m_Flags;
    uint32_t m_Offset;
    uint32_t m_Size;
    uint16_t m_NameOffset;

public:
    const uint32_t &Flags = m_Flags;
    const uint32_t &Offset = m_Offset;
    const uint32_t &Size = m_Size;
    const uint16_t &NameOffset = m_NameOffset;
};