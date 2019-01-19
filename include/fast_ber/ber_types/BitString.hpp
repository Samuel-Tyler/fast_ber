#pragma once

#include "fast_ber/ber_types/OctetString.hpp"
#include "fast_ber/ber_types/TaggedType.hpp"

namespace fast_ber
{

using BitString = TaggedType<OctetString, ExplicitIdentifier<UniversalTag::bit_string>>;
}
