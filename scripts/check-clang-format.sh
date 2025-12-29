#!/usr/bin/env bash
set -euo pipefail

# Optional: sicherstellen, dass wir wirklich in einem Git-Repo sind
git rev-parse --is-inside-work-tree >/dev/null

# Dateien einsammeln und clang-format prüfen
git ls-files -z '*.cpp' '*.h' '*.hpp' \
  | xargs -0 -r clang-format --dry-run -Werror