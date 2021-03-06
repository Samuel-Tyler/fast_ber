#pragma once

#include "fast_ber/ber_types/StringImpl.hpp"
#include "fast_ber/ber_types/Tag.hpp"

namespace fast_ber
{
template <typename Identifier = ExplicitId<UniversalTag::utf8_string>>
using UTF8String = fast_ber::StringImpl<UniversalTag::utf8_string, Identifier>;
}
