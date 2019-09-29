#include "autogen/prefixed_types.hpp"

#include "fast_ber/util/BerView.hpp"

#include "catch2/catch.hpp"

#include <vector>

TEST_CASE("Prefixed Types: Encoding, matching 8.14 of X.690")
{
    std::array<uint8_t, 100> buffer = {};

    fast_ber::PrefixedTypes::Type1 type1 = "Jones";
    fast_ber::PrefixedTypes::Type2 type2 = "Jones";
    fast_ber::PrefixedTypes::Type3 type3 = "Jones";
    fast_ber::PrefixedTypes::Type4 type4 = "Jones";
    fast_ber::PrefixedTypes::Type5 type5 = "Jones";

    fast_ber::encode(absl::MakeSpan(buffer.data(), buffer.size()), type1);
    REQUIRE(absl::MakeSpan(buffer.data(), 7) == std::array<uint8_t, 7>{0x1A, 0x05, 0x4A, 0x6F, 0x6E, 0x65, 0x73});

    fast_ber::encode(absl::MakeSpan(buffer.data(), buffer.size()), type2);
    REQUIRE(absl::MakeSpan(buffer.data(), 7) == std::array<uint8_t, 7>{0x43, 0x05, 0x4A, 0x6F, 0x6E, 0x65, 0x73});

    fast_ber::encode(absl::MakeSpan(buffer.data(), buffer.size()), type3);
    REQUIRE(absl::MakeSpan(buffer.data(), 9) ==
            std::array<uint8_t, 9>{0xA2, 0x07, 0x43, 0x05, 0x4A, 0x6F, 0x6E, 0x65, 0x73});

    fast_ber::encode(absl::MakeSpan(buffer.data(), buffer.size()), type4);
    REQUIRE(absl::MakeSpan(buffer.data(), 9) ==
            std::array<uint8_t, 9>{0x67, 0x07, 0x43, 0x05, 0x4A, 0x6F, 0x6E, 0x65, 0x73});

    fast_ber::encode(absl::MakeSpan(buffer.data(), buffer.size()), type5);
    REQUIRE(absl::MakeSpan(buffer.data(), 7) == std::array<uint8_t, 7>{0x82, 0x05, 0x4A, 0x6F, 0x6E, 0x65, 0x73});
}
