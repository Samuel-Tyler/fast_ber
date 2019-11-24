#include "autogen/real_schema.hpp"

#include "catch2/catch.hpp"

#include <array>

const fast_ber::SGSN_2009A_CDR::SGSNPDPRecord<> test_record = {
    10,
    "123",
    "12345678",
    {}, // fast_ber::IA5String<fast_ber::Id<fast_ber::Class::context_specific, 3>>{"192.168.0.1"},
    "12345678",
    "0",
    "AA",
    "BB",
    10,
    fast_ber::IA5String<fast_ber::Id<fast_ber::Class::context_specific, 3>>{"192.168.0.1"},
    fast_ber::empty,
    "CC",
    "D",
    fast_ber::SequenceOf<fast_ber::SGSN_2009A_CDR::ChangeOfCharCondition<>>{
        fast_ber::SGSN_2009A_CDR::ChangeOfCharCondition<>{}, fast_ber::SGSN_2009A_CDR::ChangeOfCharCondition<>{}},
    "EE",
    123,
    true,
    1,
    {}, // fast_ber::Integer<fast_ber::Id<fast_ber::Class::context_specific, 0>>(2),
    88888888888,
    "",
    fast_ber::SGSN_2009A_CDR::ManagementExtensions<>{{{1, 2, 3, 4}, true, {{}}}},
    100,
    fast_ber::SGSN_2009A_CDR::APNSelectionMode<>::Values::mSorNetworkProvidedSubscriptionVerified,
    {},
    {},
    {},
    100,
    fast_ber::SGSN_2009A_CDR::CAMELInformationPDP<>{{}, {}, {}, {}, {}, {}, {}, {}, {}},
    fast_ber::SGSN_2009A_CDR::ChChSelectionMode<>::Values::roamingDefault,
    {},
    {},
    {}};

TEST_CASE("RealSchema: Testing a standard asn schema")
{
    std::array<uint8_t, 5000> buffer = {};

    fast_ber::SGSN_2009A_CDR::CallEventRecord<>      record;
    fast_ber::SGSN_2009A_CDR::SGSNSMTRecord<>        smtr;
    fast_ber::SGSN_2009A_CDR::CAMELInformationPDP<>  pdp;
    fast_ber::SGSN_2009A_CDR::ManagementExtensions<> e;

    record = test_record;

    REQUIRE(fast_ber::encode(absl::Span<uint8_t>(buffer), smtr).success);
    REQUIRE(fast_ber::decode(absl::Span<uint8_t>(buffer), smtr).success);

    REQUIRE(fast_ber::encode(absl::Span<uint8_t>(buffer), pdp).success);
    REQUIRE(fast_ber::decode(absl::Span<uint8_t>(buffer), pdp).success);

    REQUIRE(fast_ber::encode(absl::Span<uint8_t>(buffer), e).success);
    REQUIRE(fast_ber::decode(absl::Span<uint8_t>(buffer), e).success);

    REQUIRE(fast_ber::encode(absl::Span<uint8_t>(buffer), record).success);
    REQUIRE(fast_ber::decode(absl::Span<uint8_t>(buffer), record).success);

    std::cout << "fast_ber::SGSN_2009A_CDR::SGSNPDPRecord: " << record << "\n\n";
}
