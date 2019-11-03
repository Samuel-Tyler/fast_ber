#include "autogen/import.hpp"
#include "autogen/multi_file_import.hpp"

#include "catch2/catch.hpp"

#include <type_traits>
#include <vector>

TEST_CASE("Imports: Single file")
{
    fast_ber::ModuleA::Collection c;
    REQUIRE(std::is_same<decltype(c.string)::AliasedType, fast_ber::OctetString<>>::value);
    REQUIRE(std::is_same<decltype(c.integer)::AliasedType, fast_ber::Integer<>>::value);

    REQUIRE(fast_ber::ModuleA::integer_value == 5);
    REQUIRE(fast_ber::ModuleA::string_value == "pear");
}

TEST_CASE("Imports: Multiple files")
{
    fast_ber::MultiModuleA::Collection c;
    REQUIRE(std::is_same<decltype(c.string)::AliasedType, fast_ber::OctetString<>>::value);
    REQUIRE(std::is_same<decltype(c.integer)::AliasedType, fast_ber::Integer<>>::value);

    REQUIRE(fast_ber::MultiModuleA::integer_value == 5);
    REQUIRE(fast_ber::MultiModuleA::string_value == "pear");
}
