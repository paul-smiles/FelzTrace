#include "filesystem_interface.h"
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <random>
#include <sstream>
#include <yaml-cpp/yaml.h>

using namespace felztrace;

class RealFilesystemTest : public ::testing::Test
{
  protected:
    void SetUp() override
    {
        // Create unique temp directory for this test
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(10000, 99999);

        std::ostringstream oss;
        oss << "felztrace_test_" << dis(gen);

        tempDir = std::filesystem::temp_directory_path() / oss.str();
        std::filesystem::create_directories(tempDir);
    }

    void TearDown() override
    {
        // Clean up temp directory
        if (std::filesystem::exists(tempDir))
        {
            std::filesystem::remove_all(tempDir);
        }
    }

    std::filesystem::path tempDir;
};

TEST_F(RealFilesystemTest, findGitRootInPath)
{
    std::filesystem::path gitDir = tempDir / ".git";
    std::filesystem::create_directories(gitDir);

    EXPECT_EQ(RealFilesystem().findGitRoot(tempDir), tempDir);
}

TEST_F(RealFilesystemTest, findGitRootInParentPath)
{
    std::filesystem::path gitDir = tempDir / ".git";
    std::filesystem::create_directories(gitDir);

    std::filesystem::path nestedDir = tempDir / "nested" / "deep";
    std::filesystem::create_directories(nestedDir);

    EXPECT_EQ(RealFilesystem().findGitRoot(nestedDir), tempDir);
}

TEST_F(RealFilesystemTest, throwsWhenNoGitRootFound)
{
    std::filesystem::path nestedDir = tempDir / "nested" / "deep";
    std::filesystem::create_directories(nestedDir);

    EXPECT_THROW(RealFilesystem().findGitRoot(nestedDir), std::runtime_error);
}

TEST_F(RealFilesystemTest, listFilesWithExtensionAndName)
{
    std::filesystem::path file1 = tempDir / "file1.txt";
    std::filesystem::path file2 = tempDir / "file2.txt";
    std::filesystem::path file3 = tempDir / "file3.yml";
    std::filesystem::path file4 = tempDir / "file4.txt";

    std::ofstream(file1) << "test";
    std::ofstream(file2) << "test";
    std::ofstream(file3) << "test";
    std::ofstream(file4) << "test";

    auto result = RealFilesystem().listFilesWithExtensionAndName(tempDir, FileExtension(".txt"),
                                                                 FileName("file2.txt"));
    EXPECT_EQ(result.size(), 1);
    EXPECT_EQ(result[0], file2);
}

TEST_F(RealFilesystemTest, listFilesWithExtensionAndNameNestedDirectories)
{
    std::filesystem::path nestedDir = tempDir / "nested";
    std::filesystem::create_directories(nestedDir);

    std::filesystem::path file1 = tempDir / "file1.txt";
    std::filesystem::path file2 = nestedDir / "file2.txt";

    std::ofstream(file1) << "test";
    std::ofstream(file2) << "test";

    auto result = RealFilesystem().listFilesWithExtensionAndName(tempDir, FileExtension(".txt"));
    EXPECT_EQ(result.size(), 2);
    EXPECT_TRUE((result[0] == file1 && result[1] == file2) ||
                (result[0] == file2 && result[1] == file1));
}

TEST_F(RealFilesystemTest, listFilesWithExtensionAndNameNoMatches)
{
    std::filesystem::path file1 = tempDir / "file1.txt";
    std::filesystem::path file2 = tempDir / "file2.yml";

    std::ofstream(file1) << "test";
    std::ofstream(file2) << "test";

    auto result = RealFilesystem().listFilesWithExtensionAndName(tempDir, FileExtension(".md"));
    EXPECT_TRUE(result.empty());
}

TEST_F(RealFilesystemTest, listFilesWithExtensionIgnoresDirectories)
{
    // Create a directory that would match the extension if it were a file
    std::filesystem::path dir = tempDir / "somedir.txt";
    std::filesystem::create_directories(dir);

    std::filesystem::path file = tempDir / "realfile.txt";
    std::ofstream(file) << "test";

    auto result = RealFilesystem().listFilesWithExtensionAndName(tempDir, FileExtension(".txt"));
    EXPECT_EQ(result.size(), 1);
    EXPECT_EQ(result[0], file);
}

TEST_F(RealFilesystemTest, writeFile)
{
    std::filesystem::path filePath = tempDir / "testfile.txt";
    std::string content = "Hello, FelzTrace!";

    RealFilesystem realFs;
    realFs.writeFile(filePath, content);
    EXPECT_TRUE(std::filesystem::exists(filePath));
    EXPECT_EQ(realFs.readFile(filePath), content);
}

TEST_F(RealFilesystemTest, readFileNonExistentThrows)
{
    std::filesystem::path filePath = tempDir / "nonexistent.txt";

    RealFilesystem realFs;
    EXPECT_THROW(realFs.readFile(filePath), std::runtime_error);
}

TEST_F(RealFilesystemTest, writeFileToNonExistentDirectoryThrows)
{
    std::filesystem::path filePath = tempDir / "nonexistent_dir" / "testfile.txt";
    std::string content = "Hello, FelzTrace!";

    RealFilesystem realFs;
    EXPECT_THROW(realFs.writeFile(filePath, content), std::runtime_error);
}

TEST_F(RealFilesystemTest, createAndRemoveDirectories)
{
    std::filesystem::path dirPath = tempDir / "subdir" / "nested";
    std::error_code ec;

    RealFilesystem realFs;
    EXPECT_TRUE(realFs.create_directories(dirPath, ec));
    EXPECT_FALSE(ec);
    EXPECT_TRUE(std::filesystem::exists(dirPath));

    realFs.remove_all(tempDir / "subdir", ec);
    EXPECT_FALSE(ec);
    EXPECT_FALSE(std::filesystem::exists(dirPath));
}

TEST_F(RealFilesystemTest, isRegularFile)
{
    std::filesystem::path filePath = tempDir / "testfile.txt";
    std::ofstream(filePath) << "test content";

    RealFilesystem realFs;
    EXPECT_TRUE(realFs.is_regular_file(filePath));

    std::filesystem::path dirPath = tempDir / "subdir";
    std::filesystem::create_directories(dirPath);
    EXPECT_FALSE(realFs.is_regular_file(dirPath));
}