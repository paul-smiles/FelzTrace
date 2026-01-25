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
    int result = felztrace::parse_cli(1, argv);

    EXPECT_EQ(result, 0);
    EXPECT_EQ(spdlog::get_level(), spdlog::level::info);
}

TEST_F(CLITest, HelpShortOption)
{
    const char* argv[] = {"FelzTrace", "-h"};
    int result = felztrace::parse_cli(2, argv);

    EXPECT_EQ(result, 0);
}

TEST_F(CLITest, HelpLongOption)
{
    const char* argv[] = {"FelzTrace", "--help"};
    int result = felztrace::parse_cli(2, argv);

    EXPECT_EQ(result, 0);
}

TEST_F(CLITest, VersionShortOption)
{
    const char* argv[] = {"FelzTrace", "-v"};
    int result = felztrace::parse_cli(2, argv);

    EXPECT_EQ(result, 0);
}

TEST_F(CLITest, VersionLongOption)
{
    const char* argv[] = {"FelzTrace", "--version"};
    int result = felztrace::parse_cli(2, argv);

    EXPECT_EQ(result, 0);
}

TEST_F(CLITest, DebugShortOption)
{
    const char* argv[] = {"FelzTrace", "-d"};
    int result = felztrace::parse_cli(2, argv);

    EXPECT_EQ(result, 0);
    EXPECT_EQ(spdlog::get_level(), spdlog::level::debug);
}

TEST_F(CLITest, DebugLongOption)
{
    const char* argv[] = {"FelzTrace", "--debug"};
    int result = felztrace::parse_cli(2, argv);

    EXPECT_EQ(result, 0);
    EXPECT_EQ(spdlog::get_level(), spdlog::level::debug);
}

TEST_F(CLITest, QuietShortOption)
{
    const char* argv[] = {"FelzTrace", "-q"};
    int result = felztrace::parse_cli(2, argv);

    EXPECT_EQ(result, 0);
    EXPECT_EQ(spdlog::get_level(), spdlog::level::err);
}

TEST_F(CLITest, QuietLongOption)
{
    const char* argv[] = {"FelzTrace", "--quiet"};
    int result = felztrace::parse_cli(2, argv);

    EXPECT_EQ(result, 0);
    EXPECT_EQ(spdlog::get_level(), spdlog::level::err);
}

TEST_F(CLITest, UnknownOption)
{
    const char* argv[] = {"FelzTrace", "--unknown"};
    int result = felztrace::parse_cli(2, argv);

    EXPECT_EQ(result, 1);
}

TEST_F(CLITest, UnknownShortOption)
{
    const char* argv[] = {"FelzTrace", "-x"};
    int result = felztrace::parse_cli(2, argv);

    EXPECT_EQ(result, 1);
}

TEST_F(CLITest, MultipleOptions)
{
    const char* argv[] = {"FelzTrace", "-d", "-q"};
    int result = felztrace::parse_cli(3, argv);

    EXPECT_EQ(result, 0);
    // Last option should win
    EXPECT_EQ(spdlog::get_level(), spdlog::level::err);
}

TEST_F(CLITest, DebugThenNormalExecution)
{
    const char* argv[] = {"FelzTrace", "--debug"};
    int result = felztrace::parse_cli(2, argv);

    EXPECT_EQ(result, 0);
    EXPECT_EQ(spdlog::get_level(), spdlog::level::debug);
}
