#include "kuri/random.hpp"

#include <cmath>
#include <cstdlib>
#include <ctime>

namespace kuri::random {

// std::rand is retained for this non-cryptographic example utility.
// NOLINTBEGIN(cert-msc30-c,cert-msc32-c,cert-msc50-cpp,cert-msc51-cpp)
auto ensure_random_seed() -> void {
    static bool initialized = false;
    if (!initialized) {
        srand(static_cast<int>(time(nullptr)));
        initialized = true;
    }
}

auto integer(long low, long high) noexcept -> long {
    ensure_random_seed();
    const auto d = rand() / (static_cast<double>(RAND_MAX) + 1.0);
    const auto s = d * (static_cast<double>(high - low) + 1.0);
    return long(std::floor(s) + static_cast<double>(low));
}

auto real(double low, double high) noexcept -> double {
    ensure_random_seed();
    const auto d = rand() / (static_cast<double>(RAND_MAX) + 1.0);
    const auto s = d * (high - low);
    return low + s;
}

// NOLINTEND(cert-msc30-c,cert-msc32-c,cert-msc50-cpp,cert-msc51-cpp)

auto chance(double p) noexcept -> bool { return real(0, 1) < p; }

}  // namespace kuri::random
