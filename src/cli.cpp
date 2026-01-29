#include "cli.h"
#include "version.h"
#include <spdlog/spdlog.h>
#include <string>

namespace felztrace
{

namespace
{

void printHelp()
{
    spdlog::info("FelzTrace - A requirements tracing application\n");
    spdlog::info("Usage: FelzTrace [OPTIONS]\n");
    spdlog::info("Options:");
    spdlog::info("  -h, --help       Show this help message");
    spdlog::info("  -v, --version    Show version information");
    spdlog::info("  -d, --debug      Enable debug logging");
    spdlog::info("  -q, --quiet      Suppress all output except errors");
}

void printVersion()
{
    spdlog::info("FelzTrace commit count: {}", VERSION);
    spdlog::info("FelzTrace commit hash: {}", COMMIT_HASH);
}

} // anonymous namespace

ReturnCode parseCli(int argc, const char* argv[])
{
    // Set default log level to info
    spdlog::set_level(spdlog::level::info);

    // Parse command line arguments
    for (int i = 1; i < argc; ++i)
    {
        std::string arg = argv[i];

        if (arg == "-h" || arg == "--help")
        {
            printHelp();
            return ReturnCode::Success;
        }
        if (arg == "-v" || arg == "--version")
        {
            printVersion();
            return ReturnCode::Success;
        }
        if (arg == "-d" || arg == "--debug")
        {
            spdlog::set_level(spdlog::level::debug);
            spdlog::debug("Debug mode enabled");
        }
        else if (arg == "-q" || arg == "--quiet")
        {
            spdlog::set_level(spdlog::level::err);
        }
        else
        {
            spdlog::error("Unknown option: {}", arg);
            spdlog::error("Use -h or --help for usage information");
            return ReturnCode::Error;
        }
    }
    return ReturnCode::Success;
}

} // namespace felztrace
