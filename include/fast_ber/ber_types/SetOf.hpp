#pragma once

#include "fast_ber/ber_types/SequenceOf.hpp"

namespace fast_ber
{

template <typename T, StorageMode s = StorageMode::dynamic>
using SetOf = SequenceOf<T, s>;

} // namespace fast_ber
