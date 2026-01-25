cmake_minimum_required(VERSION 3.16)

# ---------------------------------------------------------
# Git version information
# ---------------------------------------------------------
find_package(Git QUIET)
if(GIT_FOUND)
  # Get commit count for project version
  execute_process(
    COMMAND ${GIT_EXECUTABLE} rev-list --count HEAD
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_COMMIT_COUNT
    OUTPUT_STRIP_TRAILING_WHITESPACE
    ERROR_QUIET
  )
  
  # Get commit hash
  execute_process(
    COMMAND ${GIT_EXECUTABLE} rev-parse --short HEAD
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_COMMIT_HASH
    OUTPUT_STRIP_TRAILING_WHITESPACE
    ERROR_QUIET
  )
  
  # Check if working directory is dirty
  execute_process(
    COMMAND ${GIT_EXECUTABLE} diff-index --quiet HEAD --
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    RESULT_VARIABLE GIT_DIRTY
    ERROR_QUIET
  )
  
  if(GIT_DIRTY)
    set(GIT_VERSION "${GIT_COMMIT_HASH}-dirty")
  else()
    set(GIT_VERSION "${GIT_COMMIT_HASH}")
  endif()
  
else()
  set(GIT_VERSION "unknown")
  set(GIT_COMMIT_HASH "unknown")
endif()

# Provide default values if git commands failed
if(NOT GIT_COMMIT_COUNT OR GIT_COMMIT_COUNT STREQUAL "")
  set(GIT_COMMIT_COUNT "0")
endif()

if(NOT GIT_COMMIT_HASH OR GIT_COMMIT_HASH STREQUAL "")
  set(GIT_COMMIT_HASH "unknown")
endif()

project(FelzTrace
    VERSION ${GIT_COMMIT_COUNT}
    LANGUAGES CXX
)

set(CMAKE_MESSAGE_LOG_LEVEL VERBOSE)

# C++ standard
set(CMAKE_CXX_STANDARD 20)
# enforce the standard being used
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# strict C++ no extensions (e.g., no GNU extensions)
set(CMAKE_CXX_EXTENSIONS OFF)
# Enable generation of compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(CTest)
enable_testing()

# FetchContent settings
include(FetchContent)
set(FETCHCONTENT_QUIET OFF)

# ---------------------------------------------------------
# googletest
# ---------------------------------------------------------
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG v1.17.0
)

FetchContent_MakeAvailable(googletest)

# ---------------------------------------------------------
# Fetch spdlog
# ---------------------------------------------------------
FetchContent_Declare(
    spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG        v1.16.0
)

FetchContent_MakeAvailable(spdlog)

# Configure version header
configure_file(
  ${CMAKE_SOURCE_DIR}/include/version.h.in
  ${CMAKE_BINARY_DIR}/include/version.h
  @ONLY
)

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

# ---------------------------------------------------------
# Code coverage settings
# ---------------------------------------------------------
option(ENABLE_COVERAGE "Enable GCC coverage flags" OFF)

if(ENABLE_COVERAGE)
  if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    add_compile_options(--coverage -O0 -g)
    add_link_options(--coverage)
  else()
    message(FATAL_ERROR "ENABLE_COVERAGE ist für GCC gedacht.")
  endif()
endif()

#---------------------------------------------------------
# Coverage target using gcovr
#---------------------------------------------------------
find_program(GCOVR_EXE gcovr)

if(GCOVR_EXE)
  add_custom_target(coverage
    COMMAND ${CMAKE_CTEST_COMMAND} --test-dir ${CMAKE_BINARY_DIR} --output-on-failure
    COMMAND ${GCOVR_EXE}
            -r ${CMAKE_SOURCE_DIR}
            ${CMAKE_BINARY_DIR}
            --filter "^(src|include)/"
            --exclude ".*/_deps/.*"
            --html --html-details -o ${CMAKE_BINARY_DIR}/coverage.html
    COMMAND ${GCOVR_EXE}
            -r ${CMAKE_SOURCE_DIR}
            ${CMAKE_BINARY_DIR}
            --filter "^(src|include)/"
            --exclude ".*/_deps/.*"
            --xml-pretty -o ${CMAKE_BINARY_DIR}/coverage.xml
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Running tests + generating coverage report (gcovr)"
    VERBATIM
  )
else()
  message(WARNING "gcovr nicht gefunden – target 'coverage' deaktiviert")
endif()


# Source files
set(SOURCES
    src/main.cpp
    src/cli.cpp
    src/core.cpp
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
        ${CMAKE_BINARY_DIR}/include
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        spdlog::spdlog
)

add_subdirectory(tests)