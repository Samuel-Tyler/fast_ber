#include "autogen/enum.hpp"

#include "fast_ber/util/BerView.hpp"

#include "catch2/catch.hpp"

#include <vector>

TEST_CASE("Enumeration: Creating an enum")
{
    fast_ber::Enumerations::NumberedEnumeration numbered = fast_ber::Enumerations::NumberedEnumeration::blue;

    REQUIRE(numbered == fast_ber::Enumerations::NumberedEnumeration::blue);
    numbered = fast_ber::Enumerations::NumberedEnumeration::red;
    REQUIRE(numbered == fast_ber::Enumerations::NumberedEnumeration::red);
    REQUIRE(numbered != fast_ber::Enumerations::NumberedEnumeration::blue);
}

TEST_CASE("Enumeration: Encode Decode")
{
    std::array<uint8_t, 100> data = {};

    fast_ber::Enumerations::Enumeration enum_one = fast_ber::Enumerations::Enumeration::pear;
    fast_ber::Enumerations::Enumeration enum_two = fast_ber::Enumerations::Enumeration::orange;

    REQUIRE(fast_ber::encode(absl::Span<uint8_t>(data), enum_one,
                                              fast_ber::ExplicitIdentifier<fast_ber::UniversalTag::enumerated>{})
                .success);
    REQUIRE(fast_ber::decode(data, enum_two,
                                              fast_ber::ExplicitIdentifier<fast_ber::UniversalTag::enumerated>{}));

    REQUIRE(enum_one == enum_two);
}
