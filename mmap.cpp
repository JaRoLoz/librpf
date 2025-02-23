#include "mmap.hpp"

MMap MMap::s_FromFile(const std::filesystem::path& path, uint32_t startOffset, size_t size)
{
	HANDLE hFile = CreateFileW(
		path.c_str(),
		GENERIC_READ,
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL
	);

	if (size == 0)
	{
		LARGE_INTEGER fileSize;
		GetFileSizeEx(hFile, &fileSize);
		size = fileSize.QuadPart;
	}

	return MMap(hFile, startOffset, size);
}

MMap* MMap::s_FromFileHeap(const std::filesystem::path& path, uint32_t startOffset, size_t size)
{
	HANDLE hFile = CreateFileW(
		path.c_str(),
		GENERIC_READ,
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL
	);

	if (size == 0)
	{
		LARGE_INTEGER fileSize;
		GetFileSizeEx(hFile, &fileSize);
		size = fileSize.QuadPart;
	}

	return new MMap(hFile, startOffset, size);
}

MMap::MMap(HANDLE hFile, uint32_t startOffset, size_t size) 
	: m_FileHandle(hFile), m_Size(size)
{
	this->m_MapHandle = CreateFileMappingW(
		this->m_FileHandle,
		NULL,
		PAGE_READONLY,
		0,
		0,
		NULL
	);

	SYSTEM_INFO sysInfo;
	GetSystemInfo(&sysInfo);
	DWORD dwSysGran = sysInfo.dwAllocationGranularity;
	DWORD alignedOffset = startOffset & ~(dwSysGran - 1);
	DWORD ptrDelta = startOffset - alignedOffset;

	this->m_Data = MapViewOfFile(
		this->m_MapHandle,
		FILE_MAP_READ,
		0,
		alignedOffset,
		0
	);

	this->m_Data = (LPVOID)((uintptr_t)this->m_Data + ptrDelta);
}

MMap::~MMap()
{
	UnmapViewOfFile(this->m_Data);
	CloseHandle(this->m_MapHandle);
	CloseHandle(this->m_FileHandle);
}

MMap* MMap::Reduce(uint32_t startOffset, size_t size) const
{
	HANDLE hFile;
	DuplicateHandle(
		GetCurrentProcess(),
		this->m_FileHandle,
		GetCurrentProcess(),
		&hFile,
		0,
		FALSE,
		DUPLICATE_SAME_ACCESS
	);

	return new MMap(hFile, startOffset, size);
}