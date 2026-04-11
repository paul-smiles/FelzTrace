#include "cli.h"
#include "requirement_store.h"
#include <gtest/gtest.h>
#include <spdlog/spdlog.h>
#include <sstream>

class CLITest : public ::testing::Test
{
  protected:
    class DummyRequirementStore : public felztrace::RequirementStore
    {
      public:
        bool createCalled = false;
        bool deleteCalled = false;
        std::string lastName;
        std::string lastPath;
        int lastLevel = -1;
        std::string lastDeleteName;

        void createRequirementStore(const std::string& name, const std::string& path,
                                    int level) override
        {
            createCalled = true;
            lastName = name;
            lastPath = path;
            lastLevel = level;
        }
        void deleteRequirementStore(const std::string& name) override
        {
            deleteCalled = true;
            lastDeleteName = name;
        }
        // void addRequirement(const std::string&) override {}
    };

    DummyRequirementStore store;

    void SetUp() override
    {
        // Reset log level before each test
        spdlog::set_level(spdlog::level::info);
    }
};

TEST_F(CLITest, NoArguments)
{
    const char* argv[] = {"FelzTrace"};
    felztrace::ReturnCode result = felztrace::parseCli(1, argv, store);

    EXPECT_EQ(result, felztrace::ReturnCode::Success);
    EXPECT_EQ(spdlog::get_level(), spdlog::level::info);
}

TEST_F(CLITest, HelpShortOption)
{
    const char* argv[] = {"FelzTrace", "-h"};
    felztrace::ReturnCode result = felztrace::parseCli(2, argv, store);

    EXPECT_EQ(result, felztrace::ReturnCode::Success);
}

TEST_F(CLITest, HelpLongOption)
{
    const char* argv[] = {"FelzTrace", "--help"};
    felztrace::ReturnCode result = felztrace::parseCli(2, argv, store);

    EXPECT_EQ(result, felztrace::ReturnCode::Success);
}

TEST_F(CLITest, VersionOption)
{
    const char* argv[] = {"FelzTrace", "--version"};
    felztrace::ReturnCode result = felztrace::parseCli(2, argv, store);

    EXPECT_EQ(result, felztrace::ReturnCode::Success);
}

TEST_F(CLITest, VerboseOption)
{
    const char* argv[] = {"FelzTrace", "-v"};
    felztrace::ReturnCode result = felztrace::parseCli(2, argv, store);

    EXPECT_EQ(result, felztrace::ReturnCode::Success);
    EXPECT_EQ(spdlog::get_level(), spdlog::level::debug);
}

TEST_F(CLITest, QuietOption)
{
    const char* argv[] = {"FelzTrace", "-q"};
    felztrace::ReturnCode result = felztrace::parseCli(2, argv, store);

    EXPECT_EQ(result, felztrace::ReturnCode::Success);
    EXPECT_EQ(spdlog::get_level(), spdlog::level::err);
}

TEST_F(CLITest, UnknownOption)
{
    const char* argv[] = {"FelzTrace", "--unknown"};
    felztrace::ReturnCode result = felztrace::parseCli(2, argv, store);

    EXPECT_EQ(result, felztrace::ReturnCode::Error);
}

TEST_F(CLITest, UnknownShortOption)
{
    const char* argv[] = {"FelzTrace", "-x"};
    felztrace::ReturnCode result = felztrace::parseCli(2, argv, store);

    EXPECT_EQ(result, felztrace::ReturnCode::Error);
}

TEST_F(CLITest, MultipleOptions)
{
    const char* argv[] = {"FelzTrace", "-v", "-q"};
    felztrace::ReturnCode result = felztrace::parseCli(3, argv, store);

    EXPECT_EQ(result, felztrace::ReturnCode::Success);
    // Last option should win
    EXPECT_EQ(spdlog::get_level(), spdlog::level::err);
}

TEST_F(CLITest, QuietLongOption)
{
    const char* argv[] = {"FelzTrace", "--quiet"};
    felztrace::ReturnCode result = felztrace::parseCli(2, argv, store);

    EXPECT_EQ(result, felztrace::ReturnCode::Success);
    EXPECT_EQ(spdlog::get_level(), spdlog::level::err);
}

TEST_F(CLITest, CreateHelpLongOption)
{
    const char* argv[] = {"FelzTrace", "create", "--help"};
    felztrace::ReturnCode result = felztrace::parseCli(3, argv, store);

    EXPECT_EQ(result, felztrace::ReturnCode::Success);
}

TEST_F(CLITest, CreateHelpShortOption)
{
    const char* argv[] = {"FelzTrace", "create", "-h"};
    felztrace::ReturnCode result = felztrace::parseCli(3, argv, store);

    EXPECT_EQ(result, felztrace::ReturnCode::Success);
}

TEST_F(CLITest, CreateMissingNameAndPath)
{
    const char* argv[] = {"FelzTrace", "create"};
    felztrace::ReturnCode result = felztrace::parseCli(2, argv, store);

    EXPECT_EQ(result, felztrace::ReturnCode::Error);
    EXPECT_FALSE(store.createCalled);
}

TEST_F(CLITest, CreateMissingPath)
{
    const char* argv[] = {"FelzTrace", "create", "storeName"};
    felztrace::ReturnCode result = felztrace::parseCli(3, argv, store);

    EXPECT_EQ(result, felztrace::ReturnCode::Error);
    EXPECT_FALSE(store.createCalled);
}

