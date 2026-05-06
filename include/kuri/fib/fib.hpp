#pragma once

#include <cstddef>

namespace kuri::fib {

auto fib(unsigned n) -> std::size_t;
auto fib_memo(unsigned n) -> std::size_t;
auto fib_calc(unsigned n) -> std::size_t;

}  // namespace kuri::fib
