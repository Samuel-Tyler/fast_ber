#pragma once

#include "fast_ber/util/DecodeHelpers.hpp"
#include "fast_ber/util/EncodeHelpers.hpp"

namespace fast_ber
{

template <typename Enumerated, typename ID, typename std::enable_if<std::is_enum<Enumerated>{}, int>::type = 0>
EncodeResult encode_with_specific_id(absl::Span<uint8_t> output, const Enumerated& input, const ID& id)
{
    fast_ber::Integer i(static_cast<int64_t>(input));
    return encode_with_specific_id(output, i, id);
}

template <typename Enumerated, typename ID, typename std::enable_if<std::is_enum<Enumerated>{}, int>::type = 0>
bool decode_with_specific_id(BerViewIterator& input, Enumerated& output, const ID& id) noexcept
{
    fast_ber::Integer i;
    bool              success = decode_with_specific_id(input, i, id);
    if (success)
    {
        output = static_cast<Enumerated>(i.value());
        return true;
    }
    else
    {
        return false;
    }
}

} // namespace fast_ber
