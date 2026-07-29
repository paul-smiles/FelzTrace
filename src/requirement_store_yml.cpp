#include "requirement_store_yml.h"
#include <filesystem>
#include <fstream>
#include <spdlog/spdlog.h>
#include <stdexcept>
#include <string_view>
#include <system_error>
#include <yaml-cpp/yaml.h>

namespace felztrace
{

namespace
{
constexpr std::string_view configFilename = ".felztrace.yml";

std::string_view storeTypeToString(StoreType storeType)
{
    switch (storeType)
    {
    case StoreType::Requirements:
        return "reqs";
    case StoreType::Tests:
        return "tests";
    default:
        spdlog::error("Unsupported store type value in storeTypeToString");
        throw std::invalid_argument("Unsupported store type");
    }
}
// void generateRequirement(const std::string& storeName, const std::filesystem::path& storePath,
//                          IFilesystem* filesystem)
// {
//     // TODO: Implement requirement generation logic, e.g., create a new requirement with a unique
//     ID
//     // and add it to the YAML file at storePath. find requirement ID with the highest number and
//     // increment it for the new requirement.
//     auto yamlFiles = filesystem->listFilesWithExtensionAndName(storePath, FileExtension(".yml"));
//     int maxId = 0;
//     for (const auto& yamlPath : yamlFiles)
//     {
//         std::string filename = yamlPath.stem().string(); // Get filename without extension
//         if (filename.find(storeName) == 0)
//         { // Check if filename starts with storeName
//             std::string idStr = filename.substr(storeName.length());
//             try
//             {
//                 int id = std::stoi(idStr);
//                 if (id > maxId)
//                 {
//                     maxId = id;
//                 }
//             }
//             catch (const std::exception&)
//             {
//                 // Skip files that don't have a valid numeric ID
//             }
//         }
//     }
// }
} // namespace

RequirementStoreYml::RequirementStoreYml(std::unique_ptr<IFilesystem> filesystem)
    : m_filesystem(filesystem ? std::move(filesystem) : std::make_unique<RealFilesystem>())
{
}

void RequirementStoreYml::createRequirementStore(const std::string& name, const std::string& path,
                                                 int level)
{
    StoreSettings storeSettings{name, path, level, StoreType::Requirements};
    createStore(storeSettings);
}

void RequirementStoreYml::createTestStore(const std::string& name, const std::string& path,
                                          int level)
{
    StoreSettings storeSettings{name, path, level, StoreType::Tests};
    createStore(storeSettings);
}

void RequirementStoreYml::createStore(const StoreSettings& storeSettings)
{
    const std::string storeTypeName{storeTypeToString(storeSettings.storeType)};

    spdlog::info("Creating \nstore: '{}'\ntype: '{}'\npath: '{}'\nlevel: {}",
                 storeSettings.storeName, storeTypeName, storeSettings.path, storeSettings.level);

    std::filesystem::path gitRoot = m_filesystem->findGitRoot(storeSettings.path);

    spdlog::debug("Searching for duplicate storeName from git root: '{}'", gitRoot.string());

    // Search git root for all .felztrace.yml files and check if one has the storeName == name
    // already
    auto yamlFiles = m_filesystem->listFilesWithExtensionAndName(gitRoot, FileExtension(".yml"),
                                                                 FileName(".felztrace.yml"));
    for (const auto& yamlPath : yamlFiles)
    {
        std::string yamlContent = m_filesystem->readFile(yamlPath);
        YAML::Node existingNode = YAML::Load(yamlContent);
        if (existingNode["store"] &&
            existingNode["store"].as<std::string>() == storeSettings.storeName)
        {
            throw std::runtime_error(storeTypeName + " store with storeName '" +
                                     storeSettings.storeName + "' already exists at path '" +
                                     yamlPath.string() + "'");
        }
    }

    spdlog::debug("Check for store at specified path and create directory if needed.");

    std::filesystem::path storePath = m_filesystem->weakly_canonical(storeSettings.path);

    // Guard against creating stores at git root
    if (m_filesystem->equivalent(storePath, gitRoot))
    {
        throw std::runtime_error("Cannot create " + storeTypeName + " store at git root '" +
                                 storePath.string() +
                                 "'. Stores must be in a subdirectory to prevent "
                                 "accidental deletion of the repository.");
    }

    // Guard against creating stores in directories containing .git
    if (m_filesystem->exists(storePath / ".git"))
    {
        throw std::runtime_error("Cannot create " + storeTypeName + " store at '" +
                                 storePath.string() +
                                 "': directory contains .git repository. Stores must be in a "
                                 "subdirectory.");
    }

    std::filesystem::path yamlFilePath =
        m_filesystem->weakly_canonical(std::filesystem::path(storeSettings.path) / configFilename);
    if (m_filesystem->exists(yamlFilePath))
    {
        throw std::filesystem::filesystem_error(storeTypeName + " store YAML file already exists",
                                                yamlFilePath,
                                                std::make_error_code(std::errc::file_exists));
    }

    std::error_code errorCode;
    m_filesystem->create_directories(storeSettings.path, errorCode);
    if (errorCode)
    {
        throw std::filesystem::filesystem_error(
            "Error creating " + storeTypeName + " store directory", storeSettings.path, errorCode);
    }

    spdlog::debug("Directory created, write yaml configuration file.");

    // Update storeName with the provided name
    YAML::Node storeSettingsNode;
    storeSettingsNode["store"] = storeSettings.storeName;
    storeSettingsNode["settings"]["type"] = storeTypeName;
    storeSettingsNode["settings"]["level"] = storeSettings.level;

    m_filesystem->writeFile(yamlFilePath, YAML::Dump(storeSettingsNode) + "\n");

    spdlog::info("Store '{}' created successfully at '{}'", storeSettings.storeName,
                 yamlFilePath.string());
}

void RequirementStoreYml::deleteStore(const std::string& name)
{
    spdlog::info("Deleting store '{}'", name);

    std::filesystem::path gitRoot = m_filesystem->findGitRoot(std::filesystem::current_path());
    spdlog::debug("Searching for .felztrace.yml file with matching storeName in settings starting "
                  "from git root '{}'",
                  gitRoot.string());
    auto yamlFiles = m_filesystem->listFilesWithExtensionAndName(gitRoot, FileExtension(".yml"),
                                                                 FileName(".felztrace.yml"));
    for (const auto& yamlPath : yamlFiles)
    {
        std::string yamlContent = m_filesystem->readFile(yamlPath);
        YAML::Node existingNode = YAML::Load(yamlContent);
        if (existingNode["store"] && existingNode["store"].as<std::string>() == name)
        {
            spdlog::debug("Matching requirement store found at '{}', deleting store",
                          yamlPath.string());

            std::filesystem::path storeDir = yamlPath.parent_path();

            // Guard against deleting git root or any directory containing .git
            if (m_filesystem->exists(storeDir / ".git"))
            {
                throw std::runtime_error("Cannot delete store at '" + storeDir.string() +
                                         "': directory contains .git repository. Stores must be "
                                         "in a subdirectory.");
            }

            // Additional safety: check if store directory is the git root
            if (m_filesystem->equivalent(storeDir, gitRoot))
            {
                throw std::runtime_error("Cannot delete store at git root '" + storeDir.string() +
                                         "'. Stores must be in a subdirectory.");
            }

            std::error_code errorCode;
            m_filesystem->remove_all(storeDir, errorCode);
            if (errorCode)
            {
                throw std::filesystem::filesystem_error("Error deleting store YAML file", yamlPath,
                                                        errorCode);
            }
            spdlog::info("Store '{}' deleted successfully", name);
            return;
        }
    }
    spdlog::warn("Store '{}' not found in any .felztrace.yml file, nothing to delete", name);
}

// void RequirementStoreYml::addRequirement(const std::string& storeName)
// {
//     spdlog::info("Adding requirement to store '{}'", storeName);
//     spdlog::debug("Finding git root from current directory");
//     std::filesystem::path gitRoot = m_filesystem->findGitRoot(std::filesystem::current_path());
//     spdlog::debug("Searching for .felztrace.yml file with matching storeName in settings");
//     auto yamlFiles = m_filesystem->listFilesWithExtensionAndName(gitRoot, FileExtension(".yml"),
//     FileName(".felztrace.yml")); for (const auto& yamlPath : yamlFiles)
//     {
//         std::string yamlContent = m_filesystem->readFile(yamlPath);
//         YAML::Node existingNode = YAML::Load(yamlContent);
//         if (existingNode["store"] && existingNode["store"].as<std::string>() == storeName)
//         {
//             spdlog::debug("Matching requirement store found at '{}', adding requirement",
//                           yamlPath.string());
//             generateRequirement(storeName, yamlPath.parent_path(), m_filesystem.get());
//             spdlog::info("Requirement added to store '{}'", storeName);
//             return;
//         }
//     }
//     spdlog::warn("Requirement store '{}' not found in any .felztrace.yml file", storeName);
//     throw std::runtime_error("Requirement store not found");
// }

} // namespace felztrace
