#include "fast_ber/ber_types/Identifier.hpp"
#include "fast_ber/ber_types/Integer.hpp"
#include "fast_ber/util/EncodeHelpers.hpp"

#include <catch2/catch.hpp>
#include <limits>

TEST_CASE("Integer: Construction from int")
{
    std::initializer_list<int64_t> test_vals{std::numeric_limits<int64_t>::min(),
                                             std::numeric_limits<int>::min(),
                                             -9999999999,
                                             -27837,
                                             -256,
                                             -255,
                                             -254,
                                             -50,
                                             -20,
                                             -1,
                                             0,
                                             1,
                                             255,
                                             256,
                                             20,
                                             50,
                                             99999,
                                             27836,
                                             27837,
                                             27838,
                                             37699,
                                             999999999,
                                             std::numeric_limits<int>::max(),
                                             std::numeric_limits<int64_t>::max()};

    for (int64_t val : test_vals)
    {
        fast_ber::Integer integer(val);
        REQUIRE(integer == val);
    }
}

TEST_CASE("Integer: Encoding")
{
    fast_ber::Integer        i(100);
    std::array<uint8_t, 100> buffer   = {};
    std::array<uint8_t, 3>   expected = {0x02, 0x01, 0x64};
    size_t                   size     = fast_ber::encode(absl::Span<uint8_t>(buffer.begin(), buffer.size()), i,
                                   fast_ber::ExplicitIdentifier<fast_ber::UniversalTag::integer>{})
                      .length;

    REQUIRE(size == 3);
    REQUIRE(absl::MakeSpan(buffer.data(), 3) == absl::MakeSpan(expected));
}

TEST_CASE("Integer: Assign from raw")
{
    fast_ber::Integer      i(100);
    std::array<uint8_t, 4> test_data = {0x00, 0x02, 0x12, 0x34};

    size_t size = i.assign_ber(absl::MakeSpan(test_data.data(), test_data.size()));
    REQUIRE(size == 4);
    REQUIRE(i == 0x1234);
}

TEST_CASE("Integer: Default value") { REQUIRE(fast_ber::Integer() == 0); }
