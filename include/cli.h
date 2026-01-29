#pragma once

namespace felztrace
{

enum class ReturnCode
{
    Success = 0,
    Error = 1
};

ReturnCode parseCli(int argc, const char* argv[]);

} // namespace felztrace
