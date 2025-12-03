#!/usr/bin/env bash
set -e

# If no arguments -> interactive shell
if [ "$#" -eq 0 ]; then
    exec /bin/bash
else
    # If args provided -> run them
    exec "$@"
fi
