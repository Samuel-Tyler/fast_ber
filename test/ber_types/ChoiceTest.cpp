#include "fast_ber/ber_types/Choice.hpp"
#include "fast_ber/ber_types/Boolean.hpp"
#include "fast_ber/ber_types/Identifier.hpp"
#include "fast_ber/ber_types/Integer.hpp"
#include "fast_ber/ber_types/OctetString.hpp"

#include <catch2/catch.hpp>

TEST_CASE("Choice: Assignment")
{
    auto choice = fast_ber::Choice<fast_ber::Choices<fast_ber::Integer<>, fast_ber::OctetString<>>>("Test string");
    CHECK(fast_ber::get<1>(choice) == fast_ber::OctetString<>("Test string"));
    choice = 10;
    CHECK(fast_ber::get<0>(choice) == fast_ber::Integer<>(10));
}
