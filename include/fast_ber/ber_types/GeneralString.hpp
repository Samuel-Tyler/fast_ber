#pragma once

#include "fast_ber/ber_types/StringImpl.hpp"
#include "fast_ber/ber_types/Tag.hpp"
#include "fast_ber/ber_types/TaggedType.hpp"

namespace fast_ber
{
template <typename Identifier = ExplicitId<UniversalTag::general_string>>
using GeneralString = fast_ber::StringImpl<UniversalTag::general_string, Identifier>;
}
