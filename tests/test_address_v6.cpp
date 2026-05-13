#include <algorithm>
#include <array>
#include <catch2/catch_test_macros.hpp>
#include <cstdint>
#include <initializer_list>
#include <span>
#include <string_view>
#include "address.hpp"

using kuri::net::address_v6;

// ---- Construction & accessors ----

TEST_CASE("address_v6 default constructs to ::", "[address_v6][ctor]") {
    address_v6 a;
    REQUIRE(a.is_unspecified());
    auto bytes = a.to_bytes();
    REQUIRE(
        std::ranges::all_of(bytes, [](uint8_t b) -> bool { return b == 0; }));
}

TEST_CASE("address_v6 constructs from span<uint8_t,16>", "[address_v6][ctor]") {
    const auto raw =
        std::array<uint8_t, 16> { 0x20, 0x01, 0x48, 0x60, 0x48, 0x60,
                                  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                  0x00, 0x00, 0x88, 0x88 };
    address_v6 a(raw);
    auto expected =
        address_v6::from_string("2001:4860:4860::8888").value().to_bytes();
    REQUIRE(a.to_bytes() == expected);
}

TEST_CASE("address_v6 constructs from in6_addr", "[address_v6][ctor]") {
    auto ia =
        address_v6::from_string("2001:4860:4860::8888").value().to_in6_addr();
    address_v6 a(ia);
    auto bytes = a.to_bytes();
    // NOLINTBEGIN(cppcoreguidelines-pro-bounds-avoid-unchecked-container-access)
    REQUIRE(bytes[0] == 0x20);
    REQUIRE(bytes[1] == 0x01);
    REQUIRE(bytes[2] == 0x48);
    REQUIRE(bytes[3] == 0x60);
    REQUIRE(bytes[14] == 0x88);
    REQUIRE(bytes[15] == 0x88);
    // NOLINTEND(cppcoreguidelines-pro-bounds-avoid-unchecked-container-access)
}

TEST_CASE("address_v6::to_in6_addr round-trips through constructor",
          "[address_v6][roundtrip]") {
    auto a = address_v6::from_string("::1").value();
    auto ia = a.to_in6_addr();
    address_v6 b(ia);
    REQUIRE(a.to_bytes() == b.to_bytes());
}

TEST_CASE("address_v6 to_bytes round-trips through span constructor",
          "[address_v6][roundtrip]") {
    auto a = address_v6::from_string("fe80::1").value();
    auto bytes = a.to_bytes();
    address_v6 b { std::span<const uint8_t, 16> { bytes } };
    REQUIRE(a.to_bytes() == b.to_bytes());
}

// ---- scope_id ----

TEST_CASE("address_v6 stores scope_id from span constructor",
          "[address_v6][scope_id]") {
    const auto raw = std::array<uint8_t, 16> {};
    const std::span s(raw);
    address_v6 a(s, 42);
    REQUIRE(a.is_unspecified());
}

// ---- Predicates: is_unspecified ----

TEST_CASE("address_v6::is_unspecified", "[address_v6][predicate]") {
    REQUIRE(address_v6::from_string("::").value().is_unspecified());
    REQUIRE_FALSE(address_v6::from_string("::1").value().is_unspecified());
    REQUIRE_FALSE(address_v6::from_string("ff00::1").value().is_unspecified());
    REQUIRE_FALSE(address_v6::from_string("fe80::1").value().is_unspecified());
}

// ---- Predicates: is_loopback ----

TEST_CASE("address_v6::is_loopback", "[address_v6][predicate]") {
    REQUIRE(address_v6::from_string("::1").value().is_loopback());
    REQUIRE_FALSE(address_v6::from_string("::").value().is_loopback());
    REQUIRE_FALSE(address_v6::from_string("::2").value().is_loopback());
    REQUIRE_FALSE(address_v6::from_string("::1:0").value().is_loopback());
    REQUIRE_FALSE(address_v6::from_string("ff00::1").value().is_loopback());
}

// ---- Predicates: is_link_local ----

