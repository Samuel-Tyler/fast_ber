#include "autogen/time.hpp"

#include "catch2/catch.hpp"

#include <array>
#include <vector>

TEST_CASE("Time: Test compiled time objects")
{
    fast_ber::Times::GenTime<> a;
    std::array<uint8_t, 5000>  buffer        = {};
    fast_ber::EncodeResult     encode_result = fast_ber::encode(absl::MakeSpan(buffer.data(), buffer.size()), a);
    fast_ber::DecodeResult     decode_result = fast_ber::decode(absl::MakeSpan(buffer.data(), buffer.size()), a);

    REQUIRE(encode_result.success);
    REQUIRE(decode_result.success);
}
