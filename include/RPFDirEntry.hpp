#pragma once

#include <cassert>
#include <cstdint>
#include <array>
#include "endian.hpp"
#include "RPFEntry.hpp"

#define RPF_DIR_ENTRY_SIZE 0x10
#define RPF_DIR_ENTRY_OFFSET 0x7FFFFF

class RPFDirEntry final : public RPFEntry
{
public:
    RPFDirEntry(uint32_t firstEntryIndex, uint32_t entryCount, uint16_t nameOffset);
    uint32_t GetNameOffset() const override;
    std::string ToString() const override;
    RPFEntryType GetType() const override;
    static RPFDirEntry *s_FromBytes(const uint8_t *bytes);

private:
    uint32_t m_FirstEntryIndex;
    uint32_t m_EntryCount;
    uint16_t m_NameOffset;

public:
    const uint32_t &FirstEntryIndex = m_FirstEntryIndex;
    const uint32_t &EntryCount = m_EntryCount;
    const uint16_t &NameOffset = m_NameOffset;
};