#include "cli.h"
#include "core.h"
#include "requirement_store_yml.h"

int main(int argc, const char* argv[])
{
    felztrace::RequirementStoreYml store;
    felztrace::ReturnCode returnCode = felztrace::parseCli(argc, argv, store);
    if (returnCode == felztrace::ReturnCode::Success)
    {
        felztrace::run();
    }
    return static_cast<int>(returnCode);
}