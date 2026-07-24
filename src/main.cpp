#include <kuri/core.hpp>
#include <print>

// NOLINTNEXTLINE(bugprone-exception-escape)
auto main() -> int {
    const auto ans = kuri::add(5, 3);
    std::println("5 + 3 = {}", ans);
    return 0;
}
