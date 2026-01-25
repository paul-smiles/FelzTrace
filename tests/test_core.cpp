#include "core.h"
#include <gtest/gtest.h>
#include <spdlog/spdlog.h>

class CoreTest : public ::testing::Test
{
  protected:
    void SetUp() override
    {
        // Reset log level before each test
        spdlog::set_level(spdlog::level::info);
    }
};

TEST_F(CoreTest, RunExecutesSuccessfully)
{
    // Test that run executes without throwing exceptions
    EXPECT_NO_THROW(felztrace::run());
}
