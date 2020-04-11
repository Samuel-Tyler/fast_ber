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
    uint8_t test_byte_1 = 0x00;
    test_byte_1         = fast_ber::add_construction(test_byte_1, fast_ber::Construction::primitive);
    REQUIRE(fast_ber::get_construction(test_byte_1) == fast_ber::Construction::primitive);

    uint8_t test_byte_2 = 0xFF;
    test_byte_2 = fast_ber::add_construction(test_byte_2, fast_ber::Construction::primitive);
    REQUIRE(fast_ber::get_construction(test_byte_2) == fast_ber::Construction::primitive);
}

TEST_CASE("Construction: set constructed")
{
    uint8_t test_byte_1 = 0x00;
    test_byte_1         = fast_ber::add_construction(test_byte_1, fast_ber::Construction::constructed);
    REQUIRE(fast_ber::get_construction(test_byte_1) == fast_ber::Construction::constructed);

    uint8_t test_byte_2 = 0xFF;
    test_byte_2 = fast_ber::add_construction(test_byte_2, fast_ber::Construction::constructed);
    REQUIRE(fast_ber::get_construction(test_byte_2) == fast_ber::Construction::constructed);
}
