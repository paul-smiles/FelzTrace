#pragma once

#include <string>
#include <vector>

namespace felztrace
{

struct RequirementConfig
{
    // std::string id;
    std::string description = "";
    std::string validates = "";
    std::string validatedBy = "";
    std::string verifies = "";
    std::string verifiedBy = "";
    std::vector<std::string> tags = {};
    bool derived = false;
};

} // namespace felztrace
