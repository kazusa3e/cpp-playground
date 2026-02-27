#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>
#include "fib/fib.hpp"

TEST_CASE("Simple test case", "[fib]") {
    REQUIRE(fib(0) == 0);
    REQUIRE(fib(1) == 1);
    REQUIRE(fib(2) == 1);
    REQUIRE(fib(3) == 2);
    REQUIRE(fib(4) == 3);
    REQUIRE(fib(5) == 5);
    REQUIRE(fib(6) == 8);
    REQUIRE(fib(7) == 13);
    REQUIRE(fib(8) == 21);
    REQUIRE(fib(9) == 34);
    REQUIRE(fib(10) == 55);
}

TEST_CASE("Sample test case for three implementations", "[fib]") {
    REQUIRE(fib(0) == 0);
    REQUIRE(fib(10) == 55);
    REQUIRE(fib(20) == 6765);
    REQUIRE(fib(30) == 832040);
    REQUIRE(fib(40) == 102334155);

    REQUIRE(fib_memo(0) == 0);
    REQUIRE(fib_memo(10) == 55);
    REQUIRE(fib_memo(20) == 6765);
    REQUIRE(fib_memo(30) == 832040);
    REQUIRE(fib_memo(40) == 102334155);

    REQUIRE(fib_calc(0) == 0);
    REQUIRE(fib_calc(10) == 55);
    REQUIRE(fib_calc(20) == 6765);
    REQUIRE(fib_calc(30) == 832040);
    REQUIRE(fib_calc(40) == 102334155);
}

TEST_CASE("Fib benchmark - quick validation", "[fib][benchmark]") {
    BENCHMARK("fib 20") { return fib(20); };
    BENCHMARK("fib_memo 20") { return fib_memo(20); };
    BENCHMARK("fib_calc 20") { return fib_calc(20); };

    BENCHMARK("fib 25") { return fib(25); };
    BENCHMARK("fib_memo 25") { return fib_memo(25); };
    BENCHMARK("fib_calc 25") { return fib_calc(25); };

    BENCHMARK("fib 30") { return fib(30); };
    BENCHMARK("fib_memo 30") { return fib_memo(30); };
    BENCHMARK("fib_calc 30") { return fib_calc(30); };
}