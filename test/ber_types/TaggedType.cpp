#include "fast_ber/ber_types/TaggedType.hpp"
#include "fast_ber/ber_types/Identifier.hpp"
#include "fast_ber/ber_types/Integer.hpp"

#include <catch2/catch.hpp>

#include <array>

TEST_CASE("TaggedType: Assign")
{
    fast_ber::Integer<> a(4);
    fast_ber::TaggedType<fast_ber::Integer<>,
                         fast_ber::ImplicitIdentifier<fast_ber::Class::universal, fast_ber::Tag(5)>>
        b = a;

    REQUIRE(b == 4);
    REQUIRE(fast_ber::reference_tag(identifier(&b)) == 5);
}

TEST_CASE("TaggedType: Double Tagged")
{
    fast_ber::Integer<> a(4);
    fast_ber::TaggedType<fast_ber::TaggedType<fast_ber::Integer<>, fast_ber::ImplicitIdentifier<
                                                                       fast_ber::Class::universal, fast_ber::Tag(5)>>,
                         fast_ber::ImplicitIdentifier<fast_ber::Class::universal, fast_ber::Tag(10)>>
        b = a;

    REQUIRE(b == 4);
    REQUIRE(fast_ber::reference_tag(identifier(&b)) == 10);
}

TEST_CASE("TaggedType: Encode Decode")
{
    fast_ber::TaggedType<fast_ber::Integer<>,
                         fast_ber::ImplicitIdentifier<fast_ber::Class::universal, fast_ber::Tag(5)>>
        a = 10;
    fast_ber::TaggedType<fast_ber::Integer<>,
                         fast_ber::ImplicitIdentifier<fast_ber::Class::universal, fast_ber::Tag(5)>>
                             b      = 20;
    std::array<uint8_t, 100> buffer = {};

    fast_ber::encode(absl::Span<uint8_t>(buffer), a);
    fast_ber::decode(absl::Span<const uint8_t>(buffer), b);

    REQUIRE(b == 10);
}