TEST_CASE("address_v6::is_link_local — fe80::/10 boundary",
          "[address_v6][predicate]") {
    REQUIRE(address_v6::from_string("fe80::").value().is_link_local());
    REQUIRE(address_v6::from_string("fe80::1").value().is_link_local());
    REQUIRE(address_v6::from_string("febf:ffff:ffff:ffff:ffff:ffff:ffff:ffff")
                .value()
                .is_link_local());

    REQUIRE_FALSE(address_v6::from_string("fec0::").value().is_link_local());
    REQUIRE_FALSE(
        address_v6::from_string("fe7f:ffff:ffff:ffff:ffff:ffff:ffff:ffff")
            .value()
            .is_link_local());
    REQUIRE_FALSE(address_v6::from_string("ff80::").value().is_link_local());
}

// ---- Predicates: is_unique_local ----

TEST_CASE("address_v6::is_unique_local — fc00::/7 boundary",
          "[address_v6][predicate]") {
    REQUIRE(address_v6::from_string("fc00::").value().is_unique_local());
    REQUIRE(address_v6::from_string("fc00::1").value().is_unique_local());
    REQUIRE(address_v6::from_string("fd00::").value().is_unique_local());
    REQUIRE(address_v6::from_string("fdff:ffff:ffff:ffff:ffff:ffff:ffff:ffff")
                .value()
                .is_unique_local());

    REQUIRE_FALSE(address_v6::from_string("fe00::").value().is_unique_local());
    REQUIRE_FALSE(
        address_v6::from_string("fbff:ffff:ffff:ffff:ffff:ffff:ffff:ffff")
            .value()
            .is_unique_local());
    REQUIRE_FALSE(address_v6::from_string("ff00::").value().is_unique_local());
}

// ---- Predicates: is_multicast ----

TEST_CASE("address_v6::is_multicast", "[address_v6][predicate]") {
    REQUIRE(address_v6::from_string("ff00::").value().is_multicast());
    REQUIRE(address_v6::from_string("ff00::1").value().is_multicast());
    REQUIRE(address_v6::from_string("ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff")
                .value()
                .is_multicast());

    REQUIRE_FALSE(
        address_v6::from_string("feff:ffff:ffff:ffff:ffff:ffff:ffff:ffff")
            .value()
            .is_multicast());
    REQUIRE_FALSE(address_v6::from_string("::").value().is_multicast());
    REQUIRE_FALSE(address_v6::from_string("::1").value().is_multicast());
}

// ---- Predicates: is_documentation ----

TEST_CASE("address_v6::is_documentation — 2001:db8::/32",
          "[address_v6][predicate]") {
    REQUIRE(address_v6::from_string("2001:db8::").value().is_documentation());
    REQUIRE(address_v6::from_string("2001:db8::1").value().is_documentation());
    REQUIRE(address_v6::from_string("2001:db8:ffff:ffff:ffff:ffff:ffff:ffff")
                .value()
                .is_documentation());

    REQUIRE_FALSE(
        address_v6::from_string("2001:db9::").value().is_documentation());
    REQUIRE_FALSE(
        address_v6::from_string("2001:db7:ffff:ffff:ffff:ffff:ffff:ffff")
            .value()
            .is_documentation());
    REQUIRE_FALSE(
        address_v6::from_string("2002:db8::").value().is_documentation());
    REQUIRE_FALSE(address_v6::from_string("2001::").value().is_documentation());
}

// ---- Predicates: is_teredo ----

TEST_CASE("address_v6::is_teredo — 2001::/32", "[address_v6][predicate]") {
    REQUIRE(address_v6::from_string("2001::").value().is_teredo());
    REQUIRE(address_v6::from_string("2001::1").value().is_teredo());
    REQUIRE(address_v6::from_string("2001:0:ffff:ffff:ffff:ffff:ffff:ffff")
                .value()
                .is_teredo());

    REQUIRE_FALSE(address_v6::from_string("2001:1::").value().is_teredo());
    REQUIRE_FALSE(address_v6::from_string("2002::").value().is_teredo());
    REQUIRE_FALSE(address_v6::from_string("2001:db8::").value().is_teredo());
}

// ---- Predicates: is_6to4 ----

