#include "requirement_store_yml.h"
#include <filesystem>
#include <fstream>
#include <gtest/gtest.h>
#include <yaml-cpp/yaml.h>

using namespace felztrace;

// Integration test fixture using RealFilesystem
class RequirementStoreYmlIntegrationTest : public ::testing::Test
{
  protected:
    void SetUp() override
    {
        // Create temp directory for tests
        testPath = std::filesystem::temp_directory_path() / "felztrace_test";
        std::filesystem::remove_all(testPath);

        // Create a git root for tests that need it
        gitRoot = std::filesystem::temp_directory_path() / "felztrace_git_root";
        std::filesystem::remove_all(gitRoot);
        std::filesystem::create_directories(gitRoot / ".git");
    }

    void TearDown() override
    {
        // Clean up test directories
        std::filesystem::remove_all(testPath);
        std::filesystem::remove_all(gitRoot);
    }

    std::filesystem::path testPath;
    std::filesystem::path gitRoot;
    static constexpr const char* CONFIG_FILENAME = ".felztrace.yml";
};

TEST_F(RequirementStoreYmlIntegrationTest, CreateRequirementStoreCreatesDirectory)
{
    RequirementStoreYml store;

    std::string storePath = (gitRoot / "test_store").string();
    store.createRequirementStore("test_store", storePath, 1);

    EXPECT_TRUE(std::filesystem::exists(storePath));
    EXPECT_TRUE(std::filesystem::is_directory(storePath));
}

TEST_F(RequirementStoreYmlIntegrationTest, CreateRequirementStoreCreatesYamlFile)
{
    RequirementStoreYml store;

    std::string storePath = (gitRoot / "test_store").string();
    store.createRequirementStore("test_store", storePath, 1);

    std::filesystem::path yamlFile = std::filesystem::path(storePath) / CONFIG_FILENAME;
    EXPECT_TRUE(std::filesystem::exists(yamlFile));
    EXPECT_TRUE(std::filesystem::is_regular_file(yamlFile));
}

TEST_F(RequirementStoreYmlIntegrationTest, CreateRequirementStoreWritesCorrectSettings)
{
    RequirementStoreYml store;

    std::string storePath = (gitRoot / "my_store").string();
    store.createRequirementStore("my_store", storePath, 2);

    std::filesystem::path yamlFile = std::filesystem::path(storePath) / CONFIG_FILENAME;
    YAML::Node root = YAML::LoadFile(yamlFile.string());

    EXPECT_EQ(root["store"].as<std::string>(), "my_store");
    EXPECT_EQ(root["settings"]["level"].as<int>(), 2);
}

TEST_F(RequirementStoreYmlIntegrationTest, CreateTestStoreWritesCorrectSettings)
{
    RequirementStoreYml store;

    std::string storePath = (gitRoot / "my_test_store").string();
    store.createTestStore("my_test_store", storePath, 3);

    std::filesystem::path yamlFile = std::filesystem::path(storePath) / CONFIG_FILENAME;
    YAML::Node root = YAML::LoadFile(yamlFile.string());

    EXPECT_EQ(root["store"].as<std::string>(), "my_test_store");
    EXPECT_EQ(root["settings"]["type"].as<std::string>(), "tests");
    EXPECT_EQ(root["settings"]["level"].as<int>(), 3);
}

TEST_F(RequirementStoreYmlIntegrationTest, CreateRequirementStoreCreatesNestedDirectories)
{
    RequirementStoreYml store;

    std::filesystem::path nestedPath = gitRoot / "level1" / "level2" / "level3";
    store.createRequirementStore("nested_store", nestedPath.string(), 1);

    EXPECT_TRUE(std::filesystem::exists(nestedPath));
    EXPECT_TRUE(std::filesystem::is_directory(nestedPath));

    std::filesystem::path yamlFile = nestedPath / CONFIG_FILENAME;
    EXPECT_TRUE(std::filesystem::exists(yamlFile));
}

TEST_F(RequirementStoreYmlIntegrationTest, CreateRequirementStoreThrowsOnDuplicateStoreName)
{
    RequirementStoreYml store;

    // Create first store
    std::string storePath1 = (gitRoot / "store1").string();
    store.createRequirementStore("duplicate_name", storePath1, 1);

    // Attempt to create second store with same name
    std::string storePath2 = (gitRoot / "store2").string();
    EXPECT_THROW(store.createRequirementStore("duplicate_name", storePath2, 1), std::runtime_error);
}

TEST_F(RequirementStoreYmlIntegrationTest, CreateRequirementStoreAllowsDifferentNames)
{
    RequirementStoreYml store;

    // Create multiple stores with different names
    std::string storePath1 = (gitRoot / "store1").string();
    std::string storePath2 = (gitRoot / "store2").string();

    EXPECT_NO_THROW(store.createRequirementStore("name1", storePath1, 1));
    EXPECT_NO_THROW(store.createRequirementStore("name2", storePath2, 1));

    EXPECT_TRUE(std::filesystem::exists(std::filesystem::path(storePath1) / CONFIG_FILENAME));
    EXPECT_TRUE(std::filesystem::exists(std::filesystem::path(storePath2) / CONFIG_FILENAME));
}

TEST_F(RequirementStoreYmlIntegrationTest, CreateRequirementStoreSearchesFromGitRoot)
{
    RequirementStoreYml store;

    // Create store in nested directory under git root
    std::filesystem::path nestedPath = gitRoot / "subdir1" / "subdir2" / "store";
    store.createRequirementStore("unique_name", nestedPath.string(), 1);

    // Try to create another store with same name in different nested path
    std::filesystem::path anotherPath = gitRoot / "other" / "path" / "store";
    EXPECT_THROW(store.createRequirementStore("unique_name", anotherPath.string(), 1),
                 std::runtime_error);
}

