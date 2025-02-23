#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <span>
#include <optional>
#include <memory>
#include <zlib.h>
#include "mmap.hpp"
#include "Keyring.hpp"
#include "RPFHeader.hpp"
#include "RPFEntry.hpp"
#include "RPFDirEntry.hpp"
#include "RPFFileEntry.hpp"
#include "RPFFrameworkFileEntry.hpp"

class RPFArchive
{
public:
    RPFArchive(const std::filesystem::path &path, const std::string_view &name, const std::shared_ptr<Keyring> keyring);
    RPFArchive(const MMap *mmap, const std::string_view &name, const std::shared_ptr<Keyring> keyring);
    ~RPFArchive();
    std::string ToString() const;
    std::vector<RPFEntry *> GetDirEntries(const RPFDirEntry *entry) const;
    uint8_t *ExtractEntry(const RPFFileEntry *entry) const;
    RPFArchive *GetChildArchive(const RPFFileEntry *entry) const;
    template <typename T>
    std::optional<T *> GetEntryWithPath(std::span<std::string_view> path, RPFDirEntry *rootDir) const;
    template <typename T>
    std::optional<T *> GetEntryWithPath(std::initializer_list<std::string_view> path, RPFDirEntry *rootDir) const;
    std::optional<RPFArchive *> GetArchiveWithPath(std::span<std::string_view> path, RPFDirEntry *rootDir) const;
    std::optional<RPFArchive *> GetArchiveWithPath(std::initializer_list<std::string_view> path, RPFDirEntry *rootDir) const;

private:
    void GetEntries();
    void GetNameTable();
    uint8_t *m_EntriesData;
    uint8_t *m_NameTable;
    const MMap *m_Map;
    const RPFHeader m_Header;
    const std::string_view m_Name;
    const std::shared_ptr<Keyring> m_Keyring;

public:
    const RPFHeader &Header = m_Header;
    const std::string_view &Name = m_Name;
    RPFDirEntry *Root;
};

template <typename T>
std::optional<T *> RPFArchive::GetEntryWithPath(std::span<std::string_view> path, RPFDirEntry *rootDir) const
{
    size_t pathSize = path.size();
    assert(pathSize > 0);
    RPFEntry *searchedEntry = nullptr;
    RPFEntryType searchedEntryType = pathSize == 1
                                         ? std::is_same_v<T, RPFFileEntry>
                                               ? RPFEntryType::ENTRY_FILE
                                               : RPFEntryType::ENTRY_DIRECTORY
                                         : RPFEntryType::ENTRY_DIRECTORY;
    auto entries = this->GetDirEntries(rootDir);
    for (auto entry : entries)
    {
        auto aa = entry->GetName();
        if (entry != nullptr && path[0] == entry->GetName() && entry->GetType() == searchedEntryType)
        {
            searchedEntry = entry;
        }
        else
        {
            delete entry;
        }
    }

    if (searchedEntry == nullptr)
    {
        return std::nullopt;
    }
    else
    {
        if (pathSize == 1)
        {
            return dynamic_cast<T *>(searchedEntry);
        }
        else
        {
            auto subEntry = GetEntryWithPath<T>(path.subspan(1), dynamic_cast<RPFDirEntry *>(searchedEntry));
            delete searchedEntry;
            return subEntry;
        }
    }
}

template <typename T>
std::optional<T *> RPFArchive::GetEntryWithPath(std::initializer_list<std::string_view> path, RPFDirEntry *rootDir) const
{
    std::vector<std::string_view> pathVec(path);
    std::span<std::string_view> pathSpan(pathVec);
    return GetEntryWithPath<T>(pathSpan, rootDir);
}

template std::optional<RPFDirEntry *> RPFArchive::GetEntryWithPath<RPFDirEntry>(std::span<std::string_view> path, RPFDirEntry *rootDir) const;
template std::optional<RPFFileEntry *> RPFArchive::GetEntryWithPath<RPFFileEntry>(std::span<std::string_view> path, RPFDirEntry *rootDir) const;