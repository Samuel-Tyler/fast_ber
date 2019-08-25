#pragma once

#include "absl/types/span.h"
#include "fast_ber/ber_types/Identifier.hpp"
#include "fast_ber/util/BerView.hpp"

namespace fast_ber
{

struct DecodeResult
{
    bool success;
};

template <typename T, UniversalTag T2>
DecodeResult decode_impl(BerViewIterator& input, T& output, const ExplicitIdentifier<T2>& id) noexcept
{
    if (!input->is_valid() || val(id.tag()) != input->tag())
    {
        return DecodeResult{false};
    }

    bool success = output.assign_ber(*input) > 0;
    ++input;
    return DecodeResult{success};
}

template <typename T, Class T2, Tag T3, typename T4>
DecodeResult decode_impl(BerViewIterator& input, T& output, const TaggedExplicitIdentifier<T2, T3, T4>& id) noexcept
{
    if (!input->is_valid() || val(id.outer_tag()) != input->tag())
    {
        return DecodeResult{false};
    }

    BerView inner = fast_ber::BerView(input->content());
    if (!inner.is_valid() || val(id.inner_id().tag()) != inner.tag())
    {
        return DecodeResult{false};
    }

    bool success = output.assign_ber(inner) > 0;
    ++input;
    return DecodeResult{success};
}

template <typename T, Class T2, Tag T3>
DecodeResult decode_impl(BerViewIterator& input, T& output, const ImplicitIdentifier<T2, T3>& id) noexcept
{
    if (!input->is_valid() || val(id.tag()) != input->tag())
    {
        return DecodeResult{false};
    }

    bool success = output.assign_ber(*input) > 0;
    ++input;
    return DecodeResult{success};
}

template <typename T, typename ID>
DecodeResult decode(absl::Span<const uint8_t> input, T& output, const ID& id) noexcept
{
    BerViewIterator iter(input);
    return decode(iter, output, id);
}
template <typename T>
DecodeResult decode(absl::Span<const uint8_t> input, T& output) noexcept
{
    BerViewIterator iter(input);
    return decode(iter, output);
}

} // namespace fast_ber
