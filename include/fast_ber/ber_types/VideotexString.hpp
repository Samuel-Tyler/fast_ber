#pragma once

#include "fast_ber/ber_types/StringImpl.hpp"
#include "fast_ber/ber_types/Tag.hpp"
#include "fast_ber/ber_types/TaggedType.hpp"

namespace fast_ber
{
template <typename Identifier = ExplicitIdentifier<UniversalTag::videotex_string>>
using VideotexString = fast_ber::StringImpl<UniversalTag::videotex_string, Identifier>;
}
