#include "autogen/sequence_of.hpp"

#include "catch2/catch.hpp"

#include <array>

TEST_CASE("Sequence Of: Encode and decode sequences")
{
    std::array<uint8_t, 100>         buffer   = {};
    fast_ber::Sequence::SequenceFive sequence = {{{""}}};

    fast_ber::EncodeResult encode_result = fast_ber::encode(absl::MakeSpan(buffer.data(), buffer.size()), sequence);
    fast_ber::DecodeResult decode_result = fast_ber::decode(absl::MakeSpan(buffer.data(), buffer.size()), sequence);

    REQUIRE(encode_result.success);
    REQUIRE(decode_result.success);
}
