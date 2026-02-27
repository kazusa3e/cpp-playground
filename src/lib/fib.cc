#include <cstddef>
#include <fib/fib.hpp>
#include <vector>

auto fib(unsigned n) -> std::size_t {  // NOLINT(misc-no-recursion)
    if (n == 0 || n == 1) {
        return n;
    }
    return fib(n - 1) + fib(n - 2);
}

auto fib_memo(unsigned int n) -> std::size_t {
    auto memo = std::vector<std::size_t> { 0, 1 };
    while (memo.size() <= n) {
        const auto r1 = memo[memo.size() - 1];
        const auto r2 = memo[memo.size() - 2];
        memo.push_back(r1 + r2);
    }
    return memo[n];
}

auto fib_calc(unsigned int n) -> std::size_t {
    std::size_t a = 0, b = 1;  // NOLINT(readability-isolate-declaration)
    unsigned i = 0;
    while (i++ < n) {
        const auto t = a + b;
        a = b;
        b = t;
    }
    return a;
}
