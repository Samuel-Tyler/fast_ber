#include "fast_ber/ber_types/Integer.hpp"

#include <catch2/catch.hpp>
#include <iostream>
TEST_CASE("Integer: Construction from int")
{
    /* std::initializer_list<int64_t> test_vals{-9999999999, -50, -20, 0, 20, 50, 99999, 999999999};

     for (int64_t val : test_vals)
     {
         fast_ber::Integer i(val);
         REQUIRE(i.value() == val);
     }*/
}

TEST_CASE("Integer: Encoding")
{ /*
     fast_ber::Integer        i(100);
     std::array<uint8_t, 100> buffer;

     size_t size = i.encode(absl::Span<uint8_t>(buffer.begin(), buffer.size()));

     REQUIRE(size > 0);*/
}
