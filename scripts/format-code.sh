#!/usr/bin/env sh
set -e

# Format all C/C++ source and header files in src/ and include/
clang-format -i \
  $(find src include -type f \( \
    -name "*.cpp" -o \
    -name "*.cc"  -o \
    -name "*.cxx" -o \
    -name "*.h"   -o \
    -name "*.hpp" \
  \))