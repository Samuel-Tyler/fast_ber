#include "autogen/sequence.hpp"

#include "catch2/catch.hpp"

#include <array>

TEST_CASE("Sequence: Empty")
{
    std::array<uint8_t, 100>   buffer   = {};
    fast_ber::Sequence_::Empty sequence = {};

    fast_ber::EncodeResult encode_result = fast_ber::encode(absl::MakeSpan(buffer.data(), buffer.size()), sequence);
    fast_ber::DecodeResult decode_result = fast_ber::decode(absl::MakeSpan(buffer.data(), buffer.size()), sequence);

    REQUIRE(encode_result.success);
    REQUIRE(decode_result.success);
}

TEST_CASE("Sequence: Equality")
{
    fast_ber::Sequence_::Collection collection_1 = {{}, {}, 5, true, "O"};
    fast_ber::Sequence_::Collection collection_2 = {{}, {}, -5, false, "P"};

    REQUIRE(collection_1 != collection_2);

    collection_2 = collection_1;
    REQUIRE(collection_1 == collection_2);
}

TEST_CASE("Sequence: Encode / Decode")
{
    std::array<uint8_t, 100>   buffer   = {};
    fast_ber::Sequence_::Empty sequence = {};

    fast_ber::EncodeResult encode_result = fast_ber::encode(absl::MakeSpan(buffer.data(), buffer.size()), sequence);
    fast_ber::DecodeResult decode_result = fast_ber::decode(absl::MakeSpan(buffer.data(), buffer.size()), sequence);

    REQUIRE(encode_result.success);
    REQUIRE(decode_result.success);
    REQUIRE(encode_result.length == 2);

    /* CORRECT OUTPUT:
     * TODO: Fixme
     * REQUIRE(encode_result.success);
     REQUIRE(decode_result.success);
     REQUIRE(encode_result.length == 4);
     REQUIRE(fast_ber::BerView(buffer).is_valid());
     REQUIRE(fast_ber::BerView(buffer).identifier() == fast_ber::RuntimeId{fast_ber::Class::context_specific, 10});
     REQUIRE(fast_ber::BerView(buffer).begin()->identifier() == fast_ber::RuntimeId{fast_ber::UniversalTag::sequence});
   */
}
