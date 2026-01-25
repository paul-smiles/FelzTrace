#!/usr/bin/env bash
set -e

# Default values
BUILD_RELEASE=false
BUILD_DEBUG=false
BUILD_COVERAGE=false
RUN_TESTS=true

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        -r|--release)
            BUILD_RELEASE=true
            shift
            ;;
        -d|--debug)
            BUILD_DEBUG=true
            shift
            ;;
        -c|--coverage)
            BUILD_COVERAGE=true
            shift
            ;;
        --no-tests)
            RUN_TESTS=false
            shift
            ;;
        -a|--all)
            BUILD_RELEASE=true
            BUILD_DEBUG=true
            BUILD_COVERAGE=true
            shift
            ;;
        -h|--help)
            echo "Usage: $0 [OPTIONS]"
            echo ""
            echo "Options:"
            echo "  -r, --release     Build Release version"
            echo "  -d, --debug       Build Debug version"
            echo "  -c, --coverage    Build with coverage"
            echo "  -a, --all         Build all versions (default if no flags given)"
            echo "  --no-tests        Skip running tests"
            echo "  -h, --help        Show this help message"
            exit 0
            ;;
        *)
            echo "Unknown option: $1"
            echo "Use -h or --help for usage information"
            exit 1
            ;;
    esac
done

# If no build type specified, build all
if [[ "$BUILD_RELEASE" == false && "$BUILD_DEBUG" == false && "$BUILD_COVERAGE" == false ]]; then
    BUILD_RELEASE=true
    BUILD_DEBUG=true
    BUILD_COVERAGE=true
fi

# Build Release
if [[ "$BUILD_RELEASE" == true ]]; then
    echo "Building Release..."
    cmake -S . -B build-release -DCMAKE_BUILD_TYPE=Release
    cmake --build build-release -- -j$(nproc)

    if [[ "$RUN_TESTS" == true ]]; then
        echo "Running tests..."
        ctest --test-dir build-release --output-on-failure
    fi
fi

# Build Debug
if [[ "$BUILD_DEBUG" == true ]]; then
    echo "Building Debug..."
    cmake -S . -B build-debug -DCMAKE_BUILD_TYPE=Debug
    cmake --build build-debug -- -j$(nproc)
    
    if [[ "$RUN_TESTS" == true ]]; then
        echo "Running tests..."
        ctest --test-dir build-debug --output-on-failure --verbose
    fi
fi

# Build Coverage
if [[ "$BUILD_COVERAGE" == true ]]; then
    echo "Building with coverage..."
    cmake -S . -B build-cov -DCMAKE_BUILD_TYPE=Debug -DENABLE_COVERAGE=ON
    cmake --build build-cov -- -j$(nproc)
    
    if [[ "$RUN_TESTS" == true ]]; then
        cmake --build build-cov --target coverage
    fi
fi