#include "filesystem_interface.h"
#include <fstream>
#include <spdlog/spdlog.h>
#include <sstream>

namespace felztrace
{

bool RealFilesystem::exists(const std::filesystem::path& path) const
{
    return std::filesystem::exists(path);
}

std::filesystem::path RealFilesystem::weakly_canonical(const std::filesystem::path& path) const
{
    return std::filesystem::weakly_canonical(path);
}

bool RealFilesystem::create_directories(const std::filesystem::path& path,
                                        std::error_code& errorCode) const
{
    return std::filesystem::create_directories(path, errorCode);
}

bool RealFilesystem::is_regular_file(const std::filesystem::path& path) const
{
    return std::filesystem::is_regular_file(path);
}

void RealFilesystem::remove_all(const std::filesystem::path& path, std::error_code& errorCode) const
{
    std::filesystem::remove_all(path, errorCode);
}

bool RealFilesystem::equivalent(const std::filesystem::path& path1,
                                const std::filesystem::path& path2) const
{
    std::error_code ec;
    bool result = std::filesystem::equivalent(path1, path2, ec);
    // If ec is set, paths don't exist or can't be compared - return false
    return !ec && result;
}

// NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
std::vector<std::filesystem::path>
RealFilesystem::listFilesWithExtensionAndName(const std::filesystem::path& root,
                                              const FileExtension& extension,
                                              const FileName& filename) const
{
    std::vector<std::filesystem::path> files;
    for (const auto& entry : std::filesystem::recursive_directory_iterator(root))
    {
        if (entry.is_regular_file())
        {
            const auto& path = entry.path();

            // Check extension
            if (path.extension() != extension.value)
            {
                continue;
            }

            // If filename specified, check for exact match
            if (!filename.value.empty() && path.filename() != filename.value)
            {
                continue;
            }

            files.push_back(path);
        }
    }
    return files;
}

std::string RealFilesystem::readFile(const std::filesystem::path& path) const
{
    std::ifstream file(path);
    if (!file.is_open())
    {
        throw std::runtime_error("Failed to read file: " + path.string());
    }
    std::ostringstream ss;
    ss << file.rdbuf();
    return ss.str();
}

void RealFilesystem::writeFile(const std::filesystem::path& path, const std::string& content) const
{
    std::ofstream file(path);
    if (!file)
    {
        throw std::runtime_error("Failed to write file: " + path.string());
    }
    file << content;
}

std::filesystem::path RealFilesystem::findGitRoot(const std::filesystem::path& startPath) const
{
    spdlog::debug("Searching for git root starting from: '{}'", startPath.string());

    std::filesystem::path gitRoot = weakly_canonical(startPath);
    while (true)
    {
        if (exists(gitRoot / ".git"))
        {
            spdlog::debug("Found git root at: '{}'", gitRoot.string());
            return gitRoot;
        }

        std::filesystem::path parent = gitRoot.parent_path();
        if (parent == gitRoot) // Reached root directory
        {
            break;
        }
        gitRoot = parent;
    }

    throw std::runtime_error("No git root found starting from: " + startPath.string());
}

} // namespace felztrace
