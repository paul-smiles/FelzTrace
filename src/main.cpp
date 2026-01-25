#include "cli.h"
#include "core.h"

int main(int argc, const char* argv[])
{
    int returnCode = felztrace::parse_cli(argc, argv);
    if (returnCode == 0)
    {
        felztrace::run();
    }
    return returnCode;
}