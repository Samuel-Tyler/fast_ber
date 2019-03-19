#include "fast_ber/ber_types/GeneralizedTime.hpp"
#include "fast_ber/ber_types/Identifier.hpp"
#include "fast_ber/util/DecodeHelpers.hpp"
#include "fast_ber/util/EncodeHelpers.hpp"

#include <catch2/catch.hpp>
#include <limits>

TEST_CASE("GeneralizdTime: Assign")
{
    const auto                now = std::chrono::high_resolution_clock::now();
    fast_ber::GeneralizedTime time(now);
    REQUIRE(time.universal_time_available());
    REQUIRE(fast_ber::GeneralizedTime(time.universal_time()).string() == time.string());

    time.set_local_time(std::chrono::high_resolution_clock::time_point());
    REQUIRE(time.local_time_available());

    REQUIRE(time.local_time() == std::chrono::high_resolution_clock::time_point());
    REQUIRE(time.string() == "19700101000000");

    time.set_universal_time(std::chrono::high_resolution_clock::time_point());
    REQUIRE(time.universal_time_available());
    REQUIRE(time.universal_time() == std::chrono::high_resolution_clock::time_point());
    REQUIRE(time.string() == "19700101000000Z");
}

TEST_CASE("GeneralizdTime: Encode Decode")

{
    std::array<uint8_t, 100> buffer = {};

    fast_ber::GeneralizedTime first  = std::chrono::high_resolution_clock::time_point();
    fast_ber::GeneralizedTime second = std::chrono::high_resolution_clock::time_point();

    fast_ber::EncodeResult encode_res = fast_ber::encode(absl::Span<uint8_t>(buffer), first);
    fast_ber::DecodeResult decode_res = fast_ber::decode(absl::Span<uint8_t>(buffer), second);

    REQUIRE(encode_res.success);
    REQUIRE(decode_res.success);

    REQUIRE(first == second);
}

TEST_CASE("GeneralizdTime: Encoding")

{
    std::array<uint8_t, 100> buffer   = {};
    std::array<uint8_t, 17>  expected = {0x18, 0x0F, 0x32, 0x30, 0x31, 0x39, 0x30, 0x33, 0x31,
                                        0x39, 0x32, 0x31, 0x30, 0x39, 0x34, 0x32, 0x5A};

    fast_ber::GeneralizedTime time       = std::chrono::high_resolution_clock::from_time_t(1553029782);
    fast_ber::EncodeResult    encode_res = fast_ber::encode(absl::Span<uint8_t>(buffer), time);

    REQUIRE(encode_res.success);
    REQUIRE(time.string() == "20190319210942Z");
    REQUIRE(absl::MakeSpan(buffer.data(), expected.size()) == absl::MakeSpan(expected.data(), expected.size()));
}

TEST_CASE("GeneralizdTime: Default value")
{
    REQUIRE(fast_ber::GeneralizedTime().string() == "19700101000000Z");
    REQUIRE(fast_ber::GeneralizedTime().universal_time_available());
    REQUIRE(fast_ber::GeneralizedTime().universal_time() ==
            std::chrono::time_point<std::chrono::high_resolution_clock>());
}
