#include "fast_ber/util/Create.hpp"
#include "fast_ber/util/Extract.hpp"

#include <catch2/catch.hpp>

#include <array>

TEST_CASE("Create: Tag length")
{
    std::array<uint8_t, 10> buffer = {};

    REQUIRE(fast_ber::create_tag(absl::Span<uint8_t>(buffer.begin(), buffer.size()), 0) == 1);
    REQUIRE(fast_ber::create_tag(absl::Span<uint8_t>(buffer.begin(), buffer.size()), 30) == 1);
    REQUIRE(fast_ber::create_tag(absl::Span<uint8_t>(buffer.begin(), buffer.size()), 31) == 2);

    REQUIRE(fast_ber::create_tag(absl::Span<uint8_t>(buffer.begin(), buffer.size()), 0x1) == 1);
    REQUIRE(fast_ber::create_tag(absl::Span<uint8_t>(buffer.begin(), buffer.size()), 0x7F) == 2);
    REQUIRE(fast_ber::create_tag(absl::Span<uint8_t>(buffer.begin(), buffer.size()), 0x100) == 3);
    REQUIRE(fast_ber::create_tag(absl::Span<uint8_t>(buffer.begin(), buffer.size()), 0x10000) == 4);
    REQUIRE(fast_ber::create_tag(absl::Span<uint8_t>(buffer.begin(), buffer.size()), 0x1000000) == 5);
    REQUIRE(fast_ber::create_tag(absl::Span<uint8_t>(buffer.begin(), buffer.size()), 0x100000000) == 6);
    REQUIRE(fast_ber::create_tag(absl::Span<uint8_t>(buffer.begin(), buffer.size()), 0x10000000000) == 7);
    REQUIRE(fast_ber::create_tag(absl::Span<uint8_t>(buffer.begin(), buffer.size()), 0x1000000000000) == 8);
    REQUIRE(fast_ber::create_tag(absl::Span<uint8_t>(buffer.begin(), buffer.size()), 0x10000000000000) == 9);
    REQUIRE(fast_ber::create_tag(absl::Span<uint8_t>(buffer.begin(), buffer.size()), 0x100000000000000) == 10);
}

TEST_CASE("Create: Length length")
{
    std::array<uint8_t, 10> buffer = {};

    REQUIRE(fast_ber::create_length(absl::Span<uint8_t>(buffer.begin(), buffer.size()), 0) == 1);
    REQUIRE(fast_ber::create_length(absl::Span<uint8_t>(buffer.begin(), buffer.size()), 0x7F) == 1);
    REQUIRE(fast_ber::create_length(absl::Span<uint8_t>(buffer.begin(), buffer.size()), 0x80) == 2);
    REQUIRE(fast_ber::create_length(absl::Span<uint8_t>(buffer.begin(), buffer.size()), 0xFF) == 2);

    REQUIRE(fast_ber::create_length(absl::Span<uint8_t>(buffer.begin(), buffer.size()), 0x1) == 1);
    REQUIRE(fast_ber::create_length(absl::Span<uint8_t>(buffer.begin(), buffer.size()), 0xF0) == 2);
    REQUIRE(fast_ber::create_length(absl::Span<uint8_t>(buffer.begin(), buffer.size()), 0x100) == 3);
    REQUIRE(fast_ber::create_length(absl::Span<uint8_t>(buffer.begin(), buffer.size()), 0x10000) == 4);
    REQUIRE(fast_ber::create_length(absl::Span<uint8_t>(buffer.begin(), buffer.size()), 0x1000000) == 5);
    REQUIRE(fast_ber::create_length(absl::Span<uint8_t>(buffer.begin(), buffer.size()), 0x100000000) == 6);
    REQUIRE(fast_ber::create_length(absl::Span<uint8_t>(buffer.begin(), buffer.size()), 0x10000000000) == 7);
    REQUIRE(fast_ber::create_length(absl::Span<uint8_t>(buffer.begin(), buffer.size()), 0x1000000000000) == 8);
    REQUIRE(fast_ber::create_length(absl::Span<uint8_t>(buffer.begin(), buffer.size()), 0x100000000000000) == 9);
}

TEST_CASE("Create: Creating tags")
{
    const auto test_vals = std::initializer_list<long>{0,   1,   10,    55,    66,     127,       128,        255,
                                                       256, 500, 14000, 99244, 382348, 400532434, 99999999999};

    for (const long test_val : test_vals)
    {
        std::array<uint8_t, 10> buffer = {};

        fast_ber::Tag tag;
        INFO(test_val);

        REQUIRE(fast_ber::create_tag(absl::Span<uint8_t>(buffer.begin(), buffer.size()), test_val));
        REQUIRE(fast_ber::extract_tag(absl::Span<uint8_t>(buffer.begin(), buffer.size()), tag));
        REQUIRE(test_val == tag);
    }
}

TEST_CASE("Create: Creating lengths")
{
    const auto test_vals = std::initializer_list<uint64_t>{0,   1,   10,    55,    66,     127,       128,        255,
                                                           256, 500, 14000, 99244, 382348, 400532434, 99999999999};

    for (const uint64_t test_val : test_vals)
    {
        std::array<uint8_t, 10> buffer = {};

        uint64_t length;
        INFO(test_val);

        REQUIRE(fast_ber::create_length(absl::Span<uint8_t>(buffer.begin(), buffer.size()), test_val));
        REQUIRE(fast_ber::extract_length(absl::Span<uint8_t>(buffer.begin(), buffer.size()), length, 0));
        REQUIRE(test_val == length);
    }
}
