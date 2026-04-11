#include "filesystem_interface.h"
#include "requirement_store_yml.h"
#include <filesystem>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <yaml-cpp/yaml.h>

using namespace felztrace;
using ::testing::_;
using ::testing::DoAll;
using ::testing::Return;
using ::testing::SaveArg;
using ::testing::SetArgReferee;

// Mock filesystem for unit testing
class MockFilesystem : public IFilesystem
{
  public:
    MOCK_METHOD(bool, exists, (const std::filesystem::path&), (const, override));
    MOCK_METHOD(std::filesystem::path, weakly_canonical, (const std::filesystem::path&),
                (const, override));
    MOCK_METHOD(bool, create_directories, (const std::filesystem::path&, std::error_code&),
                (const, override));
    MOCK_METHOD(bool, is_regular_file, (const std::filesystem::path&), (const, override));
    MOCK_METHOD(void, remove_all, (const std::filesystem::path&, std::error_code&),
                (const, override));
    MOCK_METHOD(std::vector<std::filesystem::path>, listFilesWithExtensionAndName,
                (const std::filesystem::path&, const FileExtension&, const FileName&),
                (const, override));
    MOCK_METHOD(void, writeFile, (const std::filesystem::path&, const std::string&),
                (const, override));
    MOCK_METHOD(std::string, readFile, (const std::filesystem::path&), (const, override));
    MOCK_METHOD(std::filesystem::path, findGitRoot, (const std::filesystem::path&),
                (const, override));
};

// Test fixture for RequirementStore unit tests
class RequirementStoreTest : public ::testing::Test
{
  protected:
    void SetUp() override
    {
        mockFs = std::make_unique<MockFilesystem>();
        mockFsPtr = mockFs.get();
    }

    std::unique_ptr<MockFilesystem> mockFs;
    MockFilesystem* mockFsPtr;

    static constexpr const char* CONFIG_FILENAME = ".felztrace.yml";
};

// Unit Test: Verify create_directories is called with correct path
TEST_F(RequirementStoreTest, CallsCreateDirectories)
{
    std::filesystem::path storePath = "/project/requirements";
    std::filesystem::path gitRoot = "/project";

    EXPECT_CALL(*mockFsPtr, findGitRoot(storePath)).WillOnce(Return(gitRoot));
    EXPECT_CALL(*mockFsPtr, listFilesWithExtensionAndName(gitRoot, FileExtension(".yml"),
                                                          FileName(".felztrace.yml")))
        .WillOnce(Return(std::vector<std::filesystem::path>{}));

    EXPECT_CALL(*mockFsPtr, weakly_canonical(storePath / CONFIG_FILENAME))
        .WillOnce(Return(storePath / CONFIG_FILENAME));
    EXPECT_CALL(*mockFsPtr, exists(storePath / CONFIG_FILENAME)).WillOnce(Return(false));
    EXPECT_CALL(*mockFsPtr, create_directories(storePath, _))
        .WillOnce(DoAll(SetArgReferee<1>(std::error_code()), Return(true)));

    EXPECT_CALL(*mockFsPtr, writeFile(storePath / CONFIG_FILENAME, _));

    RequirementStoreYml store(std::move(mockFs));
    store.createRequirementStore("mystore", storePath.string(), 0);
}

