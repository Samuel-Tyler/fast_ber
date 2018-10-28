#pragma once

#include "absl/types/span.h"

namespace fast_ber
{

template <typename... Args>
size_t encode_combine(absl::Span<uint8_t> output, Args... args) noexcept
{
    return 0;
}

} // namespace fast_ber
