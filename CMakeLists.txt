cmake_minimum_required(VERSION 3.16)

set(CMAKE_MESSAGE_LOG_LEVEL VERBOSE)

project(FelzTrace
    VERSION 1.0 # Todo: flag for release pipeline to update this automatically
    LANGUAGES CXX
)

# C++ standard
set(CMAKE_CXX_STANDARD 20)
# enforce the standard being used
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# strict C++ no extensions (e.g., no GNU extensions)
set(CMAKE_CXX_EXTENSIONS OFF)

# FetchContent settings
set(FETCHCONTENT_QUIET OFF)

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

# ---------------------------------------------------------
# Enable clang-tidy for static analysis
# ---------------------------------------------------------
find_program(CLANG_TIDY_EXE NAMES clang-tidy)

if(CLANG_TIDY_EXE)
  file(GLOB_RECURSE CLANG_TIDY_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_SOURCE_DIR}/src/*.cpp
    ${CMAKE_SOURCE_DIR}/src/*.cc
    ${CMAKE_SOURCE_DIR}/src/*.cxx
    ${CMAKE_SOURCE_DIR}/include/*.h
    ${CMAKE_SOURCE_DIR}/include/*.hpp
  )

  add_custom_target(clang-tidy
    COMMAND ${CLANG_TIDY_EXE}
            -p=${CMAKE_BINARY_DIR}
            ${CLANG_TIDY_SOURCES}
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Running clang-tidy (using .clang-tidy)"
    VERBATIM
  )
else()
  message(WARNING "clang-tidy not found — 'clang-tidy' target disabled")
endif()

# ---------------------------------------------------------
# Static analysis target using clang's scan-build
# ---------------------------------------------------------
add_custom_target(static-analyze
  COMMAND scan-build
          --status-bugs
          -analyze-headers
          -enable-checker core,unix,deadcode,cplusplus
          cmake --build ${CMAKE_BINARY_DIR}
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  COMMENT "Running static analysis using clang's scan-build"
)

# ---------------------------------------------------------
# cppcheck target for static analysis
# ---------------------------------------------------------
# Suppress cppcheck findings for dependencies fetched into _deps (e.g., spdlog via FetchContent)
add_custom_target(cppcheck
  COMMAND cppcheck
          --project=${CMAKE_BINARY_DIR}/compile_commands.json
          --enable=warning,style,performance,portability
          --inconclusive
          --inline-suppr
          --suppress=missingIncludeSystem
          --template=gcc
          --error-exitcode=2
          --suppress=*:*/_deps/*
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  COMMENT "Running cppcheck (using compile_commands.json)"
)

# Source files
set(SOURCES
    src/main.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})

# Compiler warnings
target_compile_options(${PROJECT_NAME} PRIVATE
    -Wall
    -Wextra
    -Wpedantic
    -Wshadow
    -Wconversion
    -Wsign-conversion
    -Wformat=2
    -Wnull-dereference
)

# Treat warnings as errors in CI environment
if(DEFINED ENV{CI})
  target_compile_options(${PROJECT_NAME} PRIVATE -Werror)
endif()

# Include headers
target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        spdlog::spdlog
)