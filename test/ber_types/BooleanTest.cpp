#include "fast_ber/ber_types/Boolean.hpp"

#include <catch2/catch.hpp>

TEST_CASE("Boolean: Construction from bool")
{
    REQUIRE(fast_ber::Boolean(false).value() == false);
    REQUIRE(fast_ber::Boolean(true).value() == true);
    REQUIRE(fast_ber::Boolean(false) == false);
    REQUIRE(fast_ber::Boolean(true) == true);
}

TEST_CASE("Boolean: Assignment")
{
    fast_ber::Boolean test;
    test = true;
    REQUIRE(test == true);
    test = false;
    REQUIRE(test == false);
}

TEST_CASE("Boolean: Encoding false")
{
    fast_ber::Boolean        test(true);
    std::array<uint8_t, 100> buffer;
    std::array<uint8_t, 3>   expected = {0x80, 0x01, 0x01};

    size_t size = test.encode_with_new_id(absl::Span<uint8_t>(buffer.begin(), buffer.size()),
                                          fast_ber::Class::context_specific, 0);

    REQUIRE(size == 3);
    REQUIRE(absl::MakeSpan(buffer.data(), 3) == absl::MakeSpan(expected));
}

TEST_CASE("Boolean: Encoding true")
{
    fast_ber::Boolean        test(false);
    std::array<uint8_t, 100> buffer;
    std::array<uint8_t, 3>   expected = {0x80, 0x01, 0x00};

    size_t size = test.encode_with_new_id(absl::Span<uint8_t>(buffer.begin(), buffer.size()),
                                          fast_ber::Class::context_specific, 0);
    REQUIRE(size == 3);
    REQUIRE(absl::MakeSpan(buffer.data(), 3) == absl::MakeSpan(expected));
}
