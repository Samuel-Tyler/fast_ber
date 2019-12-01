#pragma once

#include "fast_ber/ber_types/StringImpl.hpp"
#include "fast_ber/ber_types/Tag.hpp"

namespace fast_ber
{
template <typename Identifier = ExplicitId<UniversalTag::visible_string>>
using VisibleString = fast_ber::StringImpl<UniversalTag::visible_string, Identifier>;
}
