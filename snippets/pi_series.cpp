#include <algorithm>
#include <cmath>
#include <cstddef>
#include <numeric>
#include <print>
#include <ranges>
#include <vector>

#include "kuri/random.hpp"

[[nodiscard]] auto calculate_pi_by_series(std::size_t term_count) noexcept -> double {
    constexpr auto term = [](decltype(term_count) k) noexcept -> double {
        // NOLINTNEXTLINE(readability-magic-numbers)
        return ((k % 2 == 0) ? 1.0 : -1.0) / (2.0 * static_cast<double>(k) + 1.0);
    };

    const auto terms =
        std::views::iota(decltype(term_count){0}, term_count) | std::views::transform(term);

    // NOLINTNEXTLINE(readability-magic-numbers)
    return 4.0 * std::accumulate(terms.begin(), terms.end(), 0.0);
}

[[nodiscard]] auto calculate_pi_by_rectangles(std::size_t rectangle_count) noexcept -> double {
    if (rectangle_count == 0) return 0.0;
    constexpr auto r = 2;
    const auto w = static_cast<double>(r) / static_cast<double>(rectangle_count);
    auto area = [=](decltype(rectangle_count) i) noexcept -> double {
        const auto xi = w * (static_cast<double>(i) + 0.5);
        const auto h = std::sqrt((r * r) - (xi * xi));
        return h * w;
    };

    const auto areas = std::views::iota(decltype(rectangle_count){0}, rectangle_count)
        | std::views::transform(area);
    return std::accumulate(areas.begin(), areas.end(), 0.0);
}

[[nodiscard]] auto calculate_pi_by_monte_carlo(std::size_t point_count) noexcept -> double {
    if (point_count == 0) return 0.0;

    constexpr auto sampling_square_area = 4.0;
    const auto hits = std::views::iota(decltype(point_count){0}, point_count)
        | std::views::transform([](const auto) noexcept -> bool {
                          const auto x = kuri::random::real(-1.0, 1.0);
                          const auto y = kuri::random::real(-1.0, 1.0);
                          return ((x * x) + (y * y)) <= 1.0;
                      });

    const auto inside_count = std::ranges::count(hits, true);
    return sampling_square_area * static_cast<double>(inside_count)
        / static_cast<double>(point_count);
}

// NOLINTNEXTLINE(bugprone-exception-escape)
auto main() -> int {
    const auto series_counts = std::vector<std::size_t>{1'000, 1'000'000, 10'000'000};
    std::println("Leibniz series method:");
    for (const auto& c : series_counts) {
        std::println("with iteration count {}, pi = {}", c, calculate_pi_by_series(c));
    }

    const auto rect_counts = std::vector<std::size_t>{1'000, 10'000, 100'000};
    std::println("Midpoint rectangle method:");
    for (const auto& c : rect_counts) {
        std::println("with iteration count {}, pi = {}", c, calculate_pi_by_rectangles(c));
    }

    const auto point_counts = std::vector<std::size_t>{1'000, 1'000'000, 10'000'000};
    std::println("Monte Carlo method:");
    for (const auto& c : point_counts) {
        std::println("with iteration count {}, pi = {}", c, calculate_pi_by_monte_carlo(c));
    }
    return 0;
}