// Unit Test: Throws when create_directories fails
TEST_F(RequirementStoreTest, ThrowsWhenCreateDirectoriesFails)
{
    std::filesystem::path storePath = "/project/requirements";
    std::filesystem::path gitRoot = "/project";

    EXPECT_CALL(*mockFsPtr, findGitRoot(storePath)).WillOnce(Return(gitRoot));
    EXPECT_CALL(*mockFsPtr, listFilesWithExtensionAndName(_, _, _))
        .WillOnce(Return(std::vector<std::filesystem::path>{}));

    EXPECT_CALL(*mockFsPtr, weakly_canonical(storePath / CONFIG_FILENAME))
        .WillOnce(Return(storePath / CONFIG_FILENAME));
    EXPECT_CALL(*mockFsPtr, exists(storePath / CONFIG_FILENAME)).WillOnce(Return(false));
    EXPECT_CALL(*mockFsPtr, create_directories(storePath, _))
        .WillOnce(DoAll(SetArgReferee<1>(std::make_error_code(std::errc::permission_denied)),
                        Return(false)));

    RequirementStoreYml store(std::move(mockFs));
    EXPECT_THROW(store.createRequirementStore("mystore", storePath.string(), 3),
                 std::filesystem::filesystem_error);
}

// Unit Test: Writes YAML file with correct storeName and level
TEST_F(RequirementStoreTest, WritesYamlFileWithCorrectSettings)
{
    std::filesystem::path storePath = "/project/requirements";
    std::filesystem::path gitRoot = "/project";
    std::string yamlContent;

    EXPECT_CALL(*mockFsPtr, findGitRoot(storePath)).WillOnce(Return(gitRoot));
    EXPECT_CALL(*mockFsPtr, listFilesWithExtensionAndName(_, _, _))
        .WillOnce(Return(std::vector<std::filesystem::path>{}));

    EXPECT_CALL(*mockFsPtr, weakly_canonical(storePath / CONFIG_FILENAME))
        .WillOnce(Return(storePath / CONFIG_FILENAME));
    EXPECT_CALL(*mockFsPtr, exists(storePath / CONFIG_FILENAME)).WillOnce(Return(false));
    EXPECT_CALL(*mockFsPtr, create_directories(_, _))
        .WillOnce(DoAll(SetArgReferee<1>(std::error_code()), Return(true)));

    EXPECT_CALL(*mockFsPtr, writeFile(storePath / CONFIG_FILENAME, _))
        .WillOnce(SaveArg<1>(&yamlContent));

    RequirementStoreYml store(std::move(mockFs));
    store.createRequirementStore("testName", storePath.string(), 2);

    YAML::Node root = YAML::Load(yamlContent);
    EXPECT_EQ(root["store"].as<std::string>(), "testName");
    EXPECT_EQ(root["settings"]["type"].as<std::string>(), "requirements");
    EXPECT_EQ(root["settings"]["level"].as<int>(), 2);
}

// Unit Test: Throws when YAML file already exists at path
TEST_F(RequirementStoreTest, ThrowsWhenYamlFileAlreadyExists)
{
    std::filesystem::path storePath = "/project/requirements";
    std::filesystem::path gitRoot = "/project";

    EXPECT_CALL(*mockFsPtr, findGitRoot(storePath)).WillOnce(Return(gitRoot));
    EXPECT_CALL(*mockFsPtr, listFilesWithExtensionAndName(_, _, _))
        .WillOnce(Return(std::vector<std::filesystem::path>{}));

    // YAML file already exists - should throw before create_directories
    EXPECT_CALL(*mockFsPtr, weakly_canonical(storePath / CONFIG_FILENAME))
        .WillOnce(Return(storePath / CONFIG_FILENAME));
    EXPECT_CALL(*mockFsPtr, exists(storePath / CONFIG_FILENAME)).WillOnce(Return(true));
    EXPECT_CALL(*mockFsPtr, create_directories(_, _)).Times(0); // Should not be called
    EXPECT_CALL(*mockFsPtr, writeFile(_, _)).Times(0);          // Should not write

    RequirementStoreYml store(std::move(mockFs));
    EXPECT_THROW(store.createRequirementStore("mystore", storePath.string(), 0),
                 std::filesystem::filesystem_error);
}

