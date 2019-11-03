#pragma once

#include "fast_ber/ber_types/StringImpl.hpp"
#include "fast_ber/ber_types/Tag.hpp"
#include "fast_ber/ber_types/TaggedType.hpp"

namespace fast_ber
{
template <typename Identifier = ExplicitId<UniversalTag::character_string>>
using CharacterString = fast_ber::StringImpl<UniversalTag::character_string, Identifier>;
}
