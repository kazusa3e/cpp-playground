#include "address.hpp"
#include <arpa/inet.h>
#include <net/if.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <algorithm>
#include <array>
#include <charconv>
#include <cstddef>
#include <cstdint>
#include <expected>
#include <string>
#include <string_view>
#include <system_error>

namespace kuri::net {

namespace {
class address_category_impl : public std::error_category {
public:
    [[nodiscard]] auto name() const noexcept -> const char * override {
        return "kuri::address";
    }

    [[nodiscard]] auto message(int ev) const -> std::string override {
        switch (static_cast<kuri::net::address_errc>(ev)) {
            case kuri::net::address_errc::invalid_format:
                return "Invalid IP address format";
        }
    }
};

}  // namespace

auto address_v4::from_string(std::string_view s) noexcept
    -> std::expected<address_v4, std::error_code> {
    auto buf = std::array<char, INET_ADDRSTRLEN> {};
    const auto len = std::min(s.size(), buf.size() - 1);
    std::ranges::copy_n(s.begin(), static_cast<std::ptrdiff_t>(len),
                        buf.begin());
    *(buf.begin() + len) = '\0';

    auto addr4 = in_addr {};
    if (const auto rc = inet_pton(AF_INET, buf.data(), &addr4); rc == 1) {
        return address_v4 { addr4 };
    }
    return std::unexpected(make_error_code(address_errc::invalid_format));
}

auto address_v4::to_string() const -> std::string {
    std::array<char, INET_ADDRSTRLEN> buf {};
    const auto addr = to_in_addr();
    return inet_ntop(AF_INET, &addr, buf.data(), sizeof(buf));
}

auto address_v6::from_string(std::string_view s) noexcept
    -> std::expected<address_v6, std::error_code> {
    const auto pos = s.find('%');
    const auto addr_sv = s.substr(0, pos);

    auto addr_buf = std::array<char, INET6_ADDRSTRLEN> {};
    const auto addr_len = std::min(addr_sv.size(), addr_buf.size() - 1);
    std::ranges::copy_n(addr_sv.begin(), static_cast<std::ptrdiff_t>(addr_len),
                        addr_buf.begin());
    *(addr_buf.begin() + addr_len) = '\0';

    auto addr = in6_addr {};
    if (const auto rc = inet_pton(AF_INET6, addr_buf.data(), &addr); rc != 1) {
        return std::unexpected(make_error_code(address_errc::invalid_format));
    }

    if (pos == std::string_view::npos) {
        return address_v6 { addr };
    }

    const auto scope_sv = s.substr(pos + 1);
    auto scope_buf = std::array<char, IF_NAMESIZE> {};
    const auto scope_len = std::min(scope_sv.size(), scope_buf.size() - 1);
    std::ranges::copy_n(scope_sv.begin(),
                        static_cast<std::ptrdiff_t>(scope_len),
                        scope_buf.begin());
    *(scope_buf.begin() + scope_len) = '\0';

    auto scope_id = uint32_t {};
    const auto [ptr, ec] = std::from_chars(
        scope_buf.data(), scope_buf.data() + scope_len, scope_id);

    if (ec == std::errc {} && ptr == scope_buf.data() + scope_len) {
        return address_v6 { addr, scope_id };
    }

    if (const auto idx = if_nametoindex(scope_buf.data()); idx != 0) {
        return address_v6 { addr, idx };
    }
    return std::unexpected(make_error_code(address_errc::invalid_format));
}

auto address_v6::to_string() const -> std::string {
    auto buf = std::array<char, INET6_ADDRSTRLEN> {};
    const auto in6_addr = to_in6_addr();
    inet_ntop(AF_INET6, &in6_addr, buf.data(), buf.size());
    auto ret = std::string { buf.data() };

    if (impl.scope_id != 0) {
        buf.fill('\0');
        const auto *scope_s = if_indextoname(impl.scope_id, buf.data());
        if (scope_s != nullptr) {
            ret.push_back('%');
            ret += scope_s;
        }
    }
    return ret;
}

auto address_category() noexcept -> const std::error_category & {
    static const auto instance = address_category_impl {};
    return instance;
}

auto make_error_code(address_errc e) noexcept -> std::error_code {
    return { static_cast<int>(e), address_category() };
}

}  // namespace kuri::net
