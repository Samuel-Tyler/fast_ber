#pragma once

#include "absl/container/inlined_vector.h"
#include "absl/types/span.h"

#include "fast_ber/ber_types/Class.hpp"
#include "fast_ber/ber_types/Tag.hpp"
#include "fast_ber/util/DecodeHelpers.hpp"
#include "fast_ber/util/EncodeHelpers.hpp"

namespace fast_ber
{
constexpr const size_t default_inlined_size = 5;

template <typename T, size_t N = default_inlined_size>
using SequenceOf = absl::InlinedVector<T, N>;

template <typename T>
EncodeResult encode_with_specific_id(const absl::Span<uint8_t> buffer, const SequenceOf<T>& sequence, Class class_,
                                     Tag tag) noexcept
{
    auto   content_buffer  = buffer;
    size_t combined_length = 0;
    for (const auto& element : sequence)
    {
        const auto element_encode_result = encode_with_specific_id(content_buffer, element, class_, tag);
        if (!element_encode_result.success)
        {
            return {false, 0};
        }
        combined_length += element_encode_result.length;
        content_buffer.remove_prefix(element_encode_result.length);
    }

    return wrap_with_ber_header(buffer, class_, 16, combined_length);
}

template <typename T>
EncodeResult encode(const absl::Span<uint8_t> buffer, const SequenceOf<T>& sequence) noexcept
{
    return encode_with_specific_id(buffer, sequence, Class::universal, val(UniversalTag::sequence_of));
}

template <typename T>
bool decode_with_specific_id(BerViewIterator& input, SequenceOf<T>& output, Tag tag) noexcept
{
    output.clear();
    if (!(input->is_valid() && input->tag() == 16 && input->construction() == Construction::constructed))
    {
        return false;
    }

    auto child = input->begin();
    while (child->is_valid() && child->tag() == tag)
    {
        output.emplace_back(T());
        bool success = decode_with_specific_id(child, output.back(), tag) > 0;
        if (!success)
        {
            return false;
        }
    }
    ++input;

    return true;
}

template <typename T>
bool decode(BerViewIterator& input, SequenceOf<T>& output) noexcept
{
    return decode_with_specific_id(input, output, val(UniversalTag::sequence_of));
}

} // namespace fast_ber