TEST_F(RequirementStoreYmlIntegrationTest, CreateRequirementStoreThrowsWhenYamlFileAlreadyExists)
{
    RequirementStoreYml store;

    std::string storePath = (gitRoot / "existing_store").string();

    // Create a store first
    store.createRequirementStore("first_name", storePath, 1);

    // Try to create another store at the same path (different name)
    EXPECT_THROW(store.createRequirementStore("second_name", storePath, 1),
                 std::filesystem::filesystem_error);
}

TEST_F(RequirementStoreYmlIntegrationTest, CreateRequirementStoreThrowsAtGitRoot)
{
    RequirementStoreYml store;

    // Attempt to create store at git root
    EXPECT_THROW(store.createRequirementStore("rootstore", gitRoot.string(), 1),
                 std::runtime_error);
}

TEST_F(RequirementStoreYmlIntegrationTest, CreateRequirementStoreThrowsInDirectoryWithGit)
{
    RequirementStoreYml store;

    // Create a directory with .git
    std::filesystem::path storePath = gitRoot / "subdir_with_git";
    std::filesystem::create_directories(storePath / ".git");

    // Attempt to create store in directory containing .git
    EXPECT_THROW(store.createRequirementStore("badstore", storePath.string(), 1),
                 std::runtime_error);
}

TEST_F(RequirementStoreYmlIntegrationTest, DeleteRequirementStoreRemovesDirectory)
{
    RequirementStoreYml store;

    std::string storePath = (gitRoot / "delete_test_store").string();
    store.createRequirementStore("delete_test", storePath, 1);

    EXPECT_TRUE(std::filesystem::exists(storePath));

    // Change to git root for delete operation
    std::filesystem::current_path(gitRoot);
    store.deleteStore("delete_test");

    EXPECT_FALSE(std::filesystem::exists(storePath));
}

TEST_F(RequirementStoreYmlIntegrationTest, DeleteRequirementStoreRemovesYamlFile)
{
    RequirementStoreYml store;

    std::string storePath = (gitRoot / "delete_yaml_test").string();
    store.createRequirementStore("delete_yaml", storePath, 1);

    std::filesystem::path yamlFile = std::filesystem::path(storePath) / CONFIG_FILENAME;
    EXPECT_TRUE(std::filesystem::exists(yamlFile));

    std::filesystem::current_path(gitRoot);
    store.deleteStore("delete_yaml");

    EXPECT_FALSE(std::filesystem::exists(yamlFile));
}

TEST_F(RequirementStoreYmlIntegrationTest, DeleteRequirementStoreRemovesAllContents)
{
    RequirementStoreYml store;

    std::string storePath = (gitRoot / "delete_all_test").string();
    store.createRequirementStore("delete_all", storePath, 1);

    // Add some additional files and directories to the store
    std::filesystem::create_directories(std::filesystem::path(storePath) / "subdir");
    std::ofstream(std::filesystem::path(storePath) / "extra_file.txt") << "content";

    EXPECT_TRUE(std::filesystem::exists(std::filesystem::path(storePath) / "subdir"));
    EXPECT_TRUE(std::filesystem::exists(std::filesystem::path(storePath) / "extra_file.txt"));

    std::filesystem::current_path(gitRoot);
    store.deleteStore("delete_all");

    EXPECT_FALSE(std::filesystem::exists(storePath));
}

TEST_F(RequirementStoreYmlIntegrationTest, DeleteRequirementStoreThrowsAtGitRoot)
{
    RequirementStoreYml store;

    // Create store at git root (bypass protection for this test setup)
    std::filesystem::path yamlFile = gitRoot / CONFIG_FILENAME;
    YAML::Node root;
    root["store"] = "rootstore";
    root["settings"]["level"] = 1;
    std::ofstream fout(yamlFile);
    fout << root;
    fout.close();

    std::filesystem::current_path(gitRoot);

    // Attempt to delete - should throw
    EXPECT_THROW(store.deleteStore("rootstore"), std::runtime_error);
}

TEST_F(RequirementStoreYmlIntegrationTest, DeleteRequirementStoreThrowsInDirectoryWithGit)
{
    RequirementStoreYml store;

    // Create store path
    std::filesystem::path storePath = gitRoot / "subdir_with_git";
    std::filesystem::create_directories(storePath);

    // Create .felztrace.yml and .git
    std::filesystem::path yamlFile = storePath / CONFIG_FILENAME;
    YAML::Node root;
    root["store"] = "badstore";
    root["settings"]["level"] = 1;
    std::ofstream fout(yamlFile);
    fout << root;
    fout.close();

    std::filesystem::create_directories(storePath / ".git");

    std::filesystem::current_path(gitRoot);

    // Attempt to delete - should throw
    EXPECT_THROW(store.deleteStore("badstore"), std::runtime_error);
}

TEST_F(RequirementStoreYmlIntegrationTest, DeleteNonexistentStoreDoesNotThrow)
{
    RequirementStoreYml store;

    std::filesystem::current_path(gitRoot);

    // Attempting to delete non-existent store should not throw
    EXPECT_NO_THROW(store.deleteStore("nonexistent"));
}

TEST_F(RequirementStoreYmlIntegrationTest, DeleteStoreRemovesTestStoreDirectory)
{
    RequirementStoreYml store;

    std::string storePath = (gitRoot / "delete_test_store_kind").string();
    store.createTestStore("delete_test_store_kind", storePath, 2);

    EXPECT_TRUE(std::filesystem::exists(storePath));

    std::filesystem::current_path(gitRoot);
    store.deleteStore("delete_test_store_kind");

    EXPECT_FALSE(std::filesystem::exists(storePath));
}
