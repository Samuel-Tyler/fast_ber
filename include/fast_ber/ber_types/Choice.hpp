#pragma once

#include "absl/container/inlined_vector.h"
#include "absl/types/span.h"
#include "absl/types/variant.h"

#include "fast_ber/ber_types/Class.hpp"
#include "fast_ber/ber_types/Tag.hpp"
#include "fast_ber/util/DecodeHelpers.hpp"
#include "fast_ber/util/EncodeHelpers.hpp"

#include <tuple>

namespace fast_ber
{

template <typename... args>
using Choice = absl::variant<args...>;

template <int index, typename Variant>
constexpr auto type_at_index() noexcept ->
    typename absl::variant_alternative<index, typename std::remove_reference<Variant>::type>::type*
{
    return nullptr;
}

template <int index, int max_depth, typename... Variants, typename ID,
          typename std::enable_if<(!(index < max_depth)), int>::type = 0>
EncodeResult encode_if(absl::Span<uint8_t>&, const Choice<Variants...>&, const ID&) noexcept
{
    // No substitutions found, fail
    return EncodeResult{false, 0};
}

template <int index, int max_depth, typename... Variants, typename ID,
          typename std::enable_if<(index < max_depth), int>::type = 0>
EncodeResult encode_if(absl::Span<uint8_t>& buffer, const Choice<Variants...>& choice, const ID& id) noexcept
{
    if (choice.index() == index)
    {
        const auto&         child               = absl::get<index>(choice);
        constexpr auto      child_id            = identifier(type_at_index<index, decltype(choice)>());
        const EncodeResult& inner_encode_result = encode(buffer, child, child_id);
        if (!inner_encode_result.success)
        {
            return inner_encode_result;
        }

        return wrap_with_ber_header(buffer, reference_class(id), reference_tag(id), inner_encode_result.length);
    }
    else
    {
        return encode_if<index + 1, max_depth>(buffer, choice, id);
    }
}

template <typename... Variants, typename ID = ExplicitIdentifier<UniversalTag::choice>>
EncodeResult encode(absl::Span<uint8_t> buffer, const Choice<Variants...>& choice, const ID& id = ID{}) noexcept
{
    constexpr auto depth =
        static_cast<int>(absl::variant_size<typename std::remove_reference<decltype(choice)>::type>::value);
    return encode_if<0, depth>(buffer, choice, id);
}

template <int index, int max_depth, typename... Variants, typename ID,
          typename std::enable_if<(!(index < max_depth)), int>::type = 0>
bool decode_if(BerViewIterator&, Choice<Variants...>&, const ID&) noexcept
{
    // No substitutions found, fail
    return false;
}

template <int index, int max_depth, typename... Variants, typename ID,
          typename std::enable_if<(index < max_depth), int>::type = 0>
bool decode_if(BerViewIterator& input, Choice<Variants...>& output, const ID& id) noexcept
{
    if (input->tag() == reference_tag(identifier(type_at_index<index, decltype(output)>())))
    {
        output      = Choice<Variants...>(absl::in_place_index_t<index>());
        auto& child = absl::get<index>(output);
        return decode(input, child, identifier(&child));
    }
    else
    {
        return decode_if<index + 1, max_depth>(input, output, id);
    }
}

template <typename... Variants, typename ID = ExplicitIdentifier<UniversalTag::choice>>
bool decode(BerViewIterator& input, Choice<Variants...>& output, const ID& id = ID{}) noexcept
{
    if (!input->is_valid() || input->tag() != reference_tag(id))
    {
        return false;
    }

    auto child = input->begin();
    if (!child->is_valid())
    {
        return false;
    }

    constexpr auto depth =
        static_cast<int>(absl::variant_size<typename std::remove_reference<decltype(output)>::type>::value);
    const bool success = decode_if<0, depth>(child, output, id);
    ++input;
    return success;
}

} // namespace fast_ber
