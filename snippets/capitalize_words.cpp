#include <algorithm>
#include <array>
#include <cassert>
#include <cctype>
#include <cstdint>
#include <string>
#include <string_view>
#include <utility>

class word_capitalizer {
   public:
    auto operator()(char ch) noexcept -> char {
        const auto result = transition(state_, ch);
        state_ = result.next_state;
        return result.output;
    }

   private:
    enum class state : std::uint8_t { between_words, in_word };

    state state_{state::between_words};

    struct transition_result {
        state next_state;
        char output;
    };

    [[nodiscard]] static auto transition(state current_state, char input) noexcept
        -> transition_result {
        switch (current_state) {
            case state::between_words:
                if (std::isspace(static_cast<unsigned char>(input)) != 0) {
                    return {.next_state = state::between_words, .output = input};
                } else {
                    return {
                        .next_state = state::in_word,
                        .output =
                            static_cast<char>(std::toupper(static_cast<unsigned char>(input))),
                    };
                }

            case state::in_word:
                if (std::isspace(static_cast<unsigned char>(input)) != 0) {
                    return {.next_state = state::between_words, .output = input};
                } else {
                    return {.next_state = state::in_word, .output = input};
                }
        }
        std::unreachable();
    }
};

static auto capitalize_words_inplace(std::string& text) -> void {
    auto capitalizer = word_capitalizer{};
    std::ranges::for_each(text, [&capitalizer](char& ch) -> void { ch = capitalizer(ch); });
}

auto main() -> int {
    struct test_case {
        std::string_view input;
        std::string_view expected;
    };

    constexpr std::array test_cases{
        test_case{.input = "", .expected = ""},
        test_case{.input = "hello", .expected = "Hello"},
        test_case{.input = "hello world", .expected = "Hello World"},
        test_case{.input = "   ", .expected = "   "},
        test_case{.input = "  hello\tworld\n", .expected = "  Hello\tWorld\n"},
        test_case{.input = "already Capitalized", .expected = "Already Capitalized"},
    };

    for (const auto& [input, expected] : test_cases) {
        auto actual = std::string{input};
        capitalize_words_inplace(actual);
        assert(actual == expected);
    }

    return 0;
}
