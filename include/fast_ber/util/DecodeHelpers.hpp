#pragma once

#include "absl/types/span.h"
#include "fast_ber/ber_types/Boolean.hpp"
#include "fast_ber/ber_types/Identifier.hpp"
#include "fast_ber/ber_types/Integer.hpp"
#include "fast_ber/ber_types/OctetString.hpp"
#include "fast_ber/util/BerView.hpp"

namespace fast_ber
{

template <typename T>
bool primitive_decode_impl(BerViewIterator& input, T& output, const ExplicitIdentifier& id) noexcept
{
    if (!input->is_valid() || val(id.tag) != input->tag())
    {
        return false;
    }

    bool success = output.assign_ber(*input) > 0;
    ++input;
    return success;
}

template <typename T>
bool primitive_decode_impl(BerViewIterator& input, T& output, const TaggedExplicitIdentifier& id) noexcept
{
    if (!input->is_valid() || val(id.outer_tag) != input->tag())
    {
        return false;
    }

    BerView inner = fast_ber::BerView(input->content());
    if (!inner.is_valid() || val(id.inner_tag) != inner.tag())
    {
        return false;
    }

    bool success = output.assign_ber(inner) > 0;
    ++input;
    return success;
}

template <typename T>
bool primitive_decode_impl(BerViewIterator& input, T& output, const ImplicitIdentifier& id) noexcept
{
    if (!input->is_valid() || val(id.tag) != input->tag())
    {
        return false;
    }

    bool success = output.assign_ber(*input) > 0;
    ++input;
    return success;
}

template <typename ID>
bool decode_with_specific_id(BerViewIterator& input, Boolean& output, const ID& id) noexcept
{
    return primitive_decode_impl(input, output, id);
}

template <typename ID>
bool decode_with_specific_id(BerViewIterator& input, Integer& output, const ID& id) noexcept
{
    return primitive_decode_impl(input, output, id);
}

template <typename ID>
bool decode_with_specific_id(BerViewIterator& input, OctetString& output, const ID& id) noexcept
{
    return primitive_decode_impl(input, output, id);
}

} // namespace fast_ber
