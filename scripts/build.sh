#!/usr/bin/env sh
set -e

cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release -- -j$(nproc)