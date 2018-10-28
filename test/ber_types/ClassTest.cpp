#include "fast_ber/ber_types/Class.hpp"

#include <catch2/catch.hpp>

TEST_CASE("Class: Universal") { REQUIRE(fast_ber::get_class(0x00) == fast_ber::Class::universal); }
TEST_CASE("Class: Application") { REQUIRE(fast_ber::get_class(0x40) == fast_ber::Class::application); }
TEST_CASE("Class: Context specific") { REQUIRE(fast_ber::get_class(0x80) == fast_ber::Class::context_specific); }
TEST_CASE("Class: Private") { REQUIRE(fast_ber::get_class(0xC0) == fast_ber::Class::private_); }
