#include "fast_ber/util/Create.hpp"
#include "fast_ber/util/Extract.hpp"

#include <catch2/catch.hpp>

#include <array>

TEST_CASE("Create: Tag length")
{
    std::array<uint8_t, 10> buffer = {};

    REQUIRE(fast_ber::create_tag(absl::Span<uint8_t>(buffer.begin(), buffer.size()), 0ll) == 1);
    REQUIRE(fast_ber::create_tag(absl::Span<uint8_t>(buffer.begin(), buffer.size()), 30ll) == 1);
    REQUIRE(fast_ber::create_tag(absl::Span<uint8_t>(buffer.begin(), buffer.size()), 31ll) == 2);

    REQUIRE(fast_ber::create_tag(absl::Span<uint8_t>(buffer.begin(), buffer.size()), 0x1ll) == 1);
    REQUIRE(fast_ber::create_tag(absl::Span<uint8_t>(buffer.begin(), buffer.size()), 0x7Fll) == 2);
    REQUIRE(fast_ber::create_tag(absl::Span<uint8_t>(buffer.begin(), buffer.size()), 0x100ll) == 3);
    REQUIRE(fast_ber::create_tag(absl::Span<uint8_t>(buffer.begin(), buffer.size()), 0x10000ll) == 4);
    REQUIRE(fast_ber::create_tag(absl::Span<uint8_t>(buffer.begin(), buffer.size()), 0x1000000ll) == 5);
    REQUIRE(fast_ber::create_tag(absl::Span<uint8_t>(buffer.begin(), buffer.size()), 0x100000000ll) == 6);
    REQUIRE(fast_ber::create_tag(absl::Span<uint8_t>(buffer.begin(), buffer.size()), 0x10000000000ll) == 7);
    REQUIRE(fast_ber::create_tag(absl::Span<uint8_t>(buffer.begin(), buffer.size()), 0x1000000000000ll) == 8);
    REQUIRE(fast_ber::create_tag(absl::Span<uint8_t>(buffer.begin(), buffer.size()), 0x10000000000000ll) == 9);
    REQUIRE(fast_ber::create_tag(absl::Span<uint8_t>(buffer.begin(), buffer.size()), 0x100000000000000ll) == 10);
}

TEST_CASE("Create: Length length")
{
    std::array<uint8_t, 10> buffer = {};

    REQUIRE(fast_ber::create_length(absl::Span<uint8_t>(buffer.begin(), buffer.size()), 0ll) == 1);
    REQUIRE(fast_ber::create_length(absl::Span<uint8_t>(buffer.begin(), buffer.size()), 0x7Fll) == 1);
    REQUIRE(fast_ber::create_length(absl::Span<uint8_t>(buffer.begin(), buffer.size()), 0x80ll) == 2);
    REQUIRE(fast_ber::create_length(absl::Span<uint8_t>(buffer.begin(), buffer.size()), 0xFFll) == 2);

    REQUIRE(fast_ber::create_length(absl::Span<uint8_t>(buffer.begin(), buffer.size()), 0x1ll) == 1);
    REQUIRE(fast_ber::create_length(absl::Span<uint8_t>(buffer.begin(), buffer.size()), 0xF0ll) == 2);
    REQUIRE(fast_ber::create_length(absl::Span<uint8_t>(buffer.begin(), buffer.size()), 0x100ll) == 3);
    REQUIRE(fast_ber::create_length(absl::Span<uint8_t>(buffer.begin(), buffer.size()), 0x10000ll) == 4);
    REQUIRE(fast_ber::create_length(absl::Span<uint8_t>(buffer.begin(), buffer.size()), 0x1000000ll) == 5);
    REQUIRE(fast_ber::create_length(absl::Span<uint8_t>(buffer.begin(), buffer.size()), 0x100000000ll) == 6);
    REQUIRE(fast_ber::create_length(absl::Span<uint8_t>(buffer.begin(), buffer.size()), 0x10000000000ll) == 7);
    REQUIRE(fast_ber::create_length(absl::Span<uint8_t>(buffer.begin(), buffer.size()), 0x1000000000000ll) == 8);
    REQUIRE(fast_ber::create_length(absl::Span<uint8_t>(buffer.begin(), buffer.size()), 0x100000000000000ll) == 9);
}

TEST_CASE("Create: Creating tags")
{
    const auto test_vals = std::initializer_list<fast_ber::Tag>{
        0, 1, 10, 55, 66, 127, 128, 255, 256, 500, 14000, 99244, 382348, 400532434, 99999999999ll};

    for (const int64_t test_val : test_vals)
    {
        std::array<uint8_t, 10> buffer = {};

        fast_ber::Tag tag = 1;
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

        uint64_t length = 0;
        INFO(test_val);

        REQUIRE(fast_ber::create_length(absl::Span<uint8_t>(buffer.begin(), buffer.size()), test_val));
        REQUIRE(fast_ber::extract_length(absl::Span<uint8_t>(buffer.begin(), buffer.size()), length, 0));
        REQUIRE(test_val == length);
    }
}
