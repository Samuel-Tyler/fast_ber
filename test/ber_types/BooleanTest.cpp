#include "fast_ber/ber_types/Boolean.hpp"
#include "fast_ber/ber_types/Identifier.hpp"
#include "fast_ber/util/EncodeHelpers.hpp"

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
    std::array<uint8_t, 100> buffer   = {};
    std::array<uint8_t, 3>   expected = {0x01, 0x01, 0x01};

    size_t size = encode_with_specific_id(absl::Span<uint8_t>(buffer.begin(), buffer.size()), test,
                                          fast_ber::ExplicitIdentifier<fast_ber::UniversalTag::boolean>{})
                      .length;

    REQUIRE(size == 3);
    REQUIRE(absl::MakeSpan(buffer.data(), 3) == absl::MakeSpan(expected));
}

TEST_CASE("Boolean: Encoding true")
{
    fast_ber::Boolean        test(false);
    std::array<uint8_t, 100> buffer   = {};
    std::array<uint8_t, 3>   expected = {0x01, 0x01, 0x00};

    size_t size = encode_with_specific_id(absl::Span<uint8_t>(buffer.begin(), buffer.size()), test,
                                          fast_ber::ExplicitIdentifier<fast_ber::UniversalTag::boolean>{})
                      .length;
    REQUIRE(size == 3);
    REQUIRE(absl::MakeSpan(buffer.data(), 3) == absl::MakeSpan(expected));
}
