#pragma once

#include "absl/types/optional.h"
#include "absl/types/span.h"

#include "fast_ber/ber_types/Class.hpp"
#include "fast_ber/ber_types/Tag.hpp"
#include "fast_ber/util/EncodeResult.hpp"

namespace fast_ber
{
template <typename T>
using Optional = absl::optional<T>;

template <typename T>
EncodeResult encode_with_new_id(absl::Span<uint8_t> buffer, const Optional<T>& optional_type, Class class_,
                                Tag tag) noexcept
{
    if (optional_type.has_value())
    {
        return encode_with_new_id(buffer, *optional_type, class_, tag);
    }
    else
    {
        return {true, 0};
    }
}

template <typename T>
bool decode(BerViewIterator& input, Optional<T>& output, Tag tag) noexcept
{
    if (input->is_valid() && input->tag() == tag)
    {

        output.emplace(T());
        return decode(input, *output, tag) > 0;
    }
    else
    {
        output = absl::nullopt;
        return true;
    }
}

} // namespace fast_ber
