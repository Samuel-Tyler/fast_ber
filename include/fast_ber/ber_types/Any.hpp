#pragma once

#include "fast_ber/ber_types/Choice.hpp"
#include "fast_ber/ber_types/Identifier.hpp"
#include "fast_ber/ber_types/Tag.hpp"

namespace fast_ber
{

using AnyTypes = Choice<
    Choices<BitString<>, Boolean<>, CharacterString<>, Date<>, DateTime<>, Duration<>, GeneralizedTime<>, Integer<>,
            Null<>, ObjectIdentifier<>, OctetString<>, Real<>, Time<>, TimeOfDay<>, UTCTime<>, VisibleString<>>>;

template <typename T = int>
struct Any : AnyTypes
{
    using AnyTypes::AnyTypes;
    using AnyTypes::operator=;

    std::size_t  encoded_length() const noexcept { return this->encoded_length_old(); }
    EncodeResult encode(absl::Span<uint8_t> output) const noexcept { return this->encode_old(output); }
    DecodeResult decode(BerView input) noexcept { return this->decode_old(input); }
};

} // namespace fast_ber
