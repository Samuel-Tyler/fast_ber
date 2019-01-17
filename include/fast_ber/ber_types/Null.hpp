#pragma once

#include "absl/types/span.h"

#include <algorithm>
#include <cstdint>

#include "fast_ber/util/BerView.hpp"
#include "fast_ber/util/EncodeHelpers.hpp"

namespace fast_ber
{

class Null
{
  public:
    Null() noexcept = default;

    size_t assign_ber(const BerView& rhs) noexcept;
    size_t assign_ber(absl::Span<const uint8_t> buffer) noexcept { return assign_ber(BerView(buffer)); }
    size_t encode_content_and_length(absl::Span<uint8_t> buffer) const noexcept;
};

inline size_t Null::assign_ber(const BerView& rhs) noexcept
{
    if (rhs.is_valid())
    {
        return rhs.ber_length();
    }
    else
    {
        return 0;
    }
}

inline size_t Null::encode_content_and_length(absl::Span<uint8_t> buffer) const noexcept
{
    if (buffer.length() > 0)
    {
        buffer[0] = 0x00;
        return 1;
    }
    else
    {
        return 0;
    }
}

} // namespace fast_ber
