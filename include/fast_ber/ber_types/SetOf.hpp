#pragma once

#include "fast_ber/ber_types/SequenceOf.hpp"

namespace fast_ber
{

template <typename T, size_t N = default_inlined_size>
using SetOf = SequenceOf<T, N>;

} // namespace fast_ber
