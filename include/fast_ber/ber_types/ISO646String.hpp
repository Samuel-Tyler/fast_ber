#pragma once

#include "fast_ber/ber_types/StringImpl.hpp"
#include "fast_ber/ber_types/Tag.hpp"
#include "fast_ber/ber_types/TaggedType.hpp"

namespace fast_ber
{
template <typename Identifier = ExplicitIdentifier<UniversalTag::iso646_string>>
using ISO646String = fast_ber::StringImpl<UniversalTag::iso646_string, Identifier>;
}
