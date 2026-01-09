#!/usr/bin/env bash
set -euo pipefail

# Optional: assure we are inside a git repository
git rev-parse --is-inside-work-tree >/dev/null

# Collect files and check clang-format
git ls-files -z '*.cpp' '*.h' '*.hpp' \
  | xargs -0 -r clang-format --dry-run -Werror