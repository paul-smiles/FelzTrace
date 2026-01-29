#include "cli.h"
#include <gtest/gtest.h>
#include <spdlog/spdlog.h>

class CLITest : public ::testing::Test
{
  protected:
    void SetUp() override
    {
        // Reset log level before each test
        spdlog::set_level(spdlog::level::info);
    }
};

TEST_F(CLITest, NoArguments)
{
    const char* argv[] = {"FelzTrace"};
    felztrace::ReturnCode result = felztrace::parseCli(1, argv);

    EXPECT_EQ(result, felztrace::ReturnCode::Success);
    EXPECT_EQ(spdlog::get_level(), spdlog::level::info);
}

TEST_F(CLITest, HelpShortOption)
{
    const char* argv[] = {"FelzTrace", "-h"};
    felztrace::ReturnCode result = felztrace::parseCli(2, argv);

    EXPECT_EQ(result, felztrace::ReturnCode::Success);
}

TEST_F(CLITest, HelpLongOption)
{
    const char* argv[] = {"FelzTrace", "--help"};
    felztrace::ReturnCode result = felztrace::parseCli(2, argv);

    EXPECT_EQ(result, felztrace::ReturnCode::Success);
}

TEST_F(CLITest, VersionShortOption)
{
    const char* argv[] = {"FelzTrace", "-v"};
    felztrace::ReturnCode result = felztrace::parseCli(2, argv);

    EXPECT_EQ(result, felztrace::ReturnCode::Success);
}

TEST_F(CLITest, VersionLongOption)
{
    const char* argv[] = {"FelzTrace", "--version"};
    felztrace::ReturnCode result = felztrace::parseCli(2, argv);

    EXPECT_EQ(result, felztrace::ReturnCode::Success);
}

TEST_F(CLITest, DebugShortOption)
{
    const char* argv[] = {"FelzTrace", "-d"};
    felztrace::ReturnCode result = felztrace::parseCli(2, argv);

    EXPECT_EQ(result, felztrace::ReturnCode::Success);
    EXPECT_EQ(spdlog::get_level(), spdlog::level::debug);
}

TEST_F(CLITest, DebugLongOption)
{
    const char* argv[] = {"FelzTrace", "--debug"};
    felztrace::ReturnCode result = felztrace::parseCli(2, argv);

    EXPECT_EQ(result, felztrace::ReturnCode::Success);
    EXPECT_EQ(spdlog::get_level(), spdlog::level::debug);
}

TEST_F(CLITest, QuietShortOption)
{
    const char* argv[] = {"FelzTrace", "-q"};
    felztrace::ReturnCode result = felztrace::parseCli(2, argv);

    EXPECT_EQ(result, felztrace::ReturnCode::Success);
    EXPECT_EQ(spdlog::get_level(), spdlog::level::err);
}

TEST_F(CLITest, QuietLongOption)
{
    const char* argv[] = {"FelzTrace", "--quiet"};
    felztrace::ReturnCode result = felztrace::parseCli(2, argv);

    EXPECT_EQ(result, felztrace::ReturnCode::Success);
    EXPECT_EQ(spdlog::get_level(), spdlog::level::err);
}

TEST_F(CLITest, UnknownOption)
{
    const char* argv[] = {"FelzTrace", "--unknown"};
    felztrace::ReturnCode result = felztrace::parseCli(2, argv);

    EXPECT_EQ(result, felztrace::ReturnCode::Error);
}

TEST_F(CLITest, UnknownShortOption)
{
    const char* argv[] = {"FelzTrace", "-x"};
    felztrace::ReturnCode result = felztrace::parseCli(2, argv);

    EXPECT_EQ(result, felztrace::ReturnCode::Error);
}

TEST_F(CLITest, MultipleOptions)
{
    const char* argv[] = {"FelzTrace", "-d", "-q"};
    felztrace::ReturnCode result = felztrace::parseCli(3, argv);

    EXPECT_EQ(result, felztrace::ReturnCode::Success);
    // Last option should win
    EXPECT_EQ(spdlog::get_level(), spdlog::level::err);
}

TEST_F(CLITest, DebugThenNormalExecution)
{
    const char* argv[] = {"FelzTrace", "--debug"};
    felztrace::ReturnCode result = felztrace::parseCli(2, argv);

    EXPECT_EQ(result, felztrace::ReturnCode::Success);
    EXPECT_EQ(spdlog::get_level(), spdlog::level::debug);
}
