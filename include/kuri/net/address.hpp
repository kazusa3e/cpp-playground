#pragma once

#include <netinet/in.h>
#include <algorithm>
#include <array>
#include <compare>
#include <cstdint>
#include <expected>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <system_error>
#include <type_traits>
#include <variant>

namespace kuri::net {

namespace internal {

struct address_v4_impl {
    // NOLINTNEXTLINE(misc-non-private-member-variables-in-classes)
    uint32_t addr {};  // host byte order

    auto operator<=>(const address_v4_impl &rhs) const noexcept
        -> std::strong_ordering = default;
};

struct address_v6_impl {
    // NOLINTNEXTLINE(misc-non-private-member-variables-in-classes)
    std::array<uint8_t, 16> addr {};
    // NOLINTNEXTLINE(misc-non-private-member-variables-in-classes)
    uint32_t scope_id {};

    auto operator<=>(const address_v6_impl &rhs) const noexcept
        -> std::strong_ordering = default;
};

}  // namespace internal

enum class address_errc : uint8_t {
    invalid_format = 0,
};

auto address_category() noexcept -> const std::error_category &;

auto make_error_code(address_errc e) noexcept -> std::error_code;

class address_v4 {
public:
    address_v4() = default;

    // host byte order
    explicit address_v4(uint32_t addr) : impl { .addr = addr } {}
    // host byte order
    [[nodiscard]] auto to_uint32() const noexcept -> uint32_t {
        return impl.addr;
    }

    explicit address_v4(in_addr addr) : impl { .addr = ntohl(addr.s_addr) } {}
    [[nodiscard]] auto to_in_addr() const noexcept -> in_addr {
        return in_addr {
            .s_addr = htonl(impl.addr),
        };
    }

    static auto from_string(std::string_view s) noexcept
        -> std::expected<address_v4, std::error_code>;
    [[nodiscard]] auto to_string() const -> std::string;

    auto operator<=>(const address_v4 &rhs) const noexcept
        -> std::strong_ordering = default;

    // 0.0.0.0/32
    [[nodiscard]] auto is_unspecified() const noexcept -> bool {
        return impl.addr == 0;
    }

    // 127.0.0.0/8
    [[nodiscard]] auto is_loopback() const noexcept -> bool {
        return (impl.addr & 0xff000000u) == 0x7f000000u;
    }

    // 169.254.0.0/16
    [[nodiscard]] auto is_link_local() const noexcept -> bool {
        return (impl.addr & 0xffff0000u) == 0xa9fe0000u;
    }

    // 10.0.0.0/8, 172.16.0.0/12, 192.168.0.0/16
    [[nodiscard]] auto is_private() const noexcept -> bool {
        return (impl.addr & 0xff000000u) == 0x0a000000u ||
               (impl.addr & 0xfff00000u) == 0xac100000u ||
               (impl.addr & 0xffff0000u) == 0xc0a80000u;
    }

    // 224.0.0.0/4
    [[nodiscard]] auto is_multicast() const noexcept -> bool {
        return (impl.addr & 0xf0000000u) == 0xe0000000u;
    }

    // 255.255.255.255/32
    [[nodiscard]] auto is_broadcast() const noexcept -> bool {
        return impl.addr == 0xffffffffu;
    }

    static auto loopback() noexcept -> address_v4 {
        return address_v4 { 0x7f000001u };
    }

    static auto unspecified() noexcept -> address_v4 { return address_v4 {}; }

    static auto broadcast() noexcept -> address_v4 {
        return address_v4 { 0xffffffffu };
    }

private:
    internal::address_v4_impl impl;
};

class address_v6 {
public:
    using bytes_type = std::array<uint8_t, 16>;

    address_v6() = default;

    explicit address_v6(std::span<const uint8_t, 16> addr,
                        uint32_t scope_id = 0)
        : impl { .addr = {}, .scope_id = scope_id } {
        std::ranges::copy(addr, impl.addr.begin());
    }
    [[nodiscard]] auto to_bytes() const noexcept -> bytes_type {
        return impl.addr;
    }

    explicit address_v6(in6_addr addr, uint32_t scope_id = 0)
        : impl { .addr = {}, .scope_id = scope_id } {
        std::copy(static_cast<uint8_t *>(addr.s6_addr),
                  static_cast<uint8_t *>(addr.s6_addr) + 16, impl.addr.begin());
    }
    [[nodiscard]] auto to_in6_addr() const noexcept -> in6_addr {
        auto ret = in6_addr {};
        std::ranges::copy(impl.addr, static_cast<uint8_t *>(ret.s6_addr));
        return ret;
    }

    static auto from_string(std::string_view s) noexcept
        -> std::expected<address_v6, std::error_code>;
    [[nodiscard]] auto to_string() const -> std::string;

    auto operator<=>(const address_v6 &rhs) const noexcept
        -> std::strong_ordering = default;

    [[nodiscard]] auto scope_id() const noexcept -> uint32_t {
        return impl.scope_id;
    }

    static auto loopback() noexcept -> address_v6 {
        auto a = address_v6 {};
        // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-avoid-unchecked-container-access)
        a.impl.addr[15] = 1;
        return a;
    }

    static auto unspecified() noexcept -> address_v6 { return address_v6 {}; }

    // ::/128
    [[nodiscard]] auto is_unspecified() const noexcept -> bool {
        return std::ranges::all_of(impl.addr,
                                   [](uint8_t b) -> bool { return b == 0; });
    }

