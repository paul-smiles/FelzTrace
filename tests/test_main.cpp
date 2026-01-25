#include <array>
#include <cstdlib>
#include <gtest/gtest.h>
#include <memory>
#include <stdexcept>
#include <string>

// Helper function to execute a command and capture output
std::string exec(const char* cmd)
{
    std::array<char, 128> buffer;
    std::string result;
    std::unique_ptr<FILE, int (*)(FILE*)> pipe(popen(cmd, "r"), pclose);
    if (!pipe)
    {
        throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr)
    {
        result += buffer.data();
    }
    return result;
}

// Integration tests - execute the actual binary to cover main()
TEST(MainCoverage, ExecuteBinaryNoArgs)
{
    std::string output = exec("../FelzTrace 2>&1");
    EXPECT_NE(output.find("FelzTrace: This is an info message!"), std::string::npos);
}

TEST(MainCoverage, ExecuteBinaryHelp)
{
    std::string output = exec("../FelzTrace -h 2>&1");
    EXPECT_NE(output.find("FelzTrace - A requirements tracing application"), std::string::npos);
}

TEST(MainCoverage, ExecuteBinaryVersion)
{
    std::string output = exec("../FelzTrace -v 2>&1");
    EXPECT_NE(output.find("FelzTrace commit"), std::string::npos);
}