#include "fast_ber/ber_types/Identifier.hpp"
#include "fast_ber/ber_types/Integer.hpp"
#include "fast_ber/util/EncodeHelpers.hpp"

#include "absl/types/optional.h"

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
        fast_ber::Integer<>                                                                   integer1(val);
        fast_ber::Integer<fast_ber::ImplicitIdentifier<fast_ber::Class::context_specific, 2>> integer2(integer1);
        fast_ber::Integer<fast_ber::ImplicitIdentifier<fast_ber::Class::context_specific, 4>> integer3;
        absl::optional<fast_ber::Integer<>>                                                   integer4;
        absl::optional<fast_ber::Integer<>>                                                   integer5(integer1);

        integer3 = integer2;
        integer4 = integer3;

        REQUIRE(integer1 == val);
        REQUIRE(integer2 == val);
        REQUIRE(integer3 == val);
        REQUIRE(integer4 == val);
        REQUIRE(integer5 == val);
    }
}

TEST_CASE("Integer: Encoding")
{
    fast_ber::Integer<>      i(100);
    std::array<uint8_t, 100> buffer   = {};
    std::array<uint8_t, 3>   expected = {0x02, 0x01, 0x64};
    size_t                   size     = fast_ber::encode(absl::Span<uint8_t>(buffer.data(), buffer.size()), i,
                                   fast_ber::ExplicitIdentifier<fast_ber::UniversalTag::integer>{})
                      .length;

    REQUIRE(size == 3);
    REQUIRE(absl::MakeSpan(buffer.data(), 3) == absl::MakeSpan(expected));
}

TEST_CASE("Integer: Assign from raw")
{
    fast_ber::Integer<>    i(100);
    std::array<uint8_t, 4> test_data = {0x00, 0x02, 0x12, 0x34};

    size_t size = i.assign_ber(absl::MakeSpan(test_data.data(), test_data.size()));
    REQUIRE(size == 4);
    REQUIRE(i == 0x1234);
}

TEST_CASE("Integer: Default value") { REQUIRE(fast_ber::Integer<>() == 0); }

TEST_CASE("Integer: Tagging")
{
    using Tag         = fast_ber::ImplicitIdentifier<fast_ber::Class::application, 2>;
    using DefaultTag  = fast_ber::ExplicitIdentifier<fast_ber::UniversalTag::integer>;
    using ExplicitTag = fast_ber::TaggedExplicitIdentifier<fast_ber::Class::application, 2, fast_ber::DefaultTagging>;
    using FullExplicitTag   = fast_ber::TaggedExplicitIdentifier<fast_ber::Class::application, 2, DefaultTag>;
    using TaggedInt         = fast_ber::Integer<Tag>;
    using ExplicitTaggedInt = fast_ber::Integer<ExplicitTag>;

    static_assert(std::is_same<decltype(fast_ber::identifier(static_cast<TaggedInt*>(nullptr))), Tag>::value,
                  "Tagged Integer");

    static_assert(
        std::is_same<decltype(fast_ber::explicit_identifier(static_cast<TaggedInt*>(nullptr))), DefaultTag>::value,
        "Tagged Identifier");

    static_assert(
        std::is_same<decltype(fast_ber::identifier(static_cast<ExplicitTaggedInt*>(nullptr))), FullExplicitTag>::value,
        "Tagged Identifier");

    static_assert(std::is_same<decltype(fast_ber::explicit_identifier(static_cast<ExplicitTaggedInt*>(nullptr))),
                               DefaultTag>::value,
                  "Tagged Identifier");
}