TEST_CASE("address_v6::is_6to4 — 2002::/16", "[address_v6][predicate]") {
    REQUIRE(address_v6::from_string("2002::").value().is_6to4());
    REQUIRE(address_v6::from_string("2002::1").value().is_6to4());
    REQUIRE(address_v6::from_string("2002:ffff:ffff:ffff:ffff:ffff:ffff:ffff")
                .value()
                .is_6to4());

    REQUIRE_FALSE(address_v6::from_string("2003::").value().is_6to4());
    REQUIRE_FALSE(
        address_v6::from_string("2001:ffff:ffff:ffff:ffff:ffff:ffff:ffff")
            .value()
            .is_6to4());
    REQUIRE_FALSE(address_v6::from_string("2001::").value().is_6to4());
}

// ---- Predicates: is_global_u ----

TEST_CASE("address_v6::is_global_u — positive cases",
          "[address_v6][predicate]") {
    REQUIRE(
        address_v6::from_string("2001:4860:4860::8888").value().is_global_u());
    REQUIRE(
        address_v6::from_string("2606:4700:4700::1111").value().is_global_u());
    REQUIRE(address_v6::from_string("2000::").value().is_global_u());
    REQUIRE(address_v6::from_string("3fff:ffff:ffff:ffff:ffff:ffff:ffff:ffff")
                .value()
                .is_global_u());
}

TEST_CASE("address_v6::is_global_u — negative cases",
          "[address_v6][predicate]") {
    REQUIRE_FALSE(address_v6::from_string("::").value().is_global_u());
    REQUIRE_FALSE(address_v6::from_string("::1").value().is_global_u());
    REQUIRE_FALSE(address_v6::from_string("fe80::1").value().is_global_u());
    REQUIRE_FALSE(address_v6::from_string("fc00::1").value().is_global_u());
    REQUIRE_FALSE(address_v6::from_string("ff00::1").value().is_global_u());
    REQUIRE_FALSE(address_v6::from_string("2001:db8::1").value().is_global_u());
    REQUIRE_FALSE(address_v6::from_string("2001::1").value().is_global_u());
    REQUIRE_FALSE(address_v6::from_string("2002::1").value().is_global_u());
    REQUIRE_FALSE(address_v6::from_string("4000::").value().is_global_u());
}

// ---- Predicates: mutual exclusivity ----

