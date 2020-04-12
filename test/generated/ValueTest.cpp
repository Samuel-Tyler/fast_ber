#include "autogen/value_assign.hpp"

#include "catch2/catch.hpp"

TEST_CASE("Value Assignments")
{
    REQUIRE(fast_ber::Values::defined_oid == fast_ber::ObjectIdentifier<>{0, 1, 2});
    REQUIRE(fast_ber::Values::defined_str == "Constant string");
    REQUIRE(fast_ber::Values::defined_int == -1123983485334);
}
