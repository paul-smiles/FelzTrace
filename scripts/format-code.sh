#!/usr/bin/env bash
set -e

# Format all C/C++ source and header files in src/ and include/
clang-format -i \
  $(find src include tests -type f \( \
    -name "*.cpp" -o \
    -name "*.cc"  -o \
    -name "*.cxx" -o \
    -name "*.h"   -o \
    -name "*.hpp" \
  \))