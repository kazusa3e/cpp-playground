#include <catch2/catch_test_macros.hpp>
#include <expected>
#include <initializer_list>
#include <string_view>
#include "address.hpp"

using kuri::net::address;
using kuri::net::address_errc;
using kuri::net::address_v4;
using kuri::net::address_v6;

// ---- Construction ----

TEST_CASE("address implicitly constructs from address_v4", "[address][ctor]") {
    address a = address_v4::loopback();  // NOLINT
    REQUIRE(a.is_loopback());
    REQUIRE(a.is_v4());
    REQUIRE(a.to_string() == "127.0.0.1");
}

TEST_CASE("address implicitly constructs from address_v6", "[address][ctor]") {
    address a = address_v6::loopback();  // NOLINT
    REQUIRE(a.is_loopback());
    REQUIRE(a.is_v6());
    REQUIRE(a.to_string() == "::1");
}

// ---- from_string ----

TEST_CASE("address::from_string parses v4 addresses",
          "[address][from_string]") {
    auto cases = {
        std::string_view { "0.0.0.0" },
        std::string_view { "127.0.0.1" },
        std::string_view { "192.168.1.1" },
        std::string_view { "255.255.255.255" },
    };
    for (auto input : cases) {
        auto r = address::from_string(input);
        REQUIRE(r.has_value());
        REQUIRE(r->is_v4());
    }
}

TEST_CASE("address::from_string parses v6 addresses",
          "[address][from_string]") {
    auto cases = {
        std::string_view { "::" },
        std::string_view { "::1" },
        std::string_view { "fe80::1" },
        std::string_view { "2001:db8::1" },
    };
    for (auto input : cases) {
        auto r = address::from_string(input);
        REQUIRE(r.has_value());
        REQUIRE(r->is_v6());
    }
}

TEST_CASE("address::from_string rejects invalid input",
          "[address][from_string]") {
    auto bad = std::initializer_list<std::string_view> {
        {}, "", "not an address", "256.256.256.256", "fe80::1%",
    };
    for (auto input : bad) {
        auto r = address::from_string(input);
        REQUIRE_FALSE(r.has_value());
        REQUIRE(r.error() == address_errc::invalid_format);
    }
}

// ---- to_string roundtrip ----

TEST_CASE("address::to_string roundtrips for v4", "[address][to_string]") {
    auto inputs = std::initializer_list<std::string_view> {
        "0.0.0.0",
        "127.0.0.1",
        "8.8.8.8",
        "255.255.255.255",
    };
    for (auto input : inputs) {
        auto r = address::from_string(input);
        REQUIRE(r.has_value());
        REQUIRE(r->to_string() == input);
    }
}

TEST_CASE("address::to_string roundtrips through from_string for v6",
          "[address][to_string]") {
    auto inputs = std::initializer_list<std::string_view> {
        "::",
        "::1",
        "fe80::1",
        "2001:db8::1",
    };
    for (auto input : inputs) {
        auto r = address::from_string(input);
        REQUIRE(r.has_value());
        auto r2 = address::from_string(r->to_string());
        REQUIRE(r2.has_value());
        REQUIRE(*r == *r2);
    }
}

// ---- is_v4 / is_v6 / as_v4 / as_v6 ----

TEST_CASE("address::is_v4 and is_v6 are mutually exclusive",
          "[address][type_detect]") {
    address v4 = address_v4 {};
    REQUIRE(v4.is_v4());
    REQUIRE_FALSE(v4.is_v6());

    address v6 = address_v6 {};
    REQUIRE(v6.is_v6());
    REQUIRE_FALSE(v6.is_v4());
}

TEST_CASE("address::as_v4 returns value when v4", "[address][as]") {
    address a = address_v4(0x7f000001u);
    auto v = a.as_v4();
    REQUIRE(v.has_value());
    // NOLINTNEXTLINE(bugprone-unchecked-optional-access)
    REQUIRE(v->to_uint32() == 0x7f000001u);
}

TEST_CASE("address::as_v4 returns nullopt when v6", "[address][as]") {
    address a = address_v6::loopback();
    REQUIRE_FALSE(a.as_v4().has_value());
}

TEST_CASE("address::as_v6 returns value when v6", "[address][as]") {
    address a = address_v6::loopback();
    auto v = a.as_v6();
    REQUIRE(v.has_value());
    // NOLINTNEXTLINE(bugprone-unchecked-optional-access)
    REQUIRE(v->is_loopback());
}

TEST_CASE("address::as_v6 returns nullopt when v4", "[address][as]") {
    address a = address_v4::loopback();
    REQUIRE_FALSE(a.as_v6().has_value());
}

// ---- operator== ----

TEST_CASE("address equality — same v4", "[address][compare]") {
    address a = address_v4(0x7f000001u);
    address b = address_v4(0x7f000001u);
    REQUIRE(a == b);
    REQUIRE_FALSE(a != b);
}

TEST_CASE("address equality — different v4", "[address][compare]") {
    address a = address_v4(0x7f000001u);
    address b = address_v4(0x08080808u);
    REQUIRE_FALSE(a == b);
}

TEST_CASE("address equality — v4 vs v6 never equal", "[address][compare]") {
    address v4 = address_v4::unspecified();  // 0.0.0.0
    address v6 = address_v6::unspecified();  // ::
    // both are unspecified, but different protocol versions
    REQUIRE_FALSE(v4 == v6);
}

TEST_CASE("address equality — same v6", "[address][compare]") {
    address a = address_v6::loopback();
    address b = address_v6::loopback();
    REQUIRE(a == b);
}

TEST_CASE("address equality — different v6", "[address][compare]") {
    address a = address_v6::loopback();
    address b = address_v6::unspecified();
    REQUIRE_FALSE(a == b);
}

// ---- Predicates ----

TEST_CASE("address::is_unspecified — v4 true, v6 true",
          "[address][predicate]") {
    REQUIRE(address(address_v4::unspecified()).is_unspecified());
    REQUIRE(address(address_v6::unspecified()).is_unspecified());
    REQUIRE_FALSE(address(address_v4::loopback()).is_unspecified());
    REQUIRE_FALSE(address(address_v6::loopback()).is_unspecified());
}

TEST_CASE("address::is_loopback — v4 true, v6 true", "[address][predicate]") {
    REQUIRE(address(address_v4::loopback()).is_loopback());
    REQUIRE(address(address_v6::loopback()).is_loopback());
    REQUIRE_FALSE(address(address_v4::unspecified()).is_loopback());
    REQUIRE_FALSE(address(address_v6::unspecified()).is_loopback());
}

TEST_CASE("address::is_link_local — v4 true, v6 true", "[address][predicate]") {
    REQUIRE(address(address_v4(0xa9fe0101u)).is_link_local());
    REQUIRE(address(*address_v6::from_string("fe80::1")).is_link_local());
    REQUIRE_FALSE(address(address_v4::loopback()).is_link_local());
    REQUIRE_FALSE(address(address_v6::loopback()).is_link_local());
}

TEST_CASE("address::is_multicast — v4 true, v6 true", "[address][predicate]") {
    REQUIRE(address(address_v4(0xe0000005u)).is_multicast());
    REQUIRE(address(address_v6::from_string("ff00::1").value()).is_multicast());
    REQUIRE_FALSE(address(address_v4::loopback()).is_multicast());
    REQUIRE_FALSE(address(address_v6::loopback()).is_multicast());
}
