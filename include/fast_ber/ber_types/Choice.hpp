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

template <size_t index, size_t max_depth, typename... Variants, typename ID,
          typename std::enable_if<(!(index < max_depth)), int>::type = 0>
EncodeResult encode_if(const absl::Span<uint8_t>&, const Choice<Variants...>&, const ID&) noexcept
{
    // No substitutions found, fail
    return EncodeResult{false, 0};
}

template <size_t index, size_t max_depth, typename... Variants, typename ID,
          typename std::enable_if<(index < max_depth), int>::type = 0,
          typename T = typename absl::variant_alternative<index, Choice<Variants...>>::type>
EncodeResult encode_if(absl::Span<uint8_t> buffer, const Choice<Variants...>& choice, const ID& id) noexcept
{
    if (choice.index() == index)
    {
        const auto*    child    = absl::get_if<index>(&choice);
        constexpr auto child_id = identifier(static_cast<T*>(nullptr));

        assert(child);

        const EncodeResult& inner_encode_result = encode(buffer, *child, child_id);
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

template <size_t index, size_t max_depth, typename... Variants, typename ID,
          typename std::enable_if<(index < max_depth), int>::type = 0,
          typename T = typename absl::variant_alternative<index, Choice<Variants...>>::type>
bool decode_if(BerViewIterator& input, Choice<Variants...>& output, const ID& id) noexcept
{
    constexpr auto child_id = identifier(static_cast<T*>(nullptr));
    if (input->tag() == reference_tag(child_id))
    {
        T* child = &output.template emplace<index>();
        return decode(input, *child, child_id);
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

    constexpr auto depth   = absl::variant_size<typename std::remove_reference<decltype(output)>::type>::value;
    const bool     success = decode_if<0, depth>(child, output, id);
    ++input;
    return success;
}

} // namespace fast_ber
