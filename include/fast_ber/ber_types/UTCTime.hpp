#pragma once

#include "fast_ber/ber_types/GeneralizedTime.hpp"
#include "fast_ber/ber_types/Tag.hpp"

namespace fast_ber
{
template <typename Identifier = ExplicitId<UniversalTag::utc_time>>
using UTCTime = GeneralizedTime<Identifier>;
} // namespace fast_ber
