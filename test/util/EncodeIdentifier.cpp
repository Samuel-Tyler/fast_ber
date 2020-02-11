#include "fast_ber/util/EncodeIdentifiers.hpp"
#include "fast_ber/util/Extract.hpp"

#include <catch2/catch.hpp>

#include <array>

TEST_CASE("EncodeIdentifiers: Tag length")
{
    std::array<uint8_t, 10> buffer = {};

    REQUIRE(fast_ber::encode_tag(absl::Span<uint8_t>(buffer.data(), buffer.size()), 0ll) == 1);
    REQUIRE(fast_ber::encode_tag(absl::Span<uint8_t>(buffer.data(), buffer.size()), 30ll) == 1);
    REQUIRE(fast_ber::encode_tag(absl::Span<uint8_t>(buffer.data(), buffer.size()), 31ll) == 2);

    REQUIRE(fast_ber::encode_tag(absl::Span<uint8_t>(buffer.data(), buffer.size()), 0x1ll) == 1);
    REQUIRE(fast_ber::encode_tag(absl::Span<uint8_t>(buffer.data(), buffer.size()), 0x7Fll) == 2);
    REQUIRE(fast_ber::encode_tag(absl::Span<uint8_t>(buffer.data(), buffer.size()), 0x100ll) == 3);
    REQUIRE(fast_ber::encode_tag(absl::Span<uint8_t>(buffer.data(), buffer.size()), 0x10000ll) == 4);
    REQUIRE(fast_ber::encode_tag(absl::Span<uint8_t>(buffer.data(), buffer.size()), 0x1000000ll) == 5);
    REQUIRE(fast_ber::encode_tag(absl::Span<uint8_t>(buffer.data(), buffer.size()), 0x100000000ll) == 6);
    REQUIRE(fast_ber::encode_tag(absl::Span<uint8_t>(buffer.data(), buffer.size()), 0x10000000000ll) == 7);
    REQUIRE(fast_ber::encode_tag(absl::Span<uint8_t>(buffer.data(), buffer.size()), 0x1000000000000ll) == 8);
    REQUIRE(fast_ber::encode_tag(absl::Span<uint8_t>(buffer.data(), buffer.size()), 0x10000000000000ll) == 9);
    REQUIRE(fast_ber::encode_tag(absl::Span<uint8_t>(buffer.data(), buffer.size()), 0x100000000000000ll) == 10);
}

TEST_CASE("EncodeIdentifiers: Length length")
{
    std::array<uint8_t, 10> buffer = {};

    REQUIRE(fast_ber::encode_length(absl::Span<uint8_t>(buffer.data(), buffer.size()), 0ll) == 1);
    REQUIRE(fast_ber::encode_length(absl::Span<uint8_t>(buffer.data(), buffer.size()), 0x7Fll) == 1);
    REQUIRE(fast_ber::encode_length(absl::Span<uint8_t>(buffer.data(), buffer.size()), 0x80ll) == 2);
    REQUIRE(fast_ber::encode_length(absl::Span<uint8_t>(buffer.data(), buffer.size()), 0xFFll) == 2);

    REQUIRE(fast_ber::encode_length(absl::Span<uint8_t>(buffer.data(), buffer.size()), 0x1ll) == 1);
    REQUIRE(fast_ber::encode_length(absl::Span<uint8_t>(buffer.data(), buffer.size()), 0xF0ll) == 2);
    REQUIRE(fast_ber::encode_length(absl::Span<uint8_t>(buffer.data(), buffer.size()), 0x100ll) == 3);
    REQUIRE(fast_ber::encode_length(absl::Span<uint8_t>(buffer.data(), buffer.size()), 0x10000ll) == 4);
    REQUIRE(fast_ber::encode_length(absl::Span<uint8_t>(buffer.data(), buffer.size()), 0x1000000ll) == 5);
    REQUIRE(fast_ber::encode_length(absl::Span<uint8_t>(buffer.data(), buffer.size()), 0x100000000ll) == 6);
    REQUIRE(fast_ber::encode_length(absl::Span<uint8_t>(buffer.data(), buffer.size()), 0x10000000000ll) == 7);
    REQUIRE(fast_ber::encode_length(absl::Span<uint8_t>(buffer.data(), buffer.size()), 0x1000000000000ll) == 8);
    REQUIRE(fast_ber::encode_length(absl::Span<uint8_t>(buffer.data(), buffer.size()), 0x100000000000000ll) == 9);
}

TEST_CASE("EncodeIdentifiers: Creating tags")
{
    const std::array<fast_ber::Tag, 16> test_vals = {
        0,   1,   10,    55,    66,     127,       128,           255,
        256, 500, 14000, 99244, 382348, 400532434, 99999999999ll, std::numeric_limits<fast_ber::Tag>::max()};

    for (const fast_ber::Tag test_val : test_vals)
    {
        std::array<uint8_t, 10> buffer = {};

        fast_ber::Tag tag = 1;
        INFO(test_val);

        size_t expected_length = fast_ber::encoded_tag_length(test_val);
        size_t actual_length   = fast_ber::encode_tag(absl::Span<uint8_t>(buffer.data(), buffer.size()), test_val);
        REQUIRE(expected_length == actual_length);
        REQUIRE(fast_ber::extract_tag(absl::Span<uint8_t>(buffer.data(), buffer.size()), tag));
        REQUIRE(test_val == tag);
    }
}

TEST_CASE("EncodeIdentifiers: Creating lengths")
{
    const std::array<size_t, 16> test_vals = {
        0,   1,   10,    55,    66,     127,       128,         255,
        256, 500, 14000, 99244, 382348, 400532434, 99999999999, std::numeric_limits<size_t>::max()};

    for (const size_t test_val : test_vals)
    {
        std::array<uint8_t, 10> buffer = {};

        size_t length = 0;
        INFO(test_val);

        REQUIRE(fast_ber::encode_length(absl::Span<uint8_t>(buffer.data(), buffer.size()), test_val));
        REQUIRE(fast_ber::extract_length(absl::Span<uint8_t>(buffer.data(), buffer.size()), length, 0));
        REQUIRE(test_val == length);
    }
}

TEST_CASE("EncodeIdentifiers: Tags - round trip")
{
    std::array<uint8_t, 100> buffer;
    for (fast_ber::Tag i = 0; i < 10000; i++)
    {
        fast_ber::Tag out = -1;

        size_t length = fast_ber::encoded_tag_length(i);
        REQUIRE(fast_ber::encode_tag(absl::Span<uint8_t>(buffer), i) == length);
        REQUIRE(fast_ber::extract_tag(buffer, out) == length);
        REQUIRE(out == i);
    }
}
