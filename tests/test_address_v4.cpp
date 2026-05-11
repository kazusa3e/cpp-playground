#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <catch2/catch_test_macros.hpp>
#include <compare>
#include <cstdint>
#include <expected>
#include <initializer_list>
#include <string_view>
#include <system_error>
#include "address.hpp"

using kuri::net::address_v4;

// ---- Construction & accessors ----

TEST_CASE("address_v4 default constructs to 0.0.0.0", "[address_v4][ctor]") {
    address_v4 a;
    REQUIRE(a.to_uint32() == 0);
    REQUIRE(a.is_unspecified());
}

TEST_CASE("address_v4 constructs from uint32_t (host byte order)",
          "[address_v4][ctor]") {
    address_v4 a(0x7f000001);
    REQUIRE(a.to_uint32() == 0x7f000001);
    REQUIRE(a.is_loopback());
}

TEST_CASE("address_v4 constructs from in_addr", "[address_v4][ctor]") {
    in_addr ia {};
    // 8.8.8.8 in network byte order
    inet_pton(AF_INET, "8.8.8.8", &ia);
    address_v4 a(ia);
    REQUIRE(a.to_in_addr().s_addr == ia.s_addr);
    REQUIRE(a.to_string() == "8.8.8.8");
}

TEST_CASE("address_v4::to_in_addr round-trips through constructor",
          "[address_v4][roundtrip]") {
    address_v4 a(0xc0a80001);  // 192.168.0.1 host order
    in_addr const ia = a.to_in_addr();
    address_v4 b(ia);
    REQUIRE(a == b);
    REQUIRE(a.to_uint32() == 0xc0a80001);
}

// ---- from_string ----

TEST_CASE("address_v4::from_string parses valid addresses",
          "[address_v4][from_string]") {
    struct m_case {
        std::string_view input;
        uint32_t host_order;
    };
    auto cases = {
        m_case { .input = "0.0.0.0", .host_order = 0 },
        m_case { .input = "127.0.0.1", .host_order = 0x7f000001 },
        m_case { .input = "8.8.8.8", .host_order = 0x08080808 },
        m_case { .input = "192.168.1.1", .host_order = 0xc0a80101 },
        m_case { .input = "255.255.255.255", .host_order = 0xffffffff },
        m_case { .input = "10.0.0.1", .host_order = 0x0a000001 },
        m_case { .input = "172.16.0.1", .host_order = 0xac100001 },
        m_case { .input = "224.0.0.1", .host_order = 0xe0000001 },
    };
    for (auto [input, expected] : cases) {
        auto r = address_v4::from_string(input);
        REQUIRE(r.has_value());
        REQUIRE(r->to_uint32() == expected);
    }
}

TEST_CASE("address_v4::from_string rejects invalid formats",
          "[address_v4][from_string]") {
    auto bad = std::initializer_list<std::string_view> {
        {},        "",           "not an address", "256.0.0.1", "1.2.3.4.5",
        "1.2.3",   "192.168.1.", ".192.168.1.1",   "1.2.3.256", "::1",
        "fe80::1", " 127.0.0.1", "127.0.0.1 ",
    };
    for (auto input : bad) {
        auto r = address_v4::from_string(input);
        REQUIRE(r.has_value() == false);
        REQUIRE(r.error() == kuri::net::address_errc::invalid_format);
    }
}

// ---- to_string ----

TEST_CASE("address_v4::to_string", "[address_v4][to_string]") {
    REQUIRE(address_v4().to_string() == "0.0.0.0");
    REQUIRE(address_v4::loopback().to_string() == "127.0.0.1");
    REQUIRE(address_v4::broadcast().to_string() == "255.255.255.255");
    REQUIRE(address_v4(0x08080808).to_string() == "8.8.8.8");
    REQUIRE(address_v4(0xc0a80101).to_string() == "192.168.1.1");
}

TEST_CASE("address_v4 to_string round-trips with from_string",
          "[address_v4][roundtrip]") {
    auto inputs = { "1.1.1.1",     "10.20.30.40", "172.16.254.1",
                    "192.168.0.1", "224.0.0.5",   "255.255.255.255",
                    "169.254.1.1" };
    for (const auto *input : inputs) {
        auto r = address_v4::from_string(input);
        REQUIRE(r.has_value());
        REQUIRE(r->to_string() == input);
    }
}

