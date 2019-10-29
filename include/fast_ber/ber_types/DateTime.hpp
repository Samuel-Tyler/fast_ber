#pragma once

#include "fast_ber/ber_types/GeneralizedTime.hpp"
#include "fast_ber/ber_types/Tag.hpp"

namespace fast_ber
{
template <typename Identifier = ExplicitIdentifier<UniversalTag::generalized_time>>
using DateTime = GeneralizedTime<Identifier>;
} // namespace fast_ber
