#pragma once

#include "fast_ber/ber_types/StringImpl.hpp"
#include "fast_ber/ber_types/Tag.hpp"

namespace fast_ber
{
template <typename Identifier = ExplicitId<UniversalTag::numeric_string>>
using NumericString = fast_ber::StringImpl<UniversalTag::numeric_string, Identifier>;
}