    // ::1/128
    [[nodiscard]] auto is_loopback() const noexcept -> bool {
        // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-avoid-unchecked-container-access)
        return impl.addr[15] == 1 &&
               std::all_of(impl.addr.begin(), impl.addr.begin() + 15,
                           [](uint8_t b) -> bool { return b == 0; });
    }

    // fe80::/10
    [[nodiscard]] auto is_link_local() const noexcept -> bool {
        // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-avoid-unchecked-container-access)
        return impl.addr[0] == 0xfeu && (impl.addr[1] & 0xc0u) == 0x80u;
    }

    // fc00::/7
    [[nodiscard]] auto is_unique_local() const noexcept -> bool {
        // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-avoid-unchecked-container-access)
        return (impl.addr[0] & 0xfeu) == 0xfcu;
    }

    // ff00::/8
    [[nodiscard]] auto is_multicast() const noexcept -> bool {
        // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-avoid-unchecked-container-access)
        return impl.addr[0] == 0xffu;
    }

    // 2001:db8::/32
    [[nodiscard]] auto is_documentation() const noexcept -> bool {
        // NOLINTBEGIN(cppcoreguidelines-pro-bounds-avoid-unchecked-container-access)
        return impl.addr[0] == 0x20u && impl.addr[1] == 0x01u &&
               impl.addr[2] == 0x0du && impl.addr[3] == 0xb8u;
        // NOLINTEND(cppcoreguidelines-pro-bounds-avoid-unchecked-container-access)
    }

    // 2001::/32
    [[nodiscard]] auto is_teredo() const noexcept -> bool {
        // NOLINTBEGIN(cppcoreguidelines-pro-bounds-avoid-unchecked-container-access)
        return impl.addr[0] == 0x20u && impl.addr[1] == 0x01u &&
               impl.addr[2] == 0x00u && impl.addr[3] == 0x00u;
        // NOLINTEND(cppcoreguidelines-pro-bounds-avoid-unchecked-container-access)
    }

    // 2002::/16
    [[nodiscard]] auto is_6to4() const noexcept -> bool {
        // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-avoid-unchecked-container-access)
        return impl.addr[0] == 0x20u && impl.addr[1] == 0x02u;
    }

    // global unicast (2000::/3, excluding special-purpose prefixes)
    [[nodiscard]] auto is_global_u() const noexcept -> bool {
        if (is_unspecified() || is_loopback() || is_multicast() ||
            is_link_local() || is_unique_local() || is_documentation() ||
            is_teredo() || is_6to4()) {
            return false;
        }
        // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-avoid-unchecked-container-access)
        return (impl.addr[0] & 0xe0u) == 0x20u;
    }

private:
    internal::address_v6_impl impl {};
};

class address {
public:
    // NOLINTNEXTLINE(google-explicit-constructor,hicpp-explicit-conversions)
    /* explicit */ address(address_v4 v4) : addr(v4) {}
    // NOLINTNEXTLINE(google-explicit-constructor,hicpp-explicit-conversions)
    /* explicit */ address(address_v6 v6) : addr(v6) {}

    static auto from_string(std::string_view s) noexcept
        -> std::expected<address, std::error_code> {
        if (const auto v4 = address_v4::from_string(s)) {
            return address { *v4 };
        }
        if (const auto v6 = address_v6::from_string(s)) {
            return address { *v6 };
        }
        return std::unexpected(make_error_code(address_errc::invalid_format));
    }

    [[nodiscard]] auto to_string() const -> std::string {
        return std::visit(
            [](const auto &addr) -> std::string { return addr.to_string(); },
            addr);
    }

    [[nodiscard]] auto is_v4() const noexcept -> bool {
        return std::holds_alternative<address_v4>(addr);
    }

    [[nodiscard]] auto is_v6() const noexcept -> bool {
        return std::holds_alternative<address_v6>(addr);
    }

    [[nodiscard]] auto as_v4() const -> std::optional<address_v4> {
        if (const auto *p = std::get_if<address_v4>(&addr)) {
            return *p;
        }
        return std::nullopt;
    }
    [[nodiscard]] auto as_v6() const -> std::optional<address_v6> {
        if (const auto *p = std::get_if<address_v6>(&addr)) {
            return *p;
        }
        return std::nullopt;
    }

    // NOLINTBEGIN(bugprone-exception-escape)
    auto operator==(const address &rhs) const noexcept -> bool = default;

    [[nodiscard]] auto is_unspecified() const noexcept -> bool {
        return std::visit(
            [](const auto &addr) -> bool { return addr.is_unspecified(); },
            addr);
    }
    [[nodiscard]] auto is_loopback() const noexcept -> bool {
        return std::visit(
            [](const auto &addr) -> bool { return addr.is_loopback(); }, addr);
    }
    [[nodiscard]] auto is_link_local() const noexcept -> bool {
        return std::visit(
            [](const auto &addr) -> bool { return addr.is_link_local(); },
            addr);
    }
    [[nodiscard]] auto is_multicast() const noexcept -> bool {
        return std::visit(
            [](const auto &addr) -> bool { return addr.is_multicast(); }, addr);
    }
    // NOLINTEND(bugprone-exception-escape)

private:
    std::variant<address_v4, address_v6> addr;
};

}  // namespace kuri::net

template <>
struct std::is_error_code_enum<kuri::net::address_errc> : std::true_type {};
