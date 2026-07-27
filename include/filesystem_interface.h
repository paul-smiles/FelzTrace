#pragma once

#include <filesystem>
#include <fstream>
#include <string>
#include <utility>
#include <vector>

namespace felztrace
{

struct FileExtension
{
    std::string value;

    explicit FileExtension(std::string valueIn) : value(std::move(valueIn)) {}

    friend bool operator==(const FileExtension& lhs, const FileExtension& rhs)
    {
        return lhs.value == rhs.value;
    }
};

struct FileName
{
    std::string value;

    FileName() = default;
    explicit FileName(std::string valueIn) : value(std::move(valueIn)) {}

    friend bool operator==(const FileName& lhs, const FileName& rhs)
    {
        return lhs.value == rhs.value;
    }
};

// Simple filesystem interface for dependency injection in tests
class IFilesystem
{
  public:
    virtual ~IFilesystem() = default;

    virtual bool exists(const std::filesystem::path& path) const = 0;
    virtual std::filesystem::path weakly_canonical(const std::filesystem::path& path) const = 0;
    virtual bool create_directories(const std::filesystem::path& path,
                                    std::error_code& errorCode) const = 0;
    virtual bool is_regular_file(const std::filesystem::path& path) const = 0;
    virtual void remove_all(const std::filesystem::path& path,
                            std::error_code& errorCode) const = 0;
    virtual bool equivalent(const std::filesystem::path& path1,
                            const std::filesystem::path& path2) const = 0;

    // For directory iteration
    virtual std::vector<std::filesystem::path>
    listFilesWithExtensionAndName(const std::filesystem::path& root, const FileExtension& extension,
                                  const FileName& filename = FileName()) const = 0;

    // For file I/O
    virtual std::string readFile(const std::filesystem::path& path) const = 0;
    virtual void writeFile(const std::filesystem::path& path, const std::string& content) const = 0;

    // Git operations
    virtual std::filesystem::path findGitRoot(const std::filesystem::path& startPath) const = 0;
};

// Real filesystem implementation (defined in real_filesystem.cpp)
class RealFilesystem : public IFilesystem
{
  public:
    bool exists(const std::filesystem::path& path) const override;
    std::filesystem::path weakly_canonical(const std::filesystem::path& path) const override;
    bool create_directories(const std::filesystem::path& path,
                            std::error_code& errorCode) const override;
    bool is_regular_file(const std::filesystem::path& path) const override;
    void remove_all(const std::filesystem::path& path, std::error_code& errorCode) const override;
    bool equivalent(const std::filesystem::path& path1,
                    const std::filesystem::path& path2) const override;
    std::vector<std::filesystem::path>
    listFilesWithExtensionAndName(const std::filesystem::path& root, const FileExtension& extension,
                                  const FileName& filename = FileName()) const override;
    std::string readFile(const std::filesystem::path& path) const override;
    void writeFile(const std::filesystem::path& path, const std::string& content) const override;
    std::filesystem::path findGitRoot(const std::filesystem::path& startPath) const override;
};

} // namespace felztrace
