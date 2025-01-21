#pragma once

#include <string>
#include <filesystem>
#include <windows.h>

class MMap
{
public:
    MMap(HANDLE hFile, uint32_t startOffset, size_t size);
    ~MMap();
    MMap *Reduce(uint32_t startOffset, size_t size) const;
    static MMap s_FromFile(const std::filesystem::path &path, uint32_t startOffset = 0, size_t size = 0);
    static MMap *s_FromFileH(const std::filesystem::path &path, uint32_t startOffset = 0, size_t size = 0);

private:
    HANDLE m_FileHandle;
    HANDLE m_MapHandle;
    LPVOID m_Data;
    size_t m_Size;

public:
    const HANDLE &FileHandle = m_FileHandle;
    const HANDLE &MapHandle = m_MapHandle;
    const LPVOID &Data = m_Data;
    const size_t &Size = m_Size;
};