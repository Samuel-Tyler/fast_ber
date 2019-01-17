#include "fast_ber/ber_types/Null.hpp"

#include <catch2/catch.hpp>

TEST_CASE("Null: Encoding")
{
    fast_ber::Null           null;
    std::array<uint8_t, 100> buffer   = {};
    std::array<uint8_t, 2>   expected = {0x05, 0x00};

    size_t size = fast_ber::encode_with_specific_id(absl::Span<uint8_t>(buffer.begin(), buffer.size()), null,
                                                    fast_ber::ExplicitIdentifier<fast_ber::UniversalTag::null>{})
                      .length;

    REQUIRE(size == 2);
    REQUIRE(absl::MakeSpan(buffer.data(), 2) == absl::MakeSpan(expected));
}

TEST_CASE("Null: Assign from raw")
{
    fast_ber::Null         null;
    std::array<uint8_t, 2> test_data = {0x05, 0x00};

    size_t size = null.assign_ber(absl::MakeSpan(test_data.data(), test_data.size()));
    REQUIRE(size == 2);
}
