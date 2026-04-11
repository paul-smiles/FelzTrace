#pragma once

#include <cstdint>
#include <iostream>

#include "requirement_store.h"

namespace felztrace
{

enum class ReturnCode : std::uint8_t
{
    Success = 0,
    Error = 1
};

ReturnCode parseCli(int argc, const char* argv[], RequirementStore& store,
                    std::istream& input = std::cin);

} // namespace felztrace
