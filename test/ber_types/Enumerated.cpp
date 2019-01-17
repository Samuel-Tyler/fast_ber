#include "fast_ber/ber_types/Enumerated.hpp"

#include <catch2/catch.hpp>

#include <array>

enum class TestEnum
{
    option_one,
    option_two,
    option_three
};

TEST_CASE("Enumerated: Encode Decode")
{
    std::array<uint8_t, 100> data = {};

    TestEnum enum_one = TestEnum::option_one;
    TestEnum enum_two = TestEnum::option_three;

    fast_ber::encode_with_specific_id(absl::Span<uint8_t>(data), enum_one,
                                      fast_ber::ExplicitIdentifier<fast_ber::UniversalTag::enumerated>{});
    fast_ber::decode_with_specific_id(absl::Span<const uint8_t>(data), enum_two,
                                      fast_ber::ExplicitIdentifier<fast_ber::UniversalTag::enumerated>{});

    REQUIRE(enum_one == enum_two);
}
