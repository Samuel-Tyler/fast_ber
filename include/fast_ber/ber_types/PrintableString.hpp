#pragma once

#include "fast_ber/ber_types/StringImpl.hpp"
#include "fast_ber/ber_types/Tag.hpp"

namespace fast_ber
{
template <typename Identifier = ExplicitId<UniversalTag::printable_string>>
using PrintableString = fast_ber::StringImpl<UniversalTag::printable_string, Identifier>;
}
