#include "autogen/import.hpp"

#include "catch2/catch.hpp"

#include <type_traits>
#include <vector>

TEST_CASE("Imports")
{
    REQUIRE(std::is_same<decltype(fast_ber::ModuleA::Collection::string), fast_ber::OctetString>::value);
    REQUIRE(std::is_same<decltype(fast_ber::ModuleA::Collection::integer), fast_ber::Integer>::value);
}
