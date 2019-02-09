#include "fast_ber/ber_types/Identifier.hpp"
#include "fast_ber/ber_types/Integer.hpp"
#include "fast_ber/util/DecodeHelpers.hpp"
#include "fast_ber/util/EncodeHelpers.hpp"

#include <catch2/catch.hpp>

TEST_CASE("Identifier: Encode ExplicitIdentifier")
{
    fast_ber::Integer        i(4);
    std::array<uint8_t, 100> buffer   = {};
    std::array<uint8_t, 3>   expected = {0x02, 0x01, 0x04};
    size_t                   size     = fast_ber::encode(absl::Span<uint8_t>(buffer.begin(), buffer.size()), i,
                                   fast_ber::ExplicitIdentifier<fast_ber::UniversalTag::integer>{})
                      .length;

    REQUIRE(size == 3);
    REQUIRE(absl::MakeSpan(buffer.data(), 3) == absl::MakeSpan(expected));
}

TEST_CASE("Identifier: Encode TaggedExplicitIdentifier")
{
    fast_ber::Integer        i(4);
    std::array<uint8_t, 100> buffer   = {};
    std::array<uint8_t, 5>   expected = {0xB4, 0x03, 0x02, 0x01, 0x04};
    size_t                   size =
        fast_ber::encode(
            absl::Span<uint8_t>(buffer.begin(), buffer.size()), i,
            fast_ber::TaggedExplicitIdentifier<fast_ber::Class::context_specific, 20,
                                               fast_ber::ExplicitIdentifier<fast_ber::UniversalTag::integer>>{})
            .length;

    REQUIRE(size == 5);
    REQUIRE(absl::MakeSpan(buffer.data(), 5) == absl::MakeSpan(expected));
}

TEST_CASE("Identifier: Encode ImplicitIdentifier")
{
    fast_ber::Integer        i(4);
    std::array<uint8_t, 100> buffer   = {};
    std::array<uint8_t, 3>   expected = {0x94, 0x01, 0x04};
    size_t                   size     = fast_ber::encode(absl::Span<uint8_t>(buffer.begin(), buffer.size()), i,
                                   fast_ber::ImplicitIdentifier<fast_ber::Class::context_specific, 20>{})
                      .length;

    REQUIRE(size == 3);
    REQUIRE(absl::MakeSpan(buffer.data(), 3) == absl::MakeSpan(expected));
}

TEST_CASE("Identifier: Decode ExplicitIdentifier")
{
    std::array<uint8_t, 3> data     = {0x02, 0x01, 0x04};
    auto                   iterator = fast_ber::BerViewIterator(absl::Span<uint8_t>(data.begin(), data.size()));
    fast_ber::Integer      i        = 500;
    bool                   success =
        fast_ber::decode(iterator, i, fast_ber::ExplicitIdentifier<fast_ber::UniversalTag::integer>{}).success;

    REQUIRE(success);
    REQUIRE(i == 4);
}

TEST_CASE("Identifier: Decode TaggedExplicitIdentifier")
{
    std::array<uint8_t, 5> data     = {0x94, 0x03, 0x02, 0x01, 0x04};
    auto                   iterator = fast_ber::BerViewIterator(absl::Span<uint8_t>(data.begin(), data.size()));
    fast_ber::Integer      i        = 500;
    bool                   success =
        fast_ber::decode(
            iterator, i,
            fast_ber::TaggedExplicitIdentifier<fast_ber::Class::context_specific, 20,
                                               fast_ber::ExplicitIdentifier<fast_ber::UniversalTag::integer>>{})
            .success;

    REQUIRE(success);
    REQUIRE(i == 4);
}

TEST_CASE("Identifier: Decode ImplicitIdentifier")
{
    std::array<uint8_t, 3> data     = {0x94, 0x01, 0x04};
    auto                   iterator = fast_ber::BerViewIterator(absl::Span<uint8_t>(data.begin(), data.size()));
    fast_ber::Integer      i        = 500;
    bool                   success =
        fast_ber::decode(iterator, i, fast_ber::ImplicitIdentifier<fast_ber::Class::context_specific, 20>{}).success;

    REQUIRE(success);
    REQUIRE(i == 4);
}
