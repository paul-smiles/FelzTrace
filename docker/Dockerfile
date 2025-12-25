FROM ubuntu:24.04 AS builder

ENV DEBIAN_FRONTEND=noninteractive

# Install build tools and dependencies
RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential \
    cmake \
    git \
    ca-certificates\
    && update-ca-certificates \
    && rm -rf /var/lib/apt/lists/*

# Create build directory
WORKDIR /repository
# Run the build when the container starts
# Use a shell command to ensure environment variables and piping work
ENTRYPOINT ["sh", "-c", "cmake -S . -B build -DCMAKE_BUILD_TYPE=Release && cmake --build build --config Release -- -j$(nproc)"]

FROM ubuntu:24.04 AS runtime


RUN apt-get update && apt-get install -y --no-install-recommends \
    libstdc++6 \
    ca-certificates \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /repository
COPY /build/FelzTrace /repository/FelzTrace

# Optional: copy an entrypoint script that drops you in bash
COPY docker/entrypoint.sh /entrypoint.sh
RUN chmod +x /entrypoint.sh
# Default: start an interactive shell (or run passed command)
ENTRYPOINT ["/entrypoint.sh"]