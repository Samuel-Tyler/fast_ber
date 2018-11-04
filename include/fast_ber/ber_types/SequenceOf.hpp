#pragma once

#include "absl/container/inlined_vector.h"
#include "absl/types/span.h"

#include "fast_ber/ber_types/Class.hpp"
#include "fast_ber/ber_types/Tag.hpp"
#include "fast_ber/util/DecodePrimitive.hpp"
#include "fast_ber/util/EncodeResult.hpp"

namespace fast_ber
{
constexpr const size_t default_inlined_size = 5;

template <typename T, size_t N = default_inlined_size>
using SequenceOf = absl::InlinedVector<T, N>;

template <typename T>
EncodeResult encode_with_new_id(absl::Span<uint8_t> buffer, const SequenceOf<T>& optional_type, Class class_,
                                Tag tag) noexcept
{
    EncodeResult encode_result = {true, 0};
    for (const auto& element : optional_type)
    {
        const auto element_encode_result = encode_with_new_id(buffer, element, class_, tag);
        if (!element_encode_result.success)
        {
            return {false, 0};
        }
        encode_result.encode_length += element_encode_result.encode_length;
        buffer.remove_prefix(element_encode_result.encode_length);
    }
    return encode_result;
}

template <typename T>
bool decode(BerViewIterator& input, SequenceOf<T>& output, Tag tag) noexcept
{
    output.clear();
    while (input->is_valid() && input->tag() == tag)
    {
        output.emplace_back(T());
        bool success = decode(input, output.back(), tag) > 0;
        if (!success)
        {
            return false;
        }
    }
    return true;
}

} // namespace fast_ber
