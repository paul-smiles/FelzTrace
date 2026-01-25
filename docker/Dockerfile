FROM ubuntu:24.04 AS builder

ENV DEBIAN_FRONTEND=noninteractive

# Install build tools and dependencies
RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential \
    clang \
    clang-tools \
    clang-format \
    clang-tidy \
    cppcheck \
    cmake \
    git \
    gcovr \
    ca-certificates \
    && update-ca-certificates \
    && rm -rf /var/lib/apt/lists/*

# Create build directory
WORKDIR /repository

FROM ubuntu:24.04 AS runtime


RUN apt-get update && apt-get install -y --no-install-recommends \
    libstdc++6 \
    ca-certificates \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /repository
COPY ./build-release/FelzTrace /usr/local/bin/FelzTrace