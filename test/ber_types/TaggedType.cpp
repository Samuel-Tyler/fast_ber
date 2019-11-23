#include "fast_ber/ber_types/TaggedType.hpp"
#include "fast_ber/ber_types/All.hpp"
#include "fast_ber/ber_types/Identifier.hpp"
#include "fast_ber/ber_types/Integer.hpp"

#include <catch2/catch.hpp>

#include <array>

TEST_CASE("TaggedType: Assign")
{
    using Tag       = fast_ber::Id<fast_ber::Class::universal, fast_ber::Tag(5)>;
    using TaggedInt = fast_ber::Integer<Tag>;
    fast_ber::Integer<> a(4);
    TaggedInt           b = a;

    REQUIRE(b == 4);
    REQUIRE(fast_ber::val(fast_ber::Identifier<TaggedInt>::tag()) == 5);

    static_assert(std::is_same<fast_ber::Identifier<TaggedInt>, Tag>::value, "Double Tagged Identifier");
}

TEST_CASE("TaggedType: Encode Decode")
{
    fast_ber::Integer<fast_ber::Id<fast_ber::Class::universal, fast_ber::Tag(5)>> a      = 10;
    fast_ber::Integer<fast_ber::Id<fast_ber::Class::universal, fast_ber::Tag(5)>> b      = 20;
    std::array<uint8_t, 100>                                                                      buffer = {};

    fast_ber::encode(absl::Span<uint8_t>(buffer), a);
    fast_ber::decode(absl::Span<const uint8_t>(buffer), b);

    REQUIRE(b == 10);
}

TEST_CASE("TaggedType: Tagged Enum")
{
    enum class TestEnumValues
    {
        option_one,
        option_two,
        option_three
    };

    using TestEnum = fast_ber::Enumerated<TestEnumValues, fast_ber::ExplicitId<fast_ber::UniversalTag::octet_string>>;

    static_assert(
        std::is_same<fast_ber::Identifier<TestEnum>, fast_ber::ExplicitId<fast_ber::UniversalTag::octet_string>>::value,
        "Tagged Enum Identifier");
}