// ---- Comparison ----

TEST_CASE("address_v4 ordering", "[address_v4][compare]") {
    address_v4 a(0x01000000);  // 1.0.0.0
    address_v4 b(0x7f000001);  // 127.0.0.1
    address_v4 c(0xffffffff);  // 255.255.255.255

    REQUIRE(a == a);
    REQUIRE(a != b);
    REQUIRE(a < b);
    REQUIRE(b < c);
    REQUIRE(c > a);
    REQUIRE(b >= a);
    REQUIRE(b <= c);
}

TEST_CASE("address_v4 ordering via <=>", "[address_v4][compare]") {
    address_v4 a(0x0a0a0a0a);
    address_v4 b(0x0a0a0a0a);
    REQUIRE((a <=> b) == std::strong_ordering::equal);
    REQUIRE((a <=> address_v4(0x00000000)) == std::strong_ordering::greater);
    REQUIRE((a <=> address_v4(0xffffffff)) == std::strong_ordering::less);
}

// ---- Predicates: is_unspecified ----

TEST_CASE("address_v4::is_unspecified", "[address_v4][predicate]") {
    REQUIRE(address_v4(0x00000000).is_unspecified());
    REQUIRE(address_v4::unspecified().is_unspecified());
    REQUIRE(address_v4().is_unspecified());
    REQUIRE_FALSE(address_v4(0x00000001).is_unspecified());
    REQUIRE_FALSE(address_v4::loopback().is_unspecified());
}

// ---- Predicates: is_loopback ----

TEST_CASE("address_v4::is_loopback", "[address_v4][predicate]") {
    // 127.0.0.0/8 — check boundaries
    REQUIRE(address_v4(0x7f000000).is_loopback());  // 127.0.0.0
    REQUIRE(address_v4(0x7f000001).is_loopback());  // 127.0.0.1
    REQUIRE(address_v4(0x7fffffff).is_loopback());  // 127.255.255.255
    REQUIRE(address_v4::loopback().is_loopback());

    REQUIRE_FALSE(
        address_v4(0x80000000).is_loopback());  // 128.0.0.0 — just beyond
    REQUIRE_FALSE(
        address_v4(0x7e000000).is_loopback());  // 126.0.0.0 — just below
    REQUIRE_FALSE(
        address_v4(0x0a7f0000).is_loopback());  // 10.127.0.0 — wrong byte
}

// ---- Predicates: is_link_local ----

TEST_CASE("address_v4::is_link_local", "[address_v4][predicate]") {
    // 169.254.0.0/16
    REQUIRE(address_v4(0xa9fe0000).is_link_local());  // 169.254.0.0
    REQUIRE(address_v4(0xa9fe0001).is_link_local());  // 169.254.0.1
    REQUIRE(address_v4(0xa9feffff).is_link_local());  // 169.254.255.255

    REQUIRE_FALSE(address_v4(0xa9ff0000).is_link_local());  // 169.255.0.0
    REQUIRE_FALSE(address_v4(0xa9fdffff).is_link_local());  // 169.253.255.255
    REQUIRE_FALSE(address_v4::loopback().is_link_local());
}

// ---- Predicates: is_private ----

TEST_CASE("address_v4::is_private — 10.0.0.0/8", "[address_v4][predicate]") {
    REQUIRE(address_v4(0x0a000000).is_private());        // 10.0.0.0
    REQUIRE(address_v4(0x0a000001).is_private());        // 10.0.0.1
    REQUIRE(address_v4(0x0affffff).is_private());        // 10.255.255.255
    REQUIRE_FALSE(address_v4(0x0b000000).is_private());  // 11.0.0.0
    REQUIRE_FALSE(address_v4(0x09000000).is_private());  // 9.0.0.0
}

TEST_CASE("address_v4::is_private — 172.16.0.0/12", "[address_v4][predicate]") {
    REQUIRE(address_v4(0xac100000).is_private());        // 172.16.0.0
    REQUIRE(address_v4(0xac100001).is_private());        // 172.16.0.1
    REQUIRE(address_v4(0xac1fffff).is_private());        // 172.31.255.255
    REQUIRE_FALSE(address_v4(0xac200000).is_private());  // 172.32.0.0
    REQUIRE_FALSE(address_v4(0xac0fffff).is_private());  // 172.15.255.255
}

