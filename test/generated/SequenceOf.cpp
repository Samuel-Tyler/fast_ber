#include "autogen/sequence_of.hpp"

#include "catch2/catch.hpp"

#include <array>

TEST_CASE("Sequence Of: Encode and decode sequence of")
{
    std::array<uint8_t, 100>            buffer   = {};
    fast_ber::Sequence_::SequenceFive<> sequence = {fast_ber::Sequence_::UnnamedSet0<>{{""}}};

    fast_ber::EncodeResult encode_result = fast_ber::encode(absl::MakeSpan(buffer.data(), buffer.size()), sequence);
    fast_ber::DecodeResult decode_result = fast_ber::decode(absl::MakeSpan(buffer.data(), buffer.size()), sequence);

    REQUIRE(encode_result.success);
    REQUIRE(decode_result.success);
}

TEST_CASE("Sequence Of: Empty sequence of")
{
    std::array<uint8_t, 2>              buffer   = {};
    fast_ber::Sequence_::SequenceFive<> sequence = {};

    size_t                 encoded_length = fast_ber::encoded_length(sequence);
    fast_ber::EncodeResult encode_result  = fast_ber::encode(absl::MakeSpan(buffer.data(), buffer.size()), sequence);
    fast_ber::DecodeResult decode_result  = fast_ber::decode(absl::MakeSpan(buffer.data(), buffer.size()), sequence);

    REQUIRE(encoded_length == 2);
    REQUIRE(encode_result.success);
    REQUIRE(decode_result.success);
}
