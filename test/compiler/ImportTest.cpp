#include "autogen/import.hpp"
#include "autogen/multi_file_import.hpp"

#include "catch2/catch.hpp"

#include <type_traits>
#include <vector>

TEST_CASE("Imports: Single file")
{
    REQUIRE(std::is_same<decltype(fast_ber::ModuleA::Collection::string), fast_ber::OctetString<>>::value);
    REQUIRE(std::is_same<decltype(fast_ber::ModuleA::Collection::integer), fast_ber::Integer<>>::value);
}

TEST_CASE("Imports: Multiple files")
{
    REQUIRE(std::is_same<decltype(fast_ber::MultiModuleA::Collection::string), fast_ber::OctetString<>>::value);
    REQUIRE(std::is_same<decltype(fast_ber::MultiModuleA::Collection::integer), fast_ber::Integer<>>::value);
}
