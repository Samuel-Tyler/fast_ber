#pragma once

#include "absl/container/inlined_vector.h"
#include "absl/types/span.h"
#include "absl/types/variant.h"

#include "fast_ber/ber_types/Class.hpp"
#include "fast_ber/ber_types/Tag.hpp"
#include "fast_ber/util/DecodeHelpers.hpp"
#include "fast_ber/util/EncodeHelpers.hpp"

namespace fast_ber
{

template <typename... args>
using Choice = absl::variant<args...>;
/*
struct {

    std::tie <>
}

template <typename... Variants, typename... Tags>
EncodeResult encode_with_specific_id(const absl::Span<uint8_t> buffer, const Choice<Variants...> variant,
                                     const Tags&&... sid) noexcept
{
    if (variant.golds)
    auto   content_buffer  = buffer;
    size_t combined_length = 0;
    for (const auto& element : sequence)
    {
        const auto element_encode_result = encode_with_specific_id(content_buffer, element, id);
        if (!element_encode_result.success)
        {
            return {false, 0};
        }
        combined_length += element_encode_result.length;
        content_buffer.remove_prefix(element_encode_result.length);
    }
}*/
/*
template <typename T>
EncodeResult encode(const absl::Span<uint8_t> buffer, const SequenceOf<T>& sequence) noexcept
{
    return encode_with_specific_id(buffer, sequence, ExplicitIdentifier{UniversalTag::sequence_of});
}


template <typename T, typename ID>
bool decode_with_specific_id(BerViewIterator& input, SequenceOf<T>& output, const ID& sequence_id,
                             const ID& inner_id) noexcept
{
    output.clear();
    if (!(input->is_valid() && input->tag() == 16 && input->construction() == Construction::constructed))
    {
        return false;
    }

    auto child = input->begin();
    while (child->is_valid() && child->tag() == val(inner_id.tag))
    {
        output.emplace_back(T());
        bool success = decode_with_specific_id(child, output.back(), inner_id) > 0;
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
    return decode_with_specific_id(input, output, ExplicitIdentifier{UniversalTag::sequence_of},
                                   ExplicitIdentifier{UniversalTag::sequence_of});
}*/

} // namespace fast_ber