TEST_CASE("address_v6 predicates are mutually exclusive",
          "[address_v6][predicate]") {
    auto unspecified = address_v6::from_string("::").value();
    REQUIRE(unspecified.is_unspecified());
    REQUIRE_FALSE(unspecified.is_loopback());
    REQUIRE_FALSE(unspecified.is_link_local());
    REQUIRE_FALSE(unspecified.is_unique_local());
    REQUIRE_FALSE(unspecified.is_multicast());
    REQUIRE_FALSE(unspecified.is_documentation());
    REQUIRE_FALSE(unspecified.is_teredo());
    REQUIRE_FALSE(unspecified.is_6to4());
    REQUIRE_FALSE(unspecified.is_global_u());

    auto loopback = address_v6::from_string("::1").value();
    REQUIRE(loopback.is_loopback());
    REQUIRE_FALSE(loopback.is_unspecified());
    REQUIRE_FALSE(loopback.is_link_local());
    REQUIRE_FALSE(loopback.is_unique_local());
    REQUIRE_FALSE(loopback.is_multicast());
    REQUIRE_FALSE(loopback.is_documentation());
    REQUIRE_FALSE(loopback.is_teredo());
    REQUIRE_FALSE(loopback.is_6to4());
    REQUIRE_FALSE(loopback.is_global_u());

    auto link_local = address_v6::from_string("fe80::1").value();
    REQUIRE(link_local.is_link_local());
    REQUIRE_FALSE(link_local.is_unspecified());
    REQUIRE_FALSE(link_local.is_loopback());
    REQUIRE_FALSE(link_local.is_unique_local());
    REQUIRE_FALSE(link_local.is_multicast());
    REQUIRE_FALSE(link_local.is_global_u());

    auto unique_local = address_v6::from_string("fd00::1").value();
    REQUIRE(unique_local.is_unique_local());
    REQUIRE_FALSE(unique_local.is_unspecified());
    REQUIRE_FALSE(unique_local.is_loopback());
    REQUIRE_FALSE(unique_local.is_link_local());
    REQUIRE_FALSE(unique_local.is_multicast());
    REQUIRE_FALSE(unique_local.is_global_u());

    auto multicast = address_v6::from_string("ff00::1").value();
    REQUIRE(multicast.is_multicast());
    REQUIRE_FALSE(multicast.is_unspecified());
    REQUIRE_FALSE(multicast.is_loopback());
    REQUIRE_FALSE(multicast.is_link_local());
    REQUIRE_FALSE(multicast.is_unique_local());
    REQUIRE_FALSE(multicast.is_global_u());

    auto doc = address_v6::from_string("2001:db8::1").value();
    REQUIRE(doc.is_documentation());
    REQUIRE_FALSE(doc.is_unspecified());
    REQUIRE_FALSE(doc.is_loopback());
    REQUIRE_FALSE(doc.is_link_local());
    REQUIRE_FALSE(doc.is_multicast());
    REQUIRE_FALSE(doc.is_6to4());
    REQUIRE_FALSE(doc.is_global_u());

    auto teredo = address_v6::from_string("2001::1").value();
    REQUIRE(teredo.is_teredo());
    REQUIRE_FALSE(teredo.is_unspecified());
    REQUIRE_FALSE(teredo.is_loopback());
    REQUIRE_FALSE(teredo.is_multicast());
    REQUIRE_FALSE(teredo.is_6to4());
    REQUIRE_FALSE(teredo.is_global_u());

    auto six_to_four = address_v6::from_string("2002::1").value();
    REQUIRE(six_to_four.is_6to4());
    REQUIRE_FALSE(six_to_four.is_unspecified());
    REQUIRE_FALSE(six_to_four.is_loopback());
    REQUIRE_FALSE(six_to_four.is_multicast());
    REQUIRE_FALSE(six_to_four.is_teredo());
    REQUIRE_FALSE(six_to_four.is_global_u());

    auto global = address_v6::from_string("2001:4860:4860::8888").value();
    REQUIRE(global.is_global_u());
    REQUIRE_FALSE(global.is_unspecified());
    REQUIRE_FALSE(global.is_loopback());
    REQUIRE_FALSE(global.is_link_local());
    REQUIRE_FALSE(global.is_unique_local());
    REQUIRE_FALSE(global.is_multicast());
    REQUIRE_FALSE(global.is_documentation());
    REQUIRE_FALSE(global.is_teredo());
    REQUIRE_FALSE(global.is_6to4());
}

// ---- from_string ----

TEST_CASE("address_v6::from_string parses valid addresses",
          "[address_v6][from_string]") {
    struct m_case {
        std::string_view input;
        bool is_unspec;
        bool is_loopback;
        bool is_link_local;
    };
    auto cases = {
        m_case { .input = "::",
                 .is_unspec = true,
                 .is_loopback = false,
                 .is_link_local = false },
        m_case { .input = "::1",
                 .is_unspec = false,
                 .is_loopback = true,
                 .is_link_local = false },
        m_case { .input = "fe80::1",
                 .is_unspec = false,
                 .is_loopback = false,
                 .is_link_local = true },
        m_case { .input = "2001:db8::1",
                 .is_unspec = false,
                 .is_loopback = false,
                 .is_link_local = false },
    };
    for (auto [input, unspec, loop, ll] : cases) {
        auto r = address_v6::from_string(input);
        REQUIRE(r.has_value());
        REQUIRE(r->is_unspecified() == unspec);
        REQUIRE(r->is_loopback() == loop);
        REQUIRE(r->is_link_local() == ll);
    }
}

TEST_CASE("address_v6::from_string handles abbreviated formats",
          "[address_v6][from_string]") {
    REQUIRE(address_v6::from_string("::1").has_value());
    REQUIRE(address_v6::from_string("2001::").has_value());
    REQUIRE(address_v6::from_string("2001:db8::1").has_value());
    REQUIRE(address_v6::from_string("2001:0db8:0000:0000:0000:ff00:0042:8329")
                .has_value());
    REQUIRE(address_v6::from_string("2001:DB8::1").has_value());
}

