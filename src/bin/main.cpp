#include <format>
#include <print>

namespace {
auto test() -> void { std::println("Hello, {}!", "world"); }
}  // namespace

// NOLINTNEXTLINE(bugprone-exception-escape)
auto main(int /*argc*/, const char * /*argv*/[]) -> int {
    test();
    return 0;
}
