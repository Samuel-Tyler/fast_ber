#include "autogen/real_schema.hpp"

#include "catch2/catch.hpp"

#include <array>

TEST_CASE("RealSchema: Testing a standard asn schema")
{
    std::array<uint8_t, 5000>                       buffer = {};
    fast_ber::SGSN_2009A_CDR::ChangeOfCharCondition a;
    fast_ber::SGSN_2009A_CDR::SGSNPDPRecord         record;
    fast_ber::BitString                             qos;
    fast_ber::SGSN_2009A_CDR::DataVolumeGPRS        dataVolumeGPRSUplink;
    fast_ber::SGSN_2009A_CDR::DataVolumeGPRS        dataVolumeGPRSDownlink;
    fast_ber::SGSN_2009A_CDR::ChangeCondition       changeCondition;
    fast_ber::SGSN_2009A_CDR::TimeStamp             changeTime;
    fast_ber::SGSN_2009A_CDR::SGSNSMTRecord         smtr;
    fast_ber::encode(absl::Span<uint8_t>(buffer), qos);
    fast_ber::encode(absl::Span<uint8_t>(buffer), dataVolumeGPRSUplink);
    fast_ber::encode(absl::Span<uint8_t>(buffer), dataVolumeGPRSDownlink);
    //  fast_ber::encode(absl::Span<uint8_t>(buffer), smtr);
    // fast_ber::encode(absl::Span<uint8_t>(buffer), a);
    // fast_ber::encode(absl::Span<uint8_t>(buffer), record);
}
