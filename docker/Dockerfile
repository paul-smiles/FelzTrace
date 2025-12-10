FROM ubuntu:24.04

ENV DEBIAN_FRONTEND=noninteractive

# Install build tools and dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    && rm -rf /var/lib/apt/lists/*

# Create app directory
WORKDIR /app

# Optional: copy an entrypoint script that drops you in bash
COPY docker/entrypoint.sh /entrypoint.sh
RUN chmod +x /entrypoint.sh
# Default: start an interactive shell (or run passed command)
ENTRYPOINT ["/entrypoint.sh"]