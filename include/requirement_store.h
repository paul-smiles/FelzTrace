#pragma once

#include <string>

namespace felztrace
{

class RequirementStore
{
  public:
    virtual ~RequirementStore() = default;

    virtual void createRequirementStore(const std::string& name, const std::string& path,
                                        int level) = 0;
    virtual void createTestStore(const std::string& name, const std::string& path, int level) = 0;
    virtual void deleteStore(const std::string& name) = 0;
    // virtual void addRequirement(const std::string& storeName) = 0;
};

} // namespace felztrace
