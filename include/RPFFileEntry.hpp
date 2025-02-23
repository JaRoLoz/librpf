#pragma once

#include <cstdint>
#include <array>
#include "endian.hpp"
#include "RPFEntry.hpp"

#define RPF_FILE_ENTRY_SIZE 0x10
#define FRAMEWORK_FILE_OFFSET 0x10
#define FRAMEWORK_FILE_FLAG 0x80000000

#define CHECK_FRAMEWORK_FILE_OFFSET_FLAG(offset) ((offset & FRAMEWORK_FILE_FLAG) != 0)

class RPFFileEntry : public RPFEntry
{
public:
    RPFFileEntry(uint32_t flags, uint32_t offset, uint32_t size, uint16_t nameOffset, const std::string name);
    static RPFFileEntry *s_FromBytes(const uint8_t *bytes, const uint8_t* data, const uint8_t* nameTable, const uint8_t* entriesData);
    uint32_t GetNameOffset() const override;
    std::string ToString() const override;
    RPFEntryType GetType() const override;
	const std::string_view& GetName() const override;

protected:
    const uint32_t m_Flags;
    const uint32_t m_Offset;
    const uint32_t m_Size;
    const uint16_t m_NameOffset;
    const std::string m_Name;
	bool m_IsFrameworkFile;

public:
    const uint32_t &Flags = m_Flags;
    const uint32_t &Offset = m_Offset;
    const uint32_t &Size = m_Size;
    const bool& IsCompressed = this->m_Flags != 0;
    const uint32_t& CompressedSize = (this->m_Flags != 0) ? this->Flags : this->Size;
    const bool& IsFrameworkFile = m_IsFrameworkFile;
    const uint16_t &NameOffset = m_NameOffset;
	const std::string_view& Name = m_Name;
};