#pragma once

#include "fast_ber/ber_types/StringImpl.hpp"
#include "fast_ber/ber_types/Tag.hpp"
#include "fast_ber/ber_types/TaggedType.hpp"

namespace fast_ber
{
template <typename Identifier = ExplicitId<UniversalTag::ia5_string>>
using IA5String = fast_ber::StringImpl<UniversalTag::ia5_string, Identifier>;
}
