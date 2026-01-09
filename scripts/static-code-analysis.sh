#!/usr/bin/env bash
set -e

cmake --build build --target clang-tidy
cmake --build build --target static-analyze
cmake --build build --target cppcheck