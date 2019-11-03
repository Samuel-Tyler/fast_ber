#pragma once

#include "fast_ber/ber_types/Sequence.hpp"

#include "absl/container/inlined_vector.h"

namespace fast_ber
{
constexpr const size_t default_inlined_size = 5;

template <typename T, size_t N = default_inlined_size>
struct SequenceOf : public absl::InlinedVector<T, N>
{
    using absl::InlinedVector<T, N>::InlinedVector;

    SequenceOf(const absl::InlinedVector<T, N>& t) : absl::InlinedVector<T, N>(t) {}
    SequenceOf(absl::InlinedVector<T, N>&& t) : absl::InlinedVector<T, N>(std::move(t)) {}
    template <typename T2, size_t N2 = default_inlined_size>
    SequenceOf(const SequenceOf<T2, N2>& t) : absl::InlinedVector<T, N>(t)
    {
    }
    using Id = ExplicitId<UniversalTag::enumerated>;
};

template <typename T>
constexpr inline ExplicitId<UniversalTag::sequence_of> identifier(const SequenceOf<T>*,
                                                                  IdentifierAdlToken = IdentifierAdlToken{}) noexcept
{
    return {};
}

template <typename T>
constexpr inline ExplicitId<UniversalTag::sequence_of>
explicit_identifier(const SequenceOf<T>*, IdentifierAdlToken = IdentifierAdlToken{}) noexcept
{
    return {};
}

template <typename T, typename ID = ExplicitId<UniversalTag::sequence_of>>
EncodeResult encode(const absl::Span<uint8_t> buffer, const SequenceOf<T>& sequence, ID id = ID{}) noexcept
{
    const size_t header_length_guess = 2;
    auto         content_buffer      = buffer;
    size_t       combined_length     = 0;

    content_buffer.remove_prefix(header_length_guess);
    for (const auto& element : sequence)
    {
        const auto element_encode_result = encode(content_buffer, element, identifier(&element, IdentifierAdlToken{}));
        if (!element_encode_result.success)
        {
            return {false, 0};
        }
        combined_length += element_encode_result.length;
        content_buffer.remove_prefix(element_encode_result.length);
    }

    return wrap_with_ber_header(buffer, combined_length, id, header_length_guess);
}

template <typename T, typename ID = ExplicitId<UniversalTag::sequence_of>>
DecodeResult decode(BerViewIterator& input, SequenceOf<T>& output, ID id = ID{}) noexcept
{
    output.clear();
    if (!(input->is_valid() && input->tag() == val(id.tag())) && input->construction() == Construction::constructed)
    {
        return DecodeResult{false};
    }

    auto                    child = input->begin();
    constexpr Identifier<T> child_id;

    while (child->is_valid() && child->tag() == val(child_id.tag()))
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