TEST_CASE("address_v6::from_string parses IPv4-mapped IPv6",
          "[address_v6][from_string]") {
    auto r = address_v6::from_string("::ffff:192.168.1.1");
    REQUIRE(r.has_value());
    // ::ffff:a.b.c.d layout: bytes 10-11 are 0xff, bytes 12-15 are a.b.c.d
    auto bytes = r->to_bytes();
    // NOLINTBEGIN(cppcoreguidelines-pro-bounds-avoid-unchecked-container-access)
    REQUIRE(bytes[10] == 0xffu);
    REQUIRE(bytes[11] == 0xffu);
    REQUIRE(bytes[12] == 192);
    REQUIRE(bytes[13] == 168);
    REQUIRE(bytes[14] == 1);
    REQUIRE(bytes[15] == 1);
    // NOLINTEND(cppcoreguidelines-pro-bounds-avoid-unchecked-container-access)
}

TEST_CASE("address_v6::from_string with scope ID — numeric",
          "[address_v6][from_string][scope]") {
    auto r = address_v6::from_string("::1%1");
    REQUIRE(r.has_value());
    REQUIRE(r->is_loopback());
}

TEST_CASE("address_v6::from_string with scope ID — named interface",
          "[address_v6][from_string][scope]") {
    auto r = address_v6::from_string("fe80::1%lo");
    REQUIRE(r.has_value());
    REQUIRE(r->is_link_local());
}

TEST_CASE("address_v6::from_string with scope ID — numeric round-trip",
          "[address_v6][from_string][scope][roundtrip]") {
    auto r = address_v6::from_string("::1%42");
    REQUIRE(r.has_value());
    REQUIRE(r->scope_id() == 42);
    REQUIRE(r->is_loopback());
}

TEST_CASE("address_v6::from_string with scope ID — named round-trip",
          "[address_v6][from_string][scope][roundtrip]") {
    auto r = address_v6::from_string("fe80::1%lo");
    REQUIRE(r.has_value());
    auto s = r->to_string();
    REQUIRE(s.contains("%lo"));
}

TEST_CASE("address_v6::from_string rejects invalid formats",
          "[address_v6][from_string]") {
    auto bad = std::initializer_list<std::string_view> {
        {},
        "",
        "not an address",
        "127.0.0.1",
        "fe80::1%%eth0",
        "fe80::1%",
        "::1:2:3:4:5:6:7:8",  // too many segments
        "gggg::1",            // invalid hex
        "2001:db8::1::2",     // double ::
        "fe80:",
        " fe80::1",  // leading space
        "fe80::1 ",  // trailing space
    };
    for (auto input : bad) {
        auto r = address_v6::from_string(input);
        REQUIRE(r.has_value() == false);
        REQUIRE(r.error() == kuri::net::address_errc::invalid_format);
    }
}

TEST_CASE("address_v6::from_string rejects nonexistent scope name",
          "[address_v6][from_string][scope]") {
    auto r = address_v6::from_string("fe80::1%nonexistent_iface_xyz");
    REQUIRE(r.has_value() == false);
    REQUIRE(r.error() == kuri::net::address_errc::invalid_format);
}

// ---- to_string ----

TEST_CASE("address_v6::to_string round-trips through from_string",
          "[address_v6][to_string][roundtrip]") {
    auto inputs = std::initializer_list<std::string_view> {
        "::", "::1", "2001:db8::1", "fe80::1", "ff00::1", "fc00::1",
    };
    for (auto input : inputs) {
        auto r = address_v6::from_string(input);
        REQUIRE(r.has_value());
        auto r2 = address_v6::from_string(r->to_string());
        REQUIRE(r2.has_value());
        REQUIRE(r->to_bytes() == r2->to_bytes());
    }
}

TEST_CASE("address_v6 from_string returns address with scope_id=0 by default",
          "[address_v6][from_string]") {
    auto r = address_v6::from_string("::1");
    REQUIRE(r.has_value());
    REQUIRE(!r->to_string().contains('%'));
}
