#include "fast_ber/ber_types/Enumerated.hpp"

#include <catch2/catch.hpp>

#include <array>

enum class TestEnumValues
{
    option_one,
    option_two,
    option_three
};

std::ostream& operator<<(std::ostream& os, const TestEnumValues& obj) noexcept
{
    switch (obj)
    {
    case TestEnumValues::option_one:
        return os << "option_one";
    case TestEnumValues::option_two:
        return os << "option_two";
    case TestEnumValues::option_three:
        return os << "option_three";
    }
}

using TestEnum = fast_ber::Enumerated<TestEnumValues>;

TEST_CASE("Enumerated: Encode Decode")
{
    std::array<uint8_t, 100> data = {};

    TestEnum enum_one = TestEnum::Values::option_one;
    TestEnum enum_two = TestEnum::Values::option_three;

    fast_ber::encode(absl::Span<uint8_t>(data), enum_one);
    fast_ber::decode(absl::Span<const uint8_t>(data), enum_two);

    REQUIRE(enum_one == enum_two);
}

static_assert(
    std::is_same<fast_ber::Identifier<TestEnum>, fast_ber::ExplicitId<fast_ber::UniversalTag::enumerated>>::value,
    "Enum Identifier");
