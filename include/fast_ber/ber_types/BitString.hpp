#pragma once

#include "fast_ber/ber_types/StringImpl.hpp"
#include "fast_ber/ber_types/TaggedType.hpp"

namespace fast_ber
{
template <typename Identifier = ExplicitIdentifier<UniversalTag::bit_string>>
using BitString = fast_ber::StringImpl<UniversalTag::bit_string, Identifier>;
}
