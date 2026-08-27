#include <print>

#include "kuri/random.hpp"

namespace {
constexpr auto two = 2;
constexpr auto three = 3;
constexpr auto seven = 7;
constexpr auto eleven = 11;
constexpr auto twelve = 12;
}  // namespace

auto try_to_make_point(int point) -> bool;
auto roll_two_dice() -> int;

// NOLINTNEXTLINE(bugprone-exception-escape)
auto main() -> int {
    std::println("This program plays a game of craps.");
    const auto point = roll_two_dice();
    switch (point) {
        case seven:
        case eleven:
            std::println("This'a natural. You win.");
            break;
        case two:
        case three:
        case twelve:
            std::println("That's craps. You lose.");
            break;
        default:
            if (try_to_make_point(point)) {
                std::println("You made your point. You win.");
            } else {
                std::println("You rolled a seven. You lose.");
            }
    }
}

auto try_to_make_point(int point) -> bool {
    while (true) {
        const auto total = roll_two_dice();
        if (total == point) return true;
        if (total == seven) return false;
    }
}

auto roll_two_dice() -> int {
    std::println("Rolling the dice ...");
    const auto d1 = kuri::random::integer(1, 6);
    const auto d2 = kuri::random::integer(1, 6);
    const auto total = d1 + d2;
    std::println("Your rolled {} and {} - that's {}", d1, d2, total);
    return static_cast<int>(total);
}