// Unit Test: Searches for git root by traversing up directory tree
TEST_F(RequirementStoreTest, SearchesForGitRootUpwards)
{
    std::filesystem::path storePath = "/project/nested/deep/requirements";
    std::filesystem::path gitRoot = "/project";

    EXPECT_CALL(*mockFsPtr, findGitRoot(storePath)).WillOnce(Return(gitRoot));
    EXPECT_CALL(*mockFsPtr, listFilesWithExtensionAndName(gitRoot, FileExtension(".yml"),
                                                          FileName(".felztrace.yml")))
        .WillOnce(Return(std::vector<std::filesystem::path>{}));

    EXPECT_CALL(*mockFsPtr, weakly_canonical(storePath / CONFIG_FILENAME))
        .WillOnce(Return(storePath / CONFIG_FILENAME));
    EXPECT_CALL(*mockFsPtr, exists(storePath / CONFIG_FILENAME)).WillOnce(Return(false));
    EXPECT_CALL(*mockFsPtr, create_directories(_, _))
        .WillOnce(DoAll(SetArgReferee<1>(std::error_code()), Return(true)));
    EXPECT_CALL(*mockFsPtr, writeFile(_, _));

    RequirementStoreYml store(std::move(mockFs));
    store.createRequirementStore("mystore", storePath.string(), 4);
}

// Unit Test: Throws when no git root is found
TEST_F(RequirementStoreTest, ThrowsWhenNoGitRootFound)
{
    std::filesystem::path storePath = "/project/requirements";

    EXPECT_CALL(*mockFsPtr, findGitRoot(storePath))
        .WillOnce(::testing::Throw(
            std::runtime_error("No git root found starting from: " + storePath.string())));

    RequirementStoreYml store(std::move(mockFs));
    EXPECT_THROW(store.createRequirementStore("mystore", storePath.string(), 1),
                 std::runtime_error);
}

// Unit Test: Calls listFilesWithExtensionAndName with git root to find duplicates
TEST_F(RequirementStoreTest, SearchesForDuplicatePrefixesInGitRoot)
{
    std::filesystem::path storePath = "/project/requirements";
    std::filesystem::path gitRoot = "/project";
    std::filesystem::path capturedSearchPath;

    EXPECT_CALL(*mockFsPtr, findGitRoot(storePath)).WillOnce(Return(gitRoot));
    EXPECT_CALL(*mockFsPtr, listFilesWithExtensionAndName(gitRoot, FileExtension(".yml"),
                                                          FileName(".felztrace.yml")))
        .WillOnce(
            DoAll(SaveArg<0>(&capturedSearchPath), Return(std::vector<std::filesystem::path>{})));

    EXPECT_CALL(*mockFsPtr, weakly_canonical(storePath / CONFIG_FILENAME))
        .WillOnce(Return(storePath / CONFIG_FILENAME));
    EXPECT_CALL(*mockFsPtr, exists(storePath / CONFIG_FILENAME)).WillOnce(Return(false));
    EXPECT_CALL(*mockFsPtr, create_directories(_, _))
        .WillOnce(DoAll(SetArgReferee<1>(std::error_code()), Return(true)));
    EXPECT_CALL(*mockFsPtr, writeFile(_, _));

    RequirementStoreYml store(std::move(mockFs));
    store.createRequirementStore("mystore", storePath.string(), 7);

    EXPECT_EQ(capturedSearchPath, gitRoot);
}

// Unit Test: Throws when duplicate storeName found in git repository
TEST_F(RequirementStoreTest, ThrowsWhenDuplicatePrefixFoundInRepo)
{
    std::filesystem::path storePath = "/project/requirements";
    std::filesystem::path gitRoot = "/project";
    std::filesystem::path existingStore = "/project/other/.felztrace.yml";

    EXPECT_CALL(*mockFsPtr, findGitRoot(storePath)).WillOnce(Return(gitRoot));

    // Return existing store with same storeName
    EXPECT_CALL(*mockFsPtr, listFilesWithExtensionAndName(_, _, _))
        .WillOnce(Return(std::vector<std::filesystem::path>{existingStore}));

    // Mock reading the existing YAML file
    std::string existingYaml = "store: mystore\n";
    EXPECT_CALL(*mockFsPtr, readFile(existingStore)).WillOnce(Return(existingYaml));

    RequirementStoreYml store(std::move(mockFs));
    EXPECT_THROW(store.createRequirementStore("mystore", storePath.string(), 0),
                 std::runtime_error);
}

