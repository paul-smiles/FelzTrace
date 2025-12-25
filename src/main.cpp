#include <spdlog/spdlog.h>

int main() {
    // Set global log level to info
    spdlog::set_level(spdlog::level::info);
    spdlog::info("FelzTrace: This is an info message!");
    return 0;
}