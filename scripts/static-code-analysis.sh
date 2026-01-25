#!/usr/bin/env bash
set -e

cmake --build build-release --target clang-tidy
cmake --build build-release --target static-analyze
cmake --build build-release --target cppcheck