// Unit Test: Allows creation when different storeName found
TEST_F(RequirementStoreTest, AllowsCreationWhenDifferentPrefixFound)
{
    std::filesystem::path storePath = "/project/requirements";
    std::filesystem::path gitRoot = "/project";
    std::filesystem::path existingStore = "/project/other/.felztrace.yml";

    EXPECT_CALL(*mockFsPtr, findGitRoot(storePath)).WillOnce(Return(gitRoot));

    EXPECT_CALL(*mockFsPtr, listFilesWithExtensionAndName(_, _, _))
        .WillOnce(Return(std::vector<std::filesystem::path>{existingStore}));

    // Different storeName
    std::string existingYaml = "store: differentstore\n";
    EXPECT_CALL(*mockFsPtr, readFile(existingStore)).WillOnce(Return(existingYaml));

    EXPECT_CALL(*mockFsPtr, weakly_canonical(storePath / CONFIG_FILENAME))
        .WillOnce(Return(storePath / CONFIG_FILENAME));
    EXPECT_CALL(*mockFsPtr, exists(storePath / CONFIG_FILENAME)).WillOnce(Return(false));
    EXPECT_CALL(*mockFsPtr, create_directories(_, _))
        .WillOnce(DoAll(SetArgReferee<1>(std::error_code()), Return(true)));
    EXPECT_CALL(*mockFsPtr, writeFile(_, _));

    RequirementStoreYml store(std::move(mockFs));
    EXPECT_NO_THROW(store.createRequirementStore("mystore", storePath.string(), 2));
}

// Unit Test: Deletes requirement store when matching store exists
TEST_F(RequirementStoreTest, DeletesStoreWhenExists)
{
    std::filesystem::path gitRoot = "/project";
    std::filesystem::path yamlFilePath = "/project/requirements/.felztrace.yml";

    EXPECT_CALL(*mockFsPtr, findGitRoot(std::filesystem::current_path())).WillOnce(Return(gitRoot));
    EXPECT_CALL(*mockFsPtr, listFilesWithExtensionAndName(gitRoot, FileExtension(".yml"),
                                                          FileName(".felztrace.yml")))
        .WillOnce(Return(std::vector<std::filesystem::path>{yamlFilePath}));

    // Mock reading and validating the YAML file
    std::string yamlContent = "store: mystore\n";
    EXPECT_CALL(*mockFsPtr, readFile(yamlFilePath)).WillOnce(Return(yamlContent));

    EXPECT_CALL(*mockFsPtr, remove_all(yamlFilePath.parent_path(), _))
        .WillOnce(DoAll(SetArgReferee<1>(std::error_code()), Return()));

    RequirementStoreYml store(std::move(mockFs));
    EXPECT_NO_THROW(store.deleteRequirementStore("mystore"));
}

// Unit Test: Does nothing when no matching store exists
TEST_F(RequirementStoreTest, DoesNothingWhenStoreDoesNotExist)
{
    std::filesystem::path gitRoot = "/project";

    EXPECT_CALL(*mockFsPtr, findGitRoot(std::filesystem::current_path())).WillOnce(Return(gitRoot));
    EXPECT_CALL(*mockFsPtr, listFilesWithExtensionAndName(gitRoot, FileExtension(".yml"),
                                                          FileName(".felztrace.yml")))
        .WillOnce(Return(std::vector<std::filesystem::path>{}));
    EXPECT_CALL(*mockFsPtr, remove_all(_, _)).Times(0);

    RequirementStoreYml store(std::move(mockFs));
    EXPECT_NO_THROW(store.deleteRequirementStore("mystore"));
}

