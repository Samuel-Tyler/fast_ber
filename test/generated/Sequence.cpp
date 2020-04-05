#include "autogen/sequence.hpp"

#include "catch2/catch.hpp"

#include <array>

TEST_CASE("Sequence: Empty")
{
    std::array<uint8_t, 100>     buffer   = {};
    fast_ber::Sequence_::Empty<> sequence = {};

    fast_ber::EncodeResult encode_result = fast_ber::encode(absl::MakeSpan(buffer.data(), buffer.size()), sequence);
    fast_ber::DecodeResult decode_result = fast_ber::decode(absl::MakeSpan(buffer.data(), buffer.size()), sequence);

    REQUIRE(encode_result.success);
    REQUIRE(decode_result.success);
}

TEST_CASE("Sequence: Tagged Collection")
{
    fast_ber::Sequence_::Collection<> collection = {{}, {}, 5, true, "O"};
    fast_ber::Sequence_::Collection<fast_ber::Id<fast_ber::Class::context_specific, 10>> tagged_collection = {
        {}, {}, -5, false, "P"};

    REQUIRE(collection != tagged_collection);

    tagged_collection = collection;
    REQUIRE(collection == tagged_collection);
}

TEST_CASE("Sequence: Tagged")
{
    std::array<uint8_t, 100> buffer = {};
    fast_ber::Sequence_::Empty<fast_ber::DoubleId<fast_ber::Id<fast_ber::Class::context_specific, 10>,
                                                  fast_ber::ExplicitId<fast_ber::UniversalTag::sequence>>>
        sequence = {};

    fast_ber::EncodeResult encode_result = fast_ber::encode(absl::MakeSpan(buffer.data(), buffer.size()), sequence);
    fast_ber::DecodeResult decode_result = fast_ber::decode(absl::MakeSpan(buffer.data(), buffer.size()), sequence);

    REQUIRE(encode_result.success);
    REQUIRE(decode_result.success);
    REQUIRE(encode_result.length == 4);
    REQUIRE(fast_ber::BerView(buffer).is_valid());
    REQUIRE(fast_ber::BerView(buffer).identifier() == fast_ber::RuntimeId{fast_ber::Class::context_specific, 10});
    REQUIRE(fast_ber::BerView(buffer).begin()->identifier() == fast_ber::RuntimeId{fast_ber::UniversalTag::sequence});
}
