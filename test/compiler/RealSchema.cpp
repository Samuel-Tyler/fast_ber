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
    fast_ber::encode(absl::Span<uint8_t>(buffer), smtr);
    fast_ber::encode(absl::Span<uint8_t>(buffer), pdp);
    fast_ber::encode(absl::Span<uint8_t>(buffer), e);
    fast_ber::encode(absl::Span<uint8_t>(buffer), record);
}
