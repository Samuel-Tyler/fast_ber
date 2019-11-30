#pragma once

#include "fast_ber/ber_types/Choice.hpp"
#include "fast_ber/ber_types/Identifier.hpp"
#include "fast_ber/ber_types/Tag.hpp"

namespace fast_ber
{
template <typename T = int>
using Any =
    Choice<BitString<>, Boolean<>, CharacterString<>, Date<>, DateTime<>, Duration<>, GeneralizedTime<>, Integer<>,
           Null<>, ObjectIdentifier<>, OctetString<>, Real<>, Time<>, TimeOfDay<>, UTCTime<>, VisibleString<>>;
}
