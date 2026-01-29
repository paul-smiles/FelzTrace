#include "cli.h"
#include "core.h"

int main(int argc, const char* argv[])
{
    felztrace::ReturnCode returnCode = felztrace::parseCli(argc, argv);
    if (returnCode == felztrace::ReturnCode::Success)
    {
        felztrace::run();
    }
    return static_cast<int>(returnCode);
}