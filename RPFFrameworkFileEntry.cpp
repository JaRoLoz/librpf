#include "RPFFrameworkFileEntry.hpp"

RPFFrameworkFileEntry::RPFFrameworkFileEntry(
	const uint16_t nameOffset,
	const std::string name,
	const uint32_t offset,
	const uint32_t size,
	const uint32_t systemFlags,
	const uint32_t graphicsFlags
) :
	RPFFileEntry(size, offset, systemFlags, nameOffset, std::move(name)),
	m_SystemFlags(systemFlags),
	m_GraphicsFlags(graphicsFlags),
	m_SystemSize(RPFFrameworkFileEntry::s_GetSizeFromFlags(systemFlags)),
	m_GraphicsSize(RPFFrameworkFileEntry::s_GetSizeFromFlags(graphicsFlags)),
    m_Version(RPFFrameworkFileEntry::s_GetSizeVersionFlags(systemFlags, graphicsFlags))
{
	this->m_IsFrameworkFile = true;
}

const std::string_view& RPFFrameworkFileEntry::GetName() const
{
	return this->m_Name;
}

RPFEntryType RPFFrameworkFileEntry::GetType() const
{
	return RPFEntryType::ENTRY_FILE;
}

std::string RPFFrameworkFileEntry::ToString() const
{
	std::ostringstream oss;
	oss << "RPFFrameworkFileEntry {";
	oss << " Name: " << std::string(this->Name) << ",";
	oss << " Offset: " << std::to_string(this->Offset) << ",";
	oss << " Size: " << std::to_string(this->Size) << ",";
	oss << " SystemFlags: " << std::to_string(this->SystemFlags) << ",";
	oss << " GraphicsFlags: " << std::to_string(this->GraphicsFlags) << ",";
	oss << " GraphicsSize: " << std::to_string(this->GraphicsSize) << ",";
	oss << " Version: " << std::to_string(this->Version) << " }";

	return oss.str();
}

uint32_t RPFFrameworkFileEntry::s_GetSizeFromFlags(uint32_t flags)
{
    // dexfx simplified version
    uint32_t s0 = ((flags >> 27) & 0x1) << 0;    // 1 bit  - 27        (*1)
    uint32_t s1 = ((flags >> 26) & 0x1) << 1;    // 1 bit  - 26        (*2)
    uint32_t s2 = ((flags >> 25) & 0x1) << 2;    // 1 bit  - 25        (*4)
    uint32_t s3 = ((flags >> 24) & 0x1) << 3;    // 1 bit  - 24        (*8)
    uint32_t s4 = ((flags >> 17) & 0x7F) << 4;   // 7 bits - 17 - 23   (*16)   (max 127 * 16)
    uint32_t s5 = ((flags >> 11) & 0x3F) << 5;   // 6 bits - 11 - 16   (*32)   (max 63  * 32)
    uint32_t s6 = ((flags >> 7) & 0xF) << 6;     // 4 bits - 7  - 10   (*64)   (max 15  * 64)
    uint32_t s7 = ((flags >> 5) & 0x3) << 7;     // 2 bits - 5  - 6    (*128)  (max 3   * 128)
    uint32_t s8 = ((flags >> 4) & 0x1) << 8;     // 1 bit  - 4         (*256)
    uint32_t ss = ((flags >> 0) & 0xF);          // 4 bits - 0  - 3
    uint32_t baseSize = 0x200U << (int)ss;
    uint32_t size = baseSize * (s0 + s1 + s2 + s3 + s4 + s5 + s6 + s7 + s8);
    return size;
}

uint32_t RPFFrameworkFileEntry::s_GetSizeVersionFlags(uint32_t systemFlags, uint32_t graphicsFlags)
{
    uint32_t sv = (systemFlags >> 28) & 0xF;
    uint32_t gv = (graphicsFlags >> 28) & 0xF;
    return (uint32_t)((sv << 4) + gv);
}