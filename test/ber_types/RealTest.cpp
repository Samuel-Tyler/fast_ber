#include "fast_ber/ber_types/Real.hpp"

#include "absl/types/optional.h"

#include <catch2/catch.hpp>

#include <limits>
#include <sstream>

TEST_CASE("Real: Construction from double")
{
    std::initializer_list<double> test_vals{0.0,
                                            -0.0,
                                            std::numeric_limits<double>::infinity(),
                                            -std::numeric_limits<double>::infinity(),
                                            -9999999999.99999,
                                            -27837.48,
                                            -256.0,
                                            -255.0,
                                            -254.0,
                                            -50.0,
                                            -20.434,
                                            -1.0,
                                            0,
                                            1.0,
                                            255.022,
                                            256.2343,
                                            20.888,
                                            50.00,
                                            99999.9999,
                                            27836.1232312,
                                            27837.43645454,
                                            27838.34324,
                                            37699.34234,
                                            999999999.9999};

    for (double val : test_vals)
    {
        fast_ber::Real<>                                                   real1(val);
        fast_ber::Real<fast_ber::Id<fast_ber::Class::context_specific, 2>> real2(real1);
        fast_ber::Real<fast_ber::Id<fast_ber::Class::context_specific, 4>> real3;
        absl::optional<fast_ber::Real<>>                                   real4;
        absl::optional<fast_ber::Real<>>                                   real5(real1);

        real3 = real2;
        real4 = real3;

        REQUIRE(real1 == val);
        REQUIRE(real2 == val);
        REQUIRE(real3 == val);
        REQUIRE(real4 == val);
        REQUIRE(real4 == val);
    }
}

struct TestData
{
    std::vector<uint8_t> data;
    double               value;
};

TEST_CASE("Real: Decoding")
{
    std::vector<TestData> test_data = {{{0x09, 0x00}, 0.0},
                                       {{0x09, 0x08, 0x03, 0x31, 0x32, 0x33, 0x2e, 0x45, 0x2d, 0x33}, 0.123},
                                       {{0x09, 0x01, 0x40}, std::numeric_limits<double>::infinity()},
                                       {{0x09, 0x01, 0x41}, -std::numeric_limits<double>::infinity()},
                                       {{0x09, 0x01, 0x43}, -0.0},
                                       {{0x09, 0x05, 0x01, '0', '0', '0', '0'}, 0},
                                       {{0x09, 0x05, 0x01, '2', '0', '9', '4'}, 2094},
                                       {{0x09, 0x06, 0x01, '-', '2', '0', '9', '4'}, -2094},
                                       {{0x09, 0x06, 0x01, '+', '4', '3', '2', '1'}, 4321},
                                       {{0x09, 0x07, 0x02, '3', '2', '1', ',', '5', '4'}, 321.54},
                                       {{0x09, 0x08, 0x03, '+', '6', '.', '5', 'e', '3', '0'}, 6.5e30}};

    for (TestData test : test_data)
    {
        fast_ber::Real<>       r;
        fast_ber::DecodeResult res = fast_ber::decode(test.data, r);

        REQUIRE(res.success);
        REQUIRE(r == test.value);
    }
}

TEST_CASE("Real: Default value") { REQUIRE(fast_ber::Real<>() == 0); }

TEST_CASE("Real: Tagging")
{
    using Tag                = fast_ber::Id<fast_ber::Class::application, 2>;
    using DefaultTag         = fast_ber::ExplicitId<fast_ber::UniversalTag::real>;
    using ExplicitTag        = fast_ber::DoubleId<fast_ber::Id<fast_ber::Class::application, 2>,
                                           fast_ber::ExplicitId<fast_ber::UniversalTag::real>>;
    using TaggedReal         = fast_ber::Real<Tag>;
    using ExplicitTaggedReal = fast_ber::Real<ExplicitTag>;

    static_assert(std::is_same<fast_ber::Identifier<fast_ber::Real<>>, DefaultTag>::value, "Tagged Real");
    static_assert(std::is_same<fast_ber::Identifier<TaggedReal>, Tag>::value, "Tagged Real");
    static_assert(std::is_same<fast_ber::Identifier<ExplicitTaggedReal>, ExplicitTag>::value, "Tagged Real");
}

TEST_CASE("Default: Serialize")
{
    std::stringstream ss;
    fast_ber::Real<>  r = -16;

    ss << r;
    REQUIRE(ss.str() == "0.5");
}
