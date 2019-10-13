#include "autogen/real_schema.hpp"

#include "catch2/catch.hpp"

#include <array>

TEST_CASE("RealSchema: Testing a standard asn schema")
{
    std::array<uint8_t, 5000> buffer = {};

    fast_ber::SGSN_2009A_CDR::SGSNPDPRecord        record;
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

    REQUIRE(record == record);
}
