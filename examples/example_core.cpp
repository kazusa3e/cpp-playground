#include <kuri/core.hpp>
#include <print>

// NOLINTNEXTLINE(bugprone-exception-escape)
auto main() -> int {
    // NOLINTNEXTLINE(readability-magic-numbers)
    std::println("kuri::add(10, 20) = {}", kuri::add(10, 20));
    return 0;
}
