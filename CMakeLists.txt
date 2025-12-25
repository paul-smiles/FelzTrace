cmake_minimum_required(VERSION 3.16)

set(CMAKE_MESSAGE_LOG_LEVEL VERBOSE)

project(FelzTrace
    VERSION 1.0 # Todo: flag for release pipeline to update this automatically
    LANGUAGES CXX
)

set(FETCHCONTENT_QUIET OFF)

# C++ standard
set(CMAKE_CXX_STANDARD 20)
# enforce the standard being used
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# strict C++ no extensions (e.g., no GNU extensions)
set(CMAKE_CXX_EXTENSIONS OFF)

# ---------------------------------------------------------
# Fetch spdlog
# ---------------------------------------------------------
include(FetchContent)

FetchContent_Declare(
    spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG        v1.16.0
)

FetchContent_MakeAvailable(spdlog)

# Source files
set(SOURCES
    src/main.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})

# Include headers
target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        spdlog::spdlog
)