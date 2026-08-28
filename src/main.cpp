#include <kuri/core.hpp>
#include <numeric>
#include <print>
#include <ranges>

#include "kuri/random.hpp"

// NOLINTNEXTLINE(bugprone-exception-escape)
auto main() -> int {
    constexpr auto n = 100'000;

    const auto res = std::views::iota(0, n)
        | std::views::transform([](const auto) -> double { return kuri::random::real(0, 1); });

    const auto avg = std::accumulate(res.begin(), res.end(), 0.0) / static_cast<double>(n);

    std::println("avg is: {}", avg);

    return 0;
}