TEST_F(CLITest, CreateMissingLevel)
{
    const char* argv[] = {"FelzTrace", "create", "storeName", "./store-path"};
    felztrace::ReturnCode result = felztrace::parseCli(4, argv, store);

    EXPECT_EQ(result, felztrace::ReturnCode::Error);
    EXPECT_FALSE(store.createCalled);
}

TEST_F(CLITest, CreateInvalidLevel)
{
    const char* argv[] = {"FelzTrace", "create", "storeName", "./store-path", "not-a-number"};
    felztrace::ReturnCode result = felztrace::parseCli(5, argv, store);

    EXPECT_EQ(result, felztrace::ReturnCode::Error);
    EXPECT_FALSE(store.createCalled);
}

TEST_F(CLITest, CreateSuccess)
{
    const char* argv[] = {"FelzTrace", "create", "storeName", "./store-path", "3"};
    felztrace::ReturnCode result = felztrace::parseCli(5, argv, store);

    EXPECT_EQ(result, felztrace::ReturnCode::Success);
    EXPECT_TRUE(store.createCalled);
    EXPECT_EQ(store.lastName, "storeName");
    EXPECT_EQ(store.lastPath, "./store-path");
    EXPECT_EQ(store.lastLevel, 3);
}

TEST_F(CLITest, CreateSuccessWithNegativeLevel)
{
    const char* argv[] = {"FelzTrace", "create", "storeName", "./store-path", "-2"};
    felztrace::ReturnCode result = felztrace::parseCli(5, argv, store);

    EXPECT_EQ(result, felztrace::ReturnCode::Success);
    EXPECT_TRUE(store.createCalled);
    EXPECT_EQ(store.lastName, "storeName");
    EXPECT_EQ(store.lastPath, "./store-path");
    EXPECT_EQ(store.lastLevel, -2);
}

TEST_F(CLITest, CreateThenVerbose)
{
    const char* argv[] = {"FelzTrace", "create", "storeName", "./store-path", "1", "-v"};
    felztrace::ReturnCode result = felztrace::parseCli(6, argv, store);

    EXPECT_EQ(result, felztrace::ReturnCode::Success);
    EXPECT_TRUE(store.createCalled);
    EXPECT_EQ(store.lastLevel, 1);
    EXPECT_EQ(spdlog::get_level(), spdlog::level::debug);
}

TEST_F(CLITest, DeleteHelpLongOption)
{
    std::istringstream in("");
    const char* argv[] = {"FelzTrace", "delete", "--help"};
    felztrace::ReturnCode result = felztrace::parseCli(3, argv, store, in);

    EXPECT_EQ(result, felztrace::ReturnCode::Success);
    EXPECT_FALSE(store.deleteCalled);
}

TEST_F(CLITest, DeleteHelpShortOption)
{
    std::istringstream in("");
    const char* argv[] = {"FelzTrace", "delete", "-h"};
    felztrace::ReturnCode result = felztrace::parseCli(3, argv, store, in);

    EXPECT_EQ(result, felztrace::ReturnCode::Success);
    EXPECT_FALSE(store.deleteCalled);
}

TEST_F(CLITest, DeleteMissingName)
{
    std::istringstream in("");
    const char* argv[] = {"FelzTrace", "delete"};
    felztrace::ReturnCode result = felztrace::parseCli(2, argv, store, in);

    EXPECT_EQ(result, felztrace::ReturnCode::Error);
    EXPECT_FALSE(store.deleteCalled);
}

TEST_F(CLITest, DeleteCancelledWithN)
{
    std::istringstream in("n");
    const char* argv[] = {"FelzTrace", "delete", "mystore"};
    felztrace::ReturnCode result = felztrace::parseCli(3, argv, store, in);

    EXPECT_EQ(result, felztrace::ReturnCode::Success);
    EXPECT_FALSE(store.deleteCalled);
}

TEST_F(CLITest, DeleteCancelledWithEnter)
{
    std::istringstream in("");
    const char* argv[] = {"FelzTrace", "delete", "mystore"};
    felztrace::ReturnCode result = felztrace::parseCli(3, argv, store, in);

    EXPECT_EQ(result, felztrace::ReturnCode::Success);
    EXPECT_FALSE(store.deleteCalled);
}

TEST_F(CLITest, DeleteSuccess)
{
    std::istringstream in("y");
    const char* argv[] = {"FelzTrace", "delete", "mystore"};
    felztrace::ReturnCode result = felztrace::parseCli(3, argv, store, in);

    EXPECT_EQ(result, felztrace::ReturnCode::Success);
    EXPECT_TRUE(store.deleteCalled);
    EXPECT_EQ(store.lastDeleteName, "mystore");
}

TEST_F(CLITest, DeleteSuccessWithUppercaseY)
{
    std::istringstream in("Y");
    const char* argv[] = {"FelzTrace", "delete", "mystore"};
    felztrace::ReturnCode result = felztrace::parseCli(3, argv, store, in);

    EXPECT_EQ(result, felztrace::ReturnCode::Success);
    EXPECT_TRUE(store.deleteCalled);
    EXPECT_EQ(store.lastDeleteName, "mystore");
}
