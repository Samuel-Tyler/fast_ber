#pragma once

#include "fast_ber/ber_types/Sequence.hpp"

namespace fast_ber
{
constexpr const size_t default_inlined_size = 5;

template <typename T, size_t N = default_inlined_size>
using SequenceOf = absl::InlinedVector<T, N>;

template <typename T>
constexpr inline ExplicitIdentifier<UniversalTag::sequence_of> identifier(const SequenceOf<T>*) noexcept
{
    return {};
}

template <typename T, typename ID = ExplicitIdentifier<UniversalTag::sequence_of>>
EncodeResult encode(const absl::Span<uint8_t> buffer, const SequenceOf<T>& sequence, const ID& id = ID{}) noexcept
{
    const size_t header_length_guess = 2;
    auto         content_buffer      = buffer;
    size_t       combined_length     = 0;

    content_buffer.remove_prefix(header_length_guess);
    for (const auto& element : sequence)
    {
        const auto element_encode_result = encode(content_buffer, element, identifier(&element));
        if (!element_encode_result.success)
        {
            return {false, 0};
        }
        combined_length += element_encode_result.length;
        content_buffer.remove_prefix(element_encode_result.length);
    }

    return wrap_with_ber_header(buffer, combined_length, id, header_length_guess);
}

template <typename T, typename ID = ExplicitIdentifier<UniversalTag::sequence_of>>
DecodeResult decode(BerViewIterator& input, SequenceOf<T>& output, const ID& id = ID{}) noexcept
{
    output.clear();
    if (!(input->is_valid() && input->tag() == reference_tag(id)) && input->construction() == Construction::constructed)
    {
        return DecodeResult{false};
    }

    auto           child    = input->begin();
    constexpr auto child_id = identifier(static_cast<const T*>(nullptr));

    while (child->is_valid() && child->tag() == reference_tag(child_id))
    {
        output.emplace_back(T{});
        bool success = decode(child, output.back(), child_id).success;
        if (!success)
        {
            return DecodeResult{false};
        }
    }
    ++input;

    return DecodeResult{true};
}

} // namespace fast_ber
