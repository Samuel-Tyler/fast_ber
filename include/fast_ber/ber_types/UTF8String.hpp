#pragma once

#include "fast_ber/ber_types/StringImpl.hpp"
#include "fast_ber/ber_types/Tag.hpp"
#include "fast_ber/ber_types/TaggedType.hpp"

namespace fast_ber
{
template <typename Identifier = ExplicitIdentifier<UniversalTag::utf8_string>>
using UTF8String = fast_ber::StringImpl<UniversalTag::utf8_string, Identifier>;
}
