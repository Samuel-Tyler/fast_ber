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
