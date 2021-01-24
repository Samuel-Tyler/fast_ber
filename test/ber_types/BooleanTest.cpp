#include "fast_ber/ber_types/Boolean.hpp"
#include "fast_ber/ber_types/Identifier.hpp"
#include "fast_ber/util/DecodeHelpers.hpp"
#include "fast_ber/util/EncodeHelpers.hpp"

#include <catch2/catch.hpp>

TEST_CASE("Boolean: Construction from bool")
{
    REQUIRE(fast_ber::Boolean<>(false).value() == false);
    REQUIRE(fast_ber::Boolean<>(true).value() == true);
    REQUIRE(fast_ber::Boolean<>(false) == false);
    REQUIRE(fast_ber::Boolean<>(true) == true);
}

TEST_CASE("Boolean: Assignment")
{
    fast_ber::Boolean<> test;
    test = true;
    REQUIRE(test == true);
    test = false;
    REQUIRE(test == false);
}

TEST_CASE("Boolean: Encoding false")
{
    fast_ber::Boolean<>      test(true);
    std::array<uint8_t, 100> buffer   = {};
    std::array<uint8_t, 3>   expected = {0x01, 0x01, 0xFF};

    size_t size = test.encode(absl::MakeSpan(buffer.data(), buffer.size())).length;

    REQUIRE(size == 3);
    REQUIRE(absl::MakeSpan(buffer.data(), 3) == absl::MakeSpan(expected));
}

TEST_CASE("Boolean: Encoding true")
{
    fast_ber::Boolean<>      test(false);
    std::array<uint8_t, 100> buffer   = {};
    std::array<uint8_t, 3>   expected = {0x01, 0x01, 0x00};

    size_t size = test.encode(absl::MakeSpan(buffer.data(), buffer.size())).length;
    REQUIRE(size == 3);
    REQUIRE(absl::MakeSpan(buffer.data(), 3) == absl::MakeSpan(expected));
}

TEST_CASE("Boolean: Tagged")
{
    using TestId = fast_ber::DoubleId<fast_ber::Id<fast_ber::Class::context_specific, 1000>,
                                      fast_ber::Id<fast_ber::Class::private_, 9999999>>;
    fast_ber::Boolean<TestId> test(false);
    std::array<uint8_t, 100>  buffer = {};

    test.encode(absl::MakeSpan(buffer.data(), buffer.size()));
    CHECK(fast_ber::has_correct_header(fast_ber::BerView(buffer), TestId{}, fast_ber::Construction::primitive));
}
