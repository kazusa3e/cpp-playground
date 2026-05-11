#pragma once

#include <stdexcept>

[[noreturn]] inline auto todo(const char *msg = "not implemented yet") -> void {
    throw std::logic_error { msg };
}
