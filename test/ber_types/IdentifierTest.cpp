#include "fast_ber/ber_types/Identifier.hpp"
#include "fast_ber/ber_types/Integer.hpp"
#include "fast_ber/util/DecodeHelpers.hpp"
#include "fast_ber/util/EncodeHelpers.hpp"

#include <catch2/catch.hpp>

TEST_CASE("Identifier: Encode ExplicitIdentifier")
{
    fast_ber::Integer<>      i(4);
    std::array<uint8_t, 100> buffer   = {};
    std::array<uint8_t, 3>   expected = {0x02, 0x01, 0x04};
    size_t                   enc_size = fast_ber::encoded_length(i);
    size_t                   size     = fast_ber::encode(absl::Span<uint8_t>(buffer.data(), buffer.size()), i).length;

    REQUIRE(size == 3);
    REQUIRE(enc_size == 3);
    REQUIRE(absl::MakeSpan(buffer.data(), 3) == absl::MakeSpan(expected));
}

TEST_CASE("Identifier: Encode TaggedExplicitIdentifier")
{
    fast_ber::Integer<fast_ber::DoubleId<fast_ber::Id<fast_ber::Class::context_specific, 20>,
                                         fast_ber::ExplicitId<fast_ber::UniversalTag::integer>>>
                             i(4);
    std::array<uint8_t, 100> buffer   = {};
    std::array<uint8_t, 5>   expected = {0xB4, 0x03, 0x02, 0x01, 0x04};
    size_t                   enc_size = fast_ber::encoded_length(i);
    size_t                   size     = fast_ber::encode(absl::Span<uint8_t>(buffer.data(), buffer.size()), i).length;

    REQUIRE(size == 5);
    REQUIRE(enc_size == 5);
    REQUIRE(absl::MakeSpan(buffer.data(), 5) == absl::MakeSpan(expected));
}

TEST_CASE("Identifier: Encode Id")
{
    fast_ber::Integer<fast_ber::Id<fast_ber::Class::context_specific, 20>> i(4);

    std::array<uint8_t, 100> buffer   = {};
    std::array<uint8_t, 3>   expected = {0x94, 0x01, 0x04};
    size_t                   enc_size = fast_ber::encoded_length(i);
    size_t                   size     = fast_ber::encode(absl::Span<uint8_t>(buffer.data(), buffer.size()), i).length;

    REQUIRE(size == 3);
    REQUIRE(enc_size == 3);
    REQUIRE(absl::MakeSpan(buffer.data(), 3) == absl::MakeSpan(expected));
}

TEST_CASE("Identifier: Decode ExplicitIdentifier")
{
    std::array<uint8_t, 3> data     = {0x02, 0x01, 0x04};
    auto                   iterator = fast_ber::BerViewIterator(absl::Span<uint8_t>(data.data(), data.size()));
    fast_ber::Integer<>    i        = 500;
    bool                   success  = fast_ber::decode(iterator, i).success;

    REQUIRE(success);
    REQUIRE(i == 4);
}

TEST_CASE("Identifier: Decode TaggedExplicitIdentifier")
{
    std::array<uint8_t, 5> data     = {0x94, 0x03, 0x02, 0x01, 0x04};
    auto                   iterator = fast_ber::BerViewIterator(absl::Span<uint8_t>(data.data(), data.size()));
    fast_ber::Integer<fast_ber::DoubleId<fast_ber::Id<fast_ber::Class::context_specific, 20>,
                                         fast_ber::ExplicitId<fast_ber::UniversalTag::integer>>>
         i       = 500;
    bool success = fast_ber::decode(iterator, i).success;

    REQUIRE(success);
    REQUIRE(i == 4);
}

TEST_CASE("Identifier: Decode Id")
{
    std::array<uint8_t, 3> data     = {0x94, 0x01, 0x04};
    auto                   iterator = fast_ber::BerViewIterator(absl::Span<uint8_t>(data.data(), data.size()));
    fast_ber::Integer<fast_ber::Id<fast_ber::Class::context_specific, 20>> i = 500;
    bool success                                                             = fast_ber::decode(iterator, i).success;

    REQUIRE(success);
    REQUIRE(i == 4);
}
