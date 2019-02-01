#include "fast_ber/ber_types/Construction.hpp"

#include <catch2/catch.hpp>

TEST_CASE("Construction: get primitive")
{
    REQUIRE(fast_ber::get_construction(0x00) == fast_ber::Construction::primitive);
}

TEST_CASE("Construction: get constructed")
{
    REQUIRE(fast_ber::get_construction(0x20) == fast_ber::Construction::constructed);
}

TEST_CASE("Construction: set primitive")
{
    uint8_t first = 0x00;
    first         = fast_ber::add_construction(first, fast_ber::Construction::primitive);
    REQUIRE(fast_ber::get_construction(first) == fast_ber::Construction::primitive);

    first = 0xFF;
    first = fast_ber::add_construction(first, fast_ber::Construction::primitive);
    REQUIRE(fast_ber::get_construction(first) == fast_ber::Construction::primitive);
}

TEST_CASE("Construction: set constructed")
{
    uint8_t first = 0x00;
    first         = fast_ber::add_construction(first, fast_ber::Construction::constructed);
    REQUIRE(fast_ber::get_construction(first) == fast_ber::Construction::constructed);

    first = 0xFF;
    first = fast_ber::add_construction(first, fast_ber::Construction::constructed);
    REQUIRE(fast_ber::get_construction(first) == fast_ber::Construction::constructed);
}
