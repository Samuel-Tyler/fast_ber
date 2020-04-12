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
        fast_ber::Integer<>                                                   integer1(val);
        fast_ber::Integer<fast_ber::Id<fast_ber::Class::context_specific, 2>> integer2(integer1);
        fast_ber::Integer<fast_ber::Id<fast_ber::Class::context_specific, 4>> integer3;
        absl::optional<fast_ber::Integer<>>                                   integer4;
        absl::optional<fast_ber::Integer<>>                                   integer5(integer1);

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
    std::array<uint8_t, 100> buffer       = {};
    std::array<uint8_t, 3>   expected     = {0x02, 0x01, 0x64};
    size_t                   encoded_size = fast_ber::encoded_length(i);
    size_t                   size = fast_ber::encode(absl::Span<uint8_t>(buffer.data(), buffer.size()), i).length;

    REQUIRE(size == 3);
    REQUIRE(encoded_size == 3);
    REQUIRE(absl::MakeSpan(buffer.data(), 3) == absl::MakeSpan(expected));
}

TEST_CASE("Integer: Assign from raw")
{
    fast_ber::Integer<>    i(100);
    std::array<uint8_t, 4> test_data = {0x02, 0x02, 0x12, 0x34};

    fast_ber::DecodeResult res = i.decode(fast_ber::BerView(absl::MakeSpan(test_data.data(), test_data.size())));
    REQUIRE(res.success);
    REQUIRE(i == 0x1234);
}

TEST_CASE("Integer: Default value") { REQUIRE(fast_ber::Integer<>() == 0); }

TEST_CASE("Integer: Tagging")
{
    using Tag               = fast_ber::Id<fast_ber::Class::application, 2>;
    using DefaultTag        = fast_ber::ExplicitId<fast_ber::UniversalTag::integer>;
    using ExplicitTag       = fast_ber::DoubleId<fast_ber::Id<fast_ber::Class::application, 2>,
                                           fast_ber::ExplicitId<fast_ber::UniversalTag::integer>>;
    using TaggedInt         = fast_ber::Integer<Tag>;
    using ExplicitTaggedInt = fast_ber::Integer<ExplicitTag>;

    static_assert(std::is_same<fast_ber::Identifier<fast_ber::Integer<>>, DefaultTag>::value, "Tagged Integer");
    static_assert(std::is_same<fast_ber::Identifier<TaggedInt>, Tag>::value, "Tagged Integer");
    static_assert(std::is_same<fast_ber::Identifier<ExplicitTaggedInt>, ExplicitTag>::value, "Tagged Identifier");
}

TEST_CASE("Integer: Encode Decode")
{
    using Identifier = fast_ber::DoubleId<fast_ber::Id<fast_ber::Class::application, 2>,
                                          fast_ber::ExplicitId<fast_ber::UniversalTag::integer>>;

    fast_ber::Integer<Identifier> i1 = -100;
    fast_ber::Integer<Identifier> i2 = +100;

    std::vector<uint8_t> buffer(100, 0x00);

    bool enc_success = fast_ber::encode(absl::Span<uint8_t>(buffer), i1).success;
    bool dec_success = fast_ber::decode(absl::Span<uint8_t>(buffer), i2).success;

    REQUIRE(enc_success);
    REQUIRE(dec_success);
    REQUIRE(i1 == i2);
}