TEST_CASE("address_v4::is_private — 192.168.0.0/16",
          "[address_v4][predicate]") {
    REQUIRE(address_v4(0xc0a80000).is_private());        // 192.168.0.0
    REQUIRE(address_v4(0xc0a80001).is_private());        // 192.168.0.1
    REQUIRE(address_v4(0xc0a8ffff).is_private());        // 192.168.255.255
    REQUIRE_FALSE(address_v4(0xc0a90000).is_private());  // 192.169.0.0
    REQUIRE_FALSE(address_v4(0xc0a7ffff).is_private());  // 192.167.255.255
}

// ---- Predicates: is_multicast ----

TEST_CASE("address_v4::is_multicast", "[address_v4][predicate]") {
    REQUIRE(address_v4(0xe0000000).is_multicast());  // 224.0.0.0
    REQUIRE(address_v4(0xe0000001).is_multicast());  // 224.0.0.1 (all-hosts)
    REQUIRE(address_v4(0xefffffff).is_multicast());  // 239.255.255.255
    REQUIRE_FALSE(address_v4(0xdfffffff).is_multicast());  // 223.255.255.255
    REQUIRE_FALSE(address_v4(0xf0000000).is_multicast());  // 240.0.0.0
}

// ---- Predicates: is_broadcast ----

TEST_CASE("address_v4::is_broadcast", "[address_v4][predicate]") {
    REQUIRE(address_v4(0xffffffff).is_broadcast());
    REQUIRE(address_v4::broadcast().is_broadcast());
    REQUIRE_FALSE(address_v4(0xfffffffe).is_broadcast());  // 255.255.255.254
    REQUIRE_FALSE(address_v4::unspecified().is_broadcast());
    REQUIRE_FALSE(address_v4::loopback().is_broadcast());
}

// ---- Static factories ----

TEST_CASE("address_v4 static factories", "[address_v4][factory]") {
    REQUIRE(address_v4::loopback().to_uint32() == 0x7f000001);
    REQUIRE(address_v4::unspecified().to_uint32() == 0);
    REQUIRE(address_v4::broadcast().to_uint32() == 0xffffffff);
}

// ---- No implicit overlap between predicates ----

TEST_CASE("address_v4 predicates are mutually exclusive for standard addresses",
          "[address_v4][predicate]") {
    // loopback is not private, not link-local, not multicast
    auto lb = address_v4::loopback();
    REQUIRE(lb.is_loopback());
    REQUIRE_FALSE(lb.is_unspecified());
    REQUIRE_FALSE(lb.is_link_local());
    REQUIRE_FALSE(lb.is_private());
    REQUIRE_FALSE(lb.is_multicast());
    REQUIRE_FALSE(lb.is_broadcast());

    // link-local is nothing else
    auto lla = address_v4(0xa9fe0101);  // 169.254.1.1
    REQUIRE(lla.is_link_local());
    REQUIRE_FALSE(lla.is_unspecified());
    REQUIRE_FALSE(lla.is_loopback());
    REQUIRE_FALSE(lla.is_private());
    REQUIRE_FALSE(lla.is_multicast());
    REQUIRE_FALSE(lla.is_broadcast());

    // private is nothing else
    auto priv = address_v4(0xc0a80001);  // 192.168.0.1
    REQUIRE(priv.is_private());
    REQUIRE_FALSE(priv.is_unspecified());
    REQUIRE_FALSE(priv.is_loopback());
    REQUIRE_FALSE(priv.is_link_local());
    REQUIRE_FALSE(priv.is_multicast());
    REQUIRE_FALSE(priv.is_broadcast());

    // multicast is nothing else
    auto mc = address_v4(0xe0000005);  // 224.0.0.5
    REQUIRE(mc.is_multicast());
    REQUIRE_FALSE(mc.is_unspecified());
    REQUIRE_FALSE(mc.is_loopback());
    REQUIRE_FALSE(mc.is_link_local());
    REQUIRE_FALSE(mc.is_private());
    REQUIRE_FALSE(mc.is_broadcast());
}
