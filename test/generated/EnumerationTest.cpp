#include "autogen/enum.hpp"

#include "fast_ber/ber_types/Enumerated.hpp"
#include "fast_ber/util/BerView.hpp"

#include "catch2/catch.hpp"

#include <vector>

TEST_CASE("Enumeration: Creating an enum")
{
    fast_ber::Enumerations::NumberedEnumeration<> numbered =
        fast_ber::Enumerations::NumberedEnumeration<>::Values::blue;

    REQUIRE(numbered == fast_ber::Enumerations::NumberedEnumeration<>::Values::blue);
    numbered = fast_ber::Enumerations::NumberedEnumeration<>::Values::red;
    REQUIRE(numbered == fast_ber::Enumerations::NumberedEnumeration<>::Values::red);
    REQUIRE(numbered != fast_ber::Enumerations::NumberedEnumeration<>::Values::blue);
}

TEST_CASE("Enumeration: Encode Decode")
{
    std::array<uint8_t, 100> data = {};

    fast_ber::Enumerations::Enumeration<> enum_one = fast_ber::Enumerations::Enumeration<>::Values::pear;
    fast_ber::Enumerations::Enumeration<> enum_two = fast_ber::Enumerations::Enumeration<>::Values::orange;

    REQUIRE(fast_ber::encode(absl::Span<uint8_t>(data), enum_one).success);
    REQUIRE(fast_ber::decode(data, enum_two).success);

    REQUIRE(enum_one == enum_two);
}
