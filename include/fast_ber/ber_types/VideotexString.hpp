#pragma once

#include "fast_ber/ber_types/StringImpl.hpp"
#include "fast_ber/ber_types/Tag.hpp"

namespace fast_ber
{
template <typename Identifier = ExplicitId<UniversalTag::videotex_string>>
using VideotexString = fast_ber::StringImpl<UniversalTag::videotex_string, Identifier>;
}
