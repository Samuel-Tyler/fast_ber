#pragma once

#include "absl/container/inlined_vector.h"
#include "absl/types/span.h"

#include "fast_ber/ber_types/Class.hpp"
#include "fast_ber/ber_types/Identifier.hpp"
#include "fast_ber/ber_types/Tag.hpp"
#include "fast_ber/util/DecodeHelpers.hpp"
#include "fast_ber/util/EncodeHelpers.hpp"

namespace fast_ber
{
constexpr const size_t default_inlined_size = 5;

template <typename T, size_t N = default_inlined_size>
using SequenceOf = absl::InlinedVector<T, N>;

template <typename T, typename ID = ExplicitIdentifier<UniversalTag::sequence_of>>
EncodeResult encode(const absl::Span<uint8_t> buffer, const SequenceOf<T>& sequence, const ID& id = ID{}) noexcept
{
    auto   content_buffer  = buffer;
    size_t combined_length = 0;
    for (const auto& element : sequence)
    {
        const auto element_encode_result = encode(content_buffer, element, identifier(element));
        if (!element_encode_result.success)
        {
            return {false, 0};
        }
        combined_length += element_encode_result.length;
        content_buffer.remove_prefix(element_encode_result.length);
    }

    return wrap_with_ber_header(buffer, reference_class(id), reference_tag(id), combined_length);
}

template <typename T, typename ID = ExplicitIdentifier<UniversalTag::sequence_of>>
bool decode(BerViewIterator& input, SequenceOf<T>& output, const ID& id = ID{}) noexcept
{
    output.clear();
    if (!(input->is_valid() && input->tag() == reference_tag(id)) && input->construction() == Construction::constructed)
    {
        return false;
    }

    auto       child    = input->begin();
    const auto child_id = identifier(T{});

    while (child->is_valid() && child->tag() == reference_tag(child_id))
    {
        output.emplace_back(T{});
        bool success = decode(child, output.back(), child_id) > 0;
        if (!success)
        {
            return false;
        }
    }
    ++input;

    return true;
}

} // namespace fast_ber
