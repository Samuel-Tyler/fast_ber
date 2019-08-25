#pragma once

#include "fast_ber/ber_types/GeneralizedTime.hpp"

namespace fast_ber
{
template <typename Identifier = ExplicitIdentifier<UniversalTag::utc_time>>
using Duration = GeneralizedTime<Identifier>;
} // namespace fast_ber
