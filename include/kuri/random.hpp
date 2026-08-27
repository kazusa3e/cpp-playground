#pragma once

namespace kuri::random {

auto integer(long low, long high) noexcept -> long;
auto real(double low, double high) noexcept -> double;
auto chance(double p) noexcept -> bool;

}  // namespace kuri::random
