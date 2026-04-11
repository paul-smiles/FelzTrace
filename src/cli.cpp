#include "cli.h"
#include "version.h"
#include <exception>
#include <spdlog/spdlog.h>
#include <stdexcept>
#include <string>

namespace felztrace
{

namespace
{

struct CommandResult
{
    bool shouldReturn;
    ReturnCode code;
    int consumedArgs;
};

CommandResult continueParsing(int consumedArgs = 0)
{
    return {false, ReturnCode::Success, consumedArgs};
}

CommandResult stopParsing(ReturnCode code) { return {true, code, 0}; }

void configureDefaultLogging()
{
    spdlog::set_level(spdlog::level::info);
    spdlog::set_pattern("%v");
}

void enableVerboseLogging()
{
    spdlog::debug("Verbose mode enabled - showing debug logs");
    spdlog::set_level(spdlog::level::debug);
    spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] %v");
}

void enableQuietLogging()
{
    spdlog::info("Quiet mode enabled - only errors will be shown");
    spdlog::set_level(spdlog::level::err);
}

bool isHelpOption(const std::string& arg) { return arg == "-h" || arg == "--help"; }

void printHelp()
{
    spdlog::info(R"(FelzTrace - A requirements tracing application
Usage: FelzTrace [OPTIONS] <COMMAND>

Positional arguments:
    create          Create a new requirement store with the specified name at the given path
    delete          Delete a requirement store by name

Options:
    -h, --help      Show this help message
    -V, --version   Show version information
    -v, --verbose   Enable verbose logging
    -q, --quiet     Suppress all output except errors
)");
}

void printCreateHelp()
{
    spdlog::info(R"(FelzTrace create - Create a requirement store
Usage:
    FelzTrace [GLOBAL_OPTIONS] create [OPTIONS] <name> <path> <level>

Options:
    -h, --help  Show this help message
Global Options:
    -V, --version   Show version information
    -v, --verbose   Enable verbose logging
    -q, --quiet     Suppress all output except errors
)");
}

void printDeleteHelp()
{
    spdlog::info(R"(FelzTrace delete - Delete a requirement store
Usage:
    FelzTrace [GLOBAL_OPTIONS] delete [OPTIONS] <name>

Options:
    -h, --help  Show this help message
Global Options:
    -V, --version   Show version information
    -v, --verbose   Enable verbose logging
    -q, --quiet     Suppress all output except errors
)");
}

void printVersion()
{
    spdlog::info("FelzTrace commit count: {}", VERSION);
    spdlog::info("FelzTrace commit hash: {}", COMMIT_HASH);
}

bool nextArgIsHelp(int argc, const char* argv[], int index)
{
    return index + 1 < argc && isHelpOption(argv[index + 1]);
}

bool tryParseLevel(const std::string& levelArg, int& level)
{
    try
    {
        size_t parsedChars = 0;
        level = std::stoi(levelArg, &parsedChars);
        return parsedChars == levelArg.size();
    }
    catch (const std::exception&)
    {
        return false;
    }
}

CommandResult handleCreateCommand(int argc, const char* argv[], int index, RequirementStore& store)
{
    if (nextArgIsHelp(argc, argv, index))
    {
        printCreateHelp();
        return stopParsing(ReturnCode::Success);
    }

    if (index + 3 >= argc)
    {
        spdlog::error("Missing required arguments for create");
        printCreateHelp();
        return stopParsing(ReturnCode::Error);
    }

    const std::string name = argv[index + 1];
    const std::string path = argv[index + 2];
    const std::string levelArg = argv[index + 3];
    int level = 0;

    if (!tryParseLevel(levelArg, level))
    {
        spdlog::error("Invalid level '{}'. Level must be an integer.", levelArg);
        printCreateHelp();
        return stopParsing(ReturnCode::Error);
    }

    try
    {
        store.createRequirementStore(name, path, level);
    }
    catch (const std::exception& ex)
    {
        spdlog::error("Failed to create requirement store: {}", ex.what());
        return stopParsing(ReturnCode::Error);
    }

    return continueParsing(3);
}

bool deleteConfirmed(const std::string& name, std::istream& input)
{
    spdlog::info("Are you sure you want to delete requirement store '{}'? [y/N]: ", name);

    std::string confirmation;
    std::getline(input, confirmation);
    return confirmation == "y" || confirmation == "Y";
}

CommandResult handleDeleteCommand(int argc, const char* argv[], int index, RequirementStore& store,
                                  std::istream& input)
{
    if (nextArgIsHelp(argc, argv, index))
    {
        printDeleteHelp();
        return stopParsing(ReturnCode::Success);
    }

    if (index + 1 >= argc)
    {
        spdlog::error("Missing required argument <name> for delete");
        printDeleteHelp();
        return stopParsing(ReturnCode::Error);
    }

    const std::string name = argv[index + 1];
    if (!deleteConfirmed(name, input))
    {
        spdlog::info("Delete cancelled.");
        return stopParsing(ReturnCode::Success);
    }

    try
    {
        store.deleteRequirementStore(name);
    }
    catch (const std::exception& ex)
    {
        spdlog::error("Failed to delete requirement store: {}", ex.what());
        return stopParsing(ReturnCode::Error);
    }

    return continueParsing(1);
}

} // anonymous namespace

ReturnCode parseCli(int argc, const char* argv[], RequirementStore& store, std::istream& input)
{
    configureDefaultLogging();

    for (int i = 1; i < argc; ++i)
    {
        std::string arg = argv[i];

        if (arg == "-h" || arg == "--help")
        {
            printHelp();
            return ReturnCode::Success;
        }
        if (arg == "-V" || arg == "--version")
        {
            printVersion();
            return ReturnCode::Success;
        }
        if (arg == "create")
        {
            const CommandResult result = handleCreateCommand(argc, argv, i, store);
            if (result.shouldReturn)
            {
                return result.code;
            }
            i += result.consumedArgs;
            continue;
        }
        if (arg == "delete")
        {
            const CommandResult result = handleDeleteCommand(argc, argv, i, store, input);
            if (result.shouldReturn)
            {
                return result.code;
            }
            i += result.consumedArgs;
            continue;
        }
        if (arg == "-v" || arg == "--verbose")
        {
            enableVerboseLogging();
            continue;
        }
        if (arg == "-q" || arg == "--quiet")
        {
            enableQuietLogging();
            continue;
        }

        spdlog::error("Unknown option: {}", arg);
        spdlog::error("Use -h or --help for usage information");
        return ReturnCode::Error;
    }

    return ReturnCode::Success;
}

} // namespace felztrace