// Unit Test: Does nothing when discovered stores do not match requested name
TEST_F(RequirementStoreTest, DoesNothingWhenStoreNameDoesNotMatch)
{
    std::filesystem::path gitRoot = "/project";
    std::filesystem::path yamlFilePath = "/project/requirements/.felztrace.yml";

    EXPECT_CALL(*mockFsPtr, findGitRoot(std::filesystem::current_path())).WillOnce(Return(gitRoot));
    EXPECT_CALL(*mockFsPtr, listFilesWithExtensionAndName(gitRoot, FileExtension(".yml"),
                                                          FileName(".felztrace.yml")))
        .WillOnce(Return(std::vector<std::filesystem::path>{yamlFilePath}));
    std::string yamlContent = "store: differentstore\n";
    EXPECT_CALL(*mockFsPtr, readFile(yamlFilePath)).WillOnce(Return(yamlContent));
    EXPECT_CALL(*mockFsPtr, remove_all(_, _)).Times(0);

    RequirementStoreYml store(std::move(mockFs));
    EXPECT_NO_THROW(store.deleteRequirementStore("mystore"));
}

// Unit Test: Checks multiple discovered stores and deletes the matching one
TEST_F(RequirementStoreTest, DeletesMatchingStoreAmongMultipleCandidates)
{
    std::filesystem::path gitRoot = "/project";
    std::filesystem::path otherYamlFilePath = "/project/other/.felztrace.yml";
    std::filesystem::path yamlFilePath = "/project/requirements/.felztrace.yml";

    EXPECT_CALL(*mockFsPtr, findGitRoot(std::filesystem::current_path())).WillOnce(Return(gitRoot));
    EXPECT_CALL(*mockFsPtr, listFilesWithExtensionAndName(gitRoot, FileExtension(".yml"),
                                                          FileName(".felztrace.yml")))
        .WillOnce(Return(std::vector<std::filesystem::path>{otherYamlFilePath, yamlFilePath}));
    EXPECT_CALL(*mockFsPtr, readFile(otherYamlFilePath)).WillOnce(Return("store: other\n"));
    EXPECT_CALL(*mockFsPtr, readFile(yamlFilePath)).WillOnce(Return("store: mystore\n"));
    EXPECT_CALL(*mockFsPtr, remove_all(yamlFilePath.parent_path(), _))
        .WillOnce(DoAll(SetArgReferee<1>(std::error_code()), Return()));

    RequirementStoreYml store(std::move(mockFs));
    EXPECT_NO_THROW(store.deleteRequirementStore("mystore"));
}

// Unit Test: Throws when remove_all fails
TEST_F(RequirementStoreTest, ThrowsWhenRemoveAllFails)
{
    std::filesystem::path gitRoot = "/project";
    std::filesystem::path yamlFilePath = "/project/requirements/.felztrace.yml";

    EXPECT_CALL(*mockFsPtr, findGitRoot(std::filesystem::current_path())).WillOnce(Return(gitRoot));
    EXPECT_CALL(*mockFsPtr, listFilesWithExtensionAndName(gitRoot, FileExtension(".yml"),
                                                          FileName(".felztrace.yml")))
        .WillOnce(Return(std::vector<std::filesystem::path>{yamlFilePath}));

    // Mock reading and validating the YAML file
    std::string yamlContent = "store: mystore\n";
    EXPECT_CALL(*mockFsPtr, readFile(yamlFilePath)).WillOnce(Return(yamlContent));

    EXPECT_CALL(*mockFsPtr, remove_all(yamlFilePath.parent_path(), _))
        .WillOnce(
            DoAll(SetArgReferee<1>(std::make_error_code(std::errc::permission_denied)), Return()));

    RequirementStoreYml store(std::move(mockFs));
    EXPECT_THROW(store.deleteRequirementStore("mystore"), std::filesystem::filesystem_error);
}