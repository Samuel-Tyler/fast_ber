#include "autogen/real_schema.hpp"

#include "catch2/catch.hpp"

#include <array>

const fast_ber::SGSN_2009A_CDR::SGSNPDPRecord test_record = {
    10,
    "123",
    "12345678",
    fast_ber::IA5String<fast_ber::Id<fast_ber::Class::context_specific, 2>>{"192.168.0.1"},
    "12345678",
    "0",
    "AA",
    "BB",
    10,
    fast_ber::IA5String<fast_ber::Id<fast_ber::Class::context_specific, 2>>{"192.168.0.1"},
    fast_ber::empty,
    "CC",
    "D",
    fast_ber::SGSN_2009A_CDR::SGSNPDPRecord::ListOfTrafficVolumes{
        fast_ber::SGSN_2009A_CDR::ChangeOfCharCondition{{}, {}, {}, {}, {}, "24:00:00T"},
        fast_ber::SGSN_2009A_CDR::ChangeOfCharCondition{{}, {}, {}, {}, {}, "24:00:00T"}},
    "24:00:00T",
    123,
    true,
    1,
    fast_ber::Integer<fast_ber::Id<fast_ber::Class::context_specific, 0>>(2),
    88888888888,
    "NodeID",
    fast_ber::SGSN_2009A_CDR::ManagementExtensions{fast_ber::SGSN_2009A_CDR::ManagementExtension{
        {1, 2, 3, 4}, true, fast_ber::SGSN_2009A_CDR::GprsSCdrExtensions{{}}}},
    100,
    fast_ber::SGSN_2009A_CDR::APNSelectionMode::Values::mSorNetworkProvidedSubscriptionVerified,
    {},
    {},
    "XX",
    100,
    fast_ber::SGSN_2009A_CDR::CAMELInformationPDP{{}, {}, {}, {}, {}, {}, {}, {}, {}},
    fast_ber::SGSN_2009A_CDR::ChChSelectionMode::Values::roamingDefault,
    {},
    {},
    {}};

static const std::array<uint8_t, 203> test_record_expected_encoding = {
    0xb4, 0x81, 0xc8, 0x80, 0x01, 0x0a, 0x83, 0x03, 0x31, 0x32, 0x33, 0x84, 0x08, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36,
    0x37, 0x38, 0xa5, 0x0d, 0x82, 0x0b, 0x31, 0x39, 0x32, 0x2e, 0x31, 0x36, 0x38, 0x2e, 0x30, 0x2e, 0x31, 0x86, 0x08,
    0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x87, 0x01, 0x30, 0x88, 0x02, 0x41, 0x41, 0x89, 0x02, 0x42, 0x42,
    0x8a, 0x01, 0x0a, 0xab, 0x0d, 0x82, 0x0b, 0x31, 0x39, 0x32, 0x2e, 0x31, 0x36, 0x38, 0x2e, 0x30, 0x2e, 0x31, 0x8d,
    0x02, 0x43, 0x43, 0xae, 0x03, 0x81, 0x01, 0x44, 0xaf, 0x2c, 0x30, 0x14, 0x83, 0x01, 0x00, 0x84, 0x01, 0x00, 0x85,
    0x01, 0x00, 0x86, 0x09, 0x32, 0x34, 0x3a, 0x30, 0x30, 0x3a, 0x30, 0x30, 0x54, 0x30, 0x14, 0x83, 0x01, 0x00, 0x84,
    0x01, 0x00, 0x85, 0x01, 0x00, 0x86, 0x09, 0x32, 0x34, 0x3a, 0x30, 0x30, 0x3a, 0x30, 0x30, 0x54, 0x90, 0x09, 0x32,
    0x34, 0x3a, 0x30, 0x30, 0x3a, 0x30, 0x30, 0x54, 0x91, 0x01, 0x7b, 0x92, 0x01, 0xff, 0x93, 0x01, 0x01, 0xb4, 0x03,
    0x80, 0x01, 0x02, 0x95, 0x05, 0x14, 0xb2, 0x30, 0xce, 0x38, 0x96, 0x06, 0x4e, 0x6f, 0x64, 0x65, 0x49, 0x44, 0xb7,
    0x0c, 0x30, 0x0a, 0x06, 0x03, 0x2a, 0x03, 0x04, 0x81, 0x01, 0xff, 0xa2, 0x00, 0x98, 0x01, 0x64, 0x99, 0x01, 0x00,
    0x9c, 0x02, 0x58, 0x58, 0x9d, 0x01, 0x64, 0xbe, 0x00, 0x9f, 0x20, 0x01, 0x04};

TEST_CASE("RealSchema: Default Construct")
{
    fast_ber::SGSN_2009A_CDR::SGSNPDPRecord record;

    CHECK(record.recordType == 0);  // Should actaully be 18 due to constraints
    CHECK(record.servedIMSI == ""); // Should actaully be "   " due to constraints
    CHECK(!record.servedIMEI.has_value());
    CHECK(!record.sgsnAddress.has_value());
    CHECK(!record.msNetworkCapability.has_value());
    CHECK(!record.routingArea.has_value());
    CHECK(!record.locationAreaCode.has_value());
    CHECK(!record.cellIdentifier.has_value());
    CHECK(record.chargingID == 0);
    CHECK(record.ggsnAddressUsed == fast_ber::SGSN_2009A_CDR::IPBinaryAddress());
    CHECK(!record.accessPointNameNI.has_value());
    CHECK(!record.pdpType.has_value());
    CHECK(!record.servedPDPAddress.has_value());
    CHECK(!record.listOfTrafficVolumes.has_value());
    CHECK(record.recordOpeningTime == "");
    CHECK(record.duration == 0);
    CHECK(!record.sgsnChange.has_value());
    CHECK(record.causeForRecClosing == 0);
    CHECK(!record.diagnostics.has_value());
    CHECK(!record.recordSequenceNumber.has_value());
    CHECK(!record.nodeID.has_value());
    CHECK(!record.recordExtensions.has_value());
    CHECK(!record.localSequenceNumber.has_value());
    CHECK(!record.apnSelectionMode.has_value());
    CHECK(!record.accessPointNameOI.has_value());
    CHECK(!record.servedMSISDN.has_value());
    CHECK(record.chargingCharacteristics == "");
    CHECK(!record.rATType.has_value());
    CHECK(!record.cAMELInformationPDP.has_value());
    CHECK(!record.chChSelectionMode.has_value());
    CHECK(!record.dynamicAddressFlag.has_value());
    CHECK(!record.pLMNIdentifier.has_value());
    CHECK(!record.mSTimeZone.has_value());
}

TEST_CASE("RealSchema: Encoding Default Constructed")
{
    std::array<uint8_t, 5000> buffer = {};

    fast_ber::SGSN_2009A_CDR::CallEventRecord      record;
    fast_ber::SGSN_2009A_CDR::SGSNSMTRecord        smtr;
    fast_ber::SGSN_2009A_CDR::CAMELInformationPDP  pdp;
    fast_ber::SGSN_2009A_CDR::ManagementExtensions e;

    REQUIRE(fast_ber::encode(absl::Span<uint8_t>(buffer), smtr).success);
    REQUIRE(fast_ber::decode(absl::Span<uint8_t>(buffer), smtr).success);

    REQUIRE(fast_ber::encode(absl::Span<uint8_t>(buffer), pdp).success);
    REQUIRE(fast_ber::decode(absl::Span<uint8_t>(buffer), pdp).success);

    REQUIRE(fast_ber::encode(absl::Span<uint8_t>(buffer), e).success);
    REQUIRE(fast_ber::decode(absl::Span<uint8_t>(buffer), e).success);

    REQUIRE(fast_ber::encode(absl::Span<uint8_t>(buffer), record).success);
    REQUIRE(fast_ber::decode(absl::Span<uint8_t>(buffer), record).success);
}

TEST_CASE("RealSchema: Expected Output")
{
    std::array<uint8_t, 5000> buffer = {};

    fast_ber::SGSN_2009A_CDR::CallEventRecord record;

    REQUIRE(record != fast_ber::SGSN_2009A_CDR::CallEventRecord{test_record});
    REQUIRE(fast_ber::decode(absl::Span<const uint8_t>(test_record_expected_encoding), record).success);
    REQUIRE(record == fast_ber::SGSN_2009A_CDR::CallEventRecord{test_record});

    REQUIRE(fast_ber::encode(absl::Span<uint8_t>(buffer), record).success);
    REQUIRE(fast_ber::encoded_length(record) == test_record_expected_encoding.size());
    REQUIRE(absl::MakeSpan(buffer.data(), test_record_expected_encoding.size()) == test_record_expected_encoding);
}
