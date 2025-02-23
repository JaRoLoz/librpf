#pragma once

#include <sstream>
#include "RPFFileEntry.hpp"

#define INFLATE_FRAMEWORK_FILES true

class RPFFrameworkFileEntry : public RPFFileEntry
{
public:
	RPFFrameworkFileEntry(
		const uint16_t nameOffset,
		const std::string name,
		const uint32_t offset,
		const uint32_t size,
		const uint32_t systemFlags,
		const uint32_t graphicsFlags
	);
	RPFEntryType GetType() const override;
	const std::string_view& GetName() const override;
	std::string ToString() const override;
private:
	const uint32_t m_SystemFlags;
	const uint32_t m_GraphicsFlags;
	const uint32_t m_SystemSize;
	const uint32_t m_GraphicsSize;
	const uint32_t m_Version;
	uint32_t s_GetSizeFromFlags(uint32_t flags);
	uint32_t s_GetSizeVersionFlags(uint32_t systemFlags, uint32_t graphicsFlags);
public:
	const uint32_t& SystemFlags = m_SystemFlags;
	const uint32_t& GraphicsFlags = m_GraphicsFlags;
	const uint32_t& SystemSize = m_SystemSize;
	const uint32_t& GraphicsSize = m_GraphicsSize;
	const uint32_t& Version = m_Version;
};