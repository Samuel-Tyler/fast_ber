#pragma once

#include "fast_ber/ber_types/StringImpl.hpp"

namespace fast_ber
{
template <typename Identifier = ExplicitId<UniversalTag::bit_string>>
using BitString = fast_ber::StringImpl<UniversalTag::bit_string, Identifier>;
}
