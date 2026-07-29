#pragma once

#include "filesystem_interface.h"
#include "requirement_store.h"
#include "requirement_store_config.h"
#include <memory>
#include <string>
#include <yaml-cpp/yaml.h>

namespace felztrace
{

enum class StoreType
{
    Requirements,
    Tests
};

class RequirementStoreYml : public RequirementStore
{
  private:
    struct StoreSettings
    {
        std::string storeName;
        std::string path;
        int level;
        StoreType storeType;
    };
    std::unique_ptr<IFilesystem> m_filesystem;
    void createStore(const StoreSettings& storeSettings);

  public:
    explicit RequirementStoreYml(std::unique_ptr<IFilesystem> filesystem = nullptr);
    ~RequirementStoreYml() override = default;

    void createRequirementStore(const std::string& name, const std::string& path,
                                int level) override;
    void createTestStore(const std::string& name, const std::string& path, int level) override;
    void deleteStore(const std::string& name) override;
    // void addRequirement(const std::string& storeName) override;
};

} // namespace felztrace
