#pragma once

#include "fast_ber/ber_types/OctetString.hpp"
#include "fast_ber/ber_types/Tag.hpp"
#include "fast_ber/ber_types/TaggedType.hpp"

namespace fast_ber
{
using VisibleString = TaggedType<fast_ber::OctetString<>, ExplicitIdentifier<UniversalTag::printable_string>>;
}
