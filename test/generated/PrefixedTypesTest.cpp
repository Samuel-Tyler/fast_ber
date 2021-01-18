#include "autogen/prefixed_types.hpp"

#include "fast_ber/util/BerView.hpp"

#include "catch2/catch.hpp"

#include <vector>

TEST_CASE("Prefixed Types: Encoding, matching 8.14 of X.690")
{
    std::array<uint8_t, 100> buffer_storage = {};
    auto                     buffer         = absl::MakeSpan(buffer_storage.data(), buffer_storage.size());

    fast_ber::PrefixedTypes::Type1 type1 = "Jones";
    fast_ber::PrefixedTypes::Type2 type2 = "Jones";
    fast_ber::PrefixedTypes::Type3 type3 = "Jones";
    fast_ber::PrefixedTypes::Type4 type4 = "Jones";
    fast_ber::PrefixedTypes::Type5 type5 = "Jones";
    fast_ber::EncodeResult         res   = {};

    res = fast_ber::encode(buffer, type1);
    REQUIRE(res.success);
    REQUIRE(absl::MakeSpan(buffer.data(), 7) == std::array<uint8_t, 7>{0x1A, 0x05, 0x4A, 0x6F, 0x6E, 0x65, 0x73});

    res = fast_ber::encode(absl::MakeSpan(buffer.data(), buffer.size()), type2);
    REQUIRE(res.success);
    REQUIRE(absl::MakeSpan(buffer.data(), 7) == std::array<uint8_t, 7>{0x43, 0x05, 0x4A, 0x6F, 0x6E, 0x65, 0x73});

    res = fast_ber::encode(absl::MakeSpan(buffer.data(), buffer.size()), type3);
    REQUIRE(res.success);
    REQUIRE(absl::MakeSpan(buffer.data(), 9) ==
            std::array<uint8_t, 9>{0xA2, 0x07, 0x43, 0x05, 0x4A, 0x6F, 0x6E, 0x65, 0x73});

    res = fast_ber::encode(buffer, type4);
    REQUIRE(res.success);
    REQUIRE(absl::MakeSpan(buffer.data(), 9) ==
            std::array<uint8_t, 9>{0x67, 0x07, 0x43, 0x05, 0x4A, 0x6F, 0x6E, 0x65, 0x73});

    res = fast_ber::encode(buffer, type5);
    REQUIRE(res.success);
    REQUIRE(absl::MakeSpan(buffer.data(), 7) == std::array<uint8_t, 7>{0x82, 0x05, 0x4A, 0x6F, 0x6E, 0x65, 0x73});
}

TEST_CASE("Prefixed Types: Prefixed Types in a Collection")
{
    std::array<uint8_t, 100> buffer_storage = {};
    fast_ber::EncodeResult   res            = {};
    auto                     buffer         = absl::MakeSpan(buffer_storage.data(), buffer_storage.size());

    fast_ber::PrefixedTypes::Collection collection;
    collection.type1 = "Jones";
    collection.type2 = "Jones";
    collection.type3 = "Jones";
    collection.type4 = "Jones";
    collection.type5 = "Jones";

    res = fast_ber::encode(buffer, collection);
    REQUIRE(res.success);
    fast_ber::BerViewIterator itr = BerView(buffer).begin();
    REQUIRE(itr->ber_data() == std::array<uint8_t, 7>{0x1A, 0x05, 0x4A, 0x6F, 0x6E, 0x65, 0x73});
    ++itr;
    REQUIRE(itr->ber_data() == std::array<uint8_t, 7>{0x43, 0x05, 0x4A, 0x6F, 0x6E, 0x65, 0x73});
    ++itr;
    REQUIRE(itr->ber_data() == std::array<uint8_t, 9>{0xA2, 0x07, 0x43, 0x05, 0x4A, 0x6F, 0x6E, 0x65, 0x73});
    ++itr;
    REQUIRE(itr->ber_data() == std::array<uint8_t, 9>{0x67, 0x07, 0x43, 0x05, 0x4A, 0x6F, 0x6E, 0x65, 0x73});
    ++itr;
    REQUIRE(itr->ber_data() == std::array<uint8_t, 7>{0x82, 0x05, 0x4A, 0x6F, 0x6E, 0x65, 0x73});
}
