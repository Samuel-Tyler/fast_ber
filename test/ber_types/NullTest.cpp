#include "fast_ber/ber_types/Null.hpp"

#include <catch2/catch.hpp>

TEST_CASE("Null: Encoding")
{
    fast_ber::Null<>         null;
    std::array<uint8_t, 100> buffer   = {};
    std::array<uint8_t, 2>   expected = {0x05, 0x00};

    size_t size = fast_ber::encode(absl::Span<uint8_t>(buffer.data(), buffer.size()), null).length;

    REQUIRE(size == 2);
    REQUIRE(absl::MakeSpan(buffer.data(), 2) == absl::MakeSpan(expected));
}

TEST_CASE("Null: Assign from raw")
{
    fast_ber::Null<>       null;
    std::array<uint8_t, 2> test_data = {0x05, 0x00};

    size_t size = null.assign_ber(absl::MakeSpan(test_data.data(), test_data.size()));
    REQUIRE(size == 2);
}

TEST_CASE("Null: Double id")
{
    fast_ber::Null<fast_ber::DoubleId<fast_ber::Id<fast_ber::Class::context_specific, 1000000>,
                                      fast_ber::Id<fast_ber::Class::context_specific, 1000001>>>
                            null;
    std::array<uint8_t, 10> buffer;

    constexpr size_t       encoded_length = fast_ber::encoded_length(null);
    fast_ber::EncodeResult encode_result  = fast_ber::encode(absl::Span<uint8_t>(buffer), null);
    fast_ber::DecodeResult decode_result  = fast_ber::decode(buffer, null);

    REQUIRE(encode_result.success);
    REQUIRE(decode_result.success);
    REQUIRE(encode_result.length == 10);
    REQUIRE(encoded_length == 10);
}
