#pragma once

#include "fast_ber/ber_types/StringImpl.hpp"
#include "fast_ber/ber_types/Tag.hpp"
#include "fast_ber/ber_types/TaggedType.hpp"

namespace fast_ber
{
template <typename Identifier = ExplicitId<UniversalTag::bmp_string>>
using BMPString = fast_ber::StringImpl<UniversalTag::bmp_string, Identifier>;
}
