#include <format>
#include <print>

namespace {
auto test() -> void {
    std::println("Hello, {}!", "world");
}
}  // namespace

auto main(int /*argc*/, const char * /*argv*/[]) -> int {  // NOLINT(bugprone-exception-escape)
    test();
    return 0;
}
