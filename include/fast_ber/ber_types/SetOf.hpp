#pragma once

#include "fast_ber/ber_types/SequenceOf.hpp"

namespace fast_ber
{

template <typename T, typename I = fast_ber::ExplicitId<fast_ber::UniversalTag::set>,
          StorageMode s = StorageMode::dynamic>
using SetOf = SequenceOf<T, I, s>;

} // namespace fast_ber
