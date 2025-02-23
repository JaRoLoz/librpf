#pragma once

#include <cstdint>
#include <vector>
#include <string>

#define RPF_ENTRY_SIZE 0x10

typedef enum
{
    /* should never appear */
    ENTRY_INVALID,
    ENTRY_FILE,
    ENTRY_DIRECTORY
} RPFEntryType;

class RPFEntry
{
public:
    virtual uint32_t GetNameOffset() const;
	virtual const std::string_view& GetName() const;
    virtual std::string ToString() const;
    virtual RPFEntryType GetType() const;
    static RPFEntry *s_FromBytes(const uint8_t* bytes, const uint8_t* data, const uint8_t* nameTable, const uint8_t* entriesData);
    static void s_FreeEntryVector(std::vector<RPFEntry *> &entries);
	static std::string s_GetEntryNameFromTable(const uint8_t* nameTable, uint16_t nameOffset);
};