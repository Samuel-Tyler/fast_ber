#pragma once

#include "fast_ber/ber_types/StringImpl.hpp"
#include "fast_ber/ber_types/Tag.hpp"
#include "fast_ber/ber_types/TaggedType.hpp"

namespace fast_ber
{
template <typename Identifier = ExplicitId<UniversalTag::graphic_string>>
using GraphicString = fast_ber::StringImpl<UniversalTag::graphic_string, Identifier>;
}
