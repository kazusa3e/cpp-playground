#include <cstddef>
#include <numeric>
#include <print>
#include <ranges>
#include <vector>

[[nodiscard]] auto calculate_pi(std::size_t term_count) noexcept -> double {
    constexpr auto term = [](decltype(term_count) k) noexcept -> double {
        // NOLINTNEXTLINE(readability-magic-numbers)
        return ((k % 2 == 0) ? 1.0 : -1.0) / (2.0 * static_cast<double>(k) + 1.0);
    };

    const auto terms =
        std::views::iota(decltype(term_count){0}, term_count) | std::views::transform(term);

    // NOLINTNEXTLINE(readability-magic-numbers)
    return 4.0 * std::accumulate(terms.begin(), terms.end(), 0.0);
}

// NOLINTNEXTLINE(bugprone-exception-escape)
auto main() -> int {
    const auto counts = std::vector<std::size_t>{1'000, 1'000'000, 10'000'000};
    for (const auto& c : counts) {
        std::println("with iteration count {}, pi = {}", c, calculate_pi(c));
    }
    return 0;
}
