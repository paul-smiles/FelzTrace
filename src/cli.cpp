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
    spdlog::set_level(spdlog::level::debug);
    spdlog::debug("Verbose mode enabled - showing debug logs");
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
Usage: FelzTrace [OPTIONS] [COMMAND]

Commands:
    reqs init       Create a new requirement store with the specified name at the given path
    tests init      Create a new test store with the specified name at the given path
    delete          Delete a store by name

Options:
    -h, --help      Show this help message
    -V, --version   Show version information
    -v, --verbose   Enable verbose logging
    -q, --quiet     Suppress all output except errors
)");
}

void printReqsHelp()
{
    spdlog::info(R"(FelzTrace reqs init - Create a requirement store
Usage:
    FelzTrace [GLOBAL_OPTIONS] reqs init [OPTIONS] <name> <path> <level>

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
    spdlog::info(R"(FelzTrace delete - Delete a store
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

void printTestsHelp()
{
    spdlog::info(R"(FelzTrace tests init - Create a test store
Usage:
    FelzTrace [GLOBAL_OPTIONS] tests init [OPTIONS] <name> <path> <level>

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

bool isValidOption(const std::string& arg)
{
    return arg == "-h" || arg == "--help" || arg == "-V" || arg == "--version" || arg == "-v" ||
           arg == "--verbose" || arg == "-q" || arg == "--quiet";
}

bool validateTrailingArgs(int argc, const char* argv[], int startIndex)
{
    for (int i = startIndex; i < argc; ++i)
    {
        if (!isValidOption(argv[i]))
        {
            spdlog::error("Unknown option: {}", argv[i]);
            spdlog::error("Use -h or --help for usage information");
            return false;
        }
    }
    return true;
}

void applyTrailingLoggingOptions(int argc, const char* argv[], int startIndex)
{
    for (int i = startIndex; i < argc; ++i)
    {
        const std::string option = argv[i];
        if (option == "-v" || option == "--verbose")
        {
            enableVerboseLogging();
        }
        else if (option == "-q" || option == "--quiet")
        {
            enableQuietLogging();
        }
    }
}

bool commandArgIsHelp(int argc, const char* argv[], int index)
{
    return index < argc && isHelpOption(argv[index]);
}

using CreateStoreAction = void (RequirementStore::*)(const std::string&, const std::string&, int);

CommandResult handleInitCommand(int argc, const char* argv[], int index, const std::string& command,
                                void (*printCommandHelp)(), RequirementStore& store,
                                CreateStoreAction createStoreAction,
                                const std::string& createStoreErrorLabel)
{
    if (commandArgIsHelp(argc, argv, index + 1))
    {
        printCommandHelp();
        return stopParsing(ReturnCode::Success);
    }

    if (index + 1 >= argc || std::string(argv[index + 1]) != "init")
    {
        spdlog::error("Missing required subcommand 'init' for {}", command);
        printCommandHelp();
        return stopParsing(ReturnCode::Error);
    }

    if (commandArgIsHelp(argc, argv, index + 2))
    {
        printCommandHelp();
        return stopParsing(ReturnCode::Success);
    }

    if (index + 4 >= argc)
    {
        spdlog::error("Missing required arguments for {} init", command);
        printCommandHelp();
        return stopParsing(ReturnCode::Error);
    }

    const std::string name = argv[index + 2];
    const std::string path = argv[index + 3];
    const std::string levelArg = argv[index + 4];
    int level = 0;

    if (!tryParseLevel(levelArg, level))
    {
        spdlog::error("Invalid level '{}'. Level must be an integer.", levelArg);
        printCommandHelp();
        return stopParsing(ReturnCode::Error);
    }

    if (!validateTrailingArgs(argc, argv, index + 5))
    {
        return stopParsing(ReturnCode::Error);
    }

    applyTrailingLoggingOptions(argc, argv, index + 5);

    try
    {
        (store.*createStoreAction)(name, path, level);
    }
    catch (const std::exception& ex)
    {
        spdlog::error("Failed to create {} store: {}", createStoreErrorLabel, ex.what());
        return stopParsing(ReturnCode::Error);
    }

    return continueParsing(4);
}

CommandResult handleReqsInitCommand(int argc, const char* argv[], int index,
                                    RequirementStore& store)
{
    return handleInitCommand(argc, argv, index, "reqs", printReqsHelp, store,
                             &RequirementStore::createRequirementStore, "requirement");
}

CommandResult handleTestsInitCommand(int argc, const char* argv[], int index,
                                     RequirementStore& store)
{
    return handleInitCommand(argc, argv, index, "tests", printTestsHelp, store,
                             &RequirementStore::createTestStore, "test");
}

bool deleteConfirmed(const std::string& name, std::istream& input)
{
    // Temporarily restore log level for interactive prompt (bypasses quiet mode)
    auto savedLevel = spdlog::get_level();
    spdlog::set_level(spdlog::level::info);

    spdlog::info("Are you sure you want to delete store '{}'? [y/N]: ", name);

    // Restore previous log level
    spdlog::set_level(savedLevel);

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

    // Validate trailing arguments before prompting user
    if (!validateTrailingArgs(argc, argv, index + 2))
    {
        return stopParsing(ReturnCode::Error);
    }

    if (!deleteConfirmed(name, input))
    {
        spdlog::info("Delete cancelled.");
        return stopParsing(ReturnCode::Success);
    }

    try
    {
        store.deleteStore(name);
    }
    catch (const std::exception& ex)
    {
        spdlog::error("Failed to delete store: {}", ex.what());
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
        if (arg == "reqs")
        {
            const CommandResult result = handleReqsInitCommand(argc, argv, i, store);
            if (result.shouldReturn)
            {
                return result.code;
            }
            i += result.consumedArgs;
            continue;
        }
        if (arg == "tests")
        {
            const CommandResult result = handleTestsInitCommand(argc, argv, i, store);
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
