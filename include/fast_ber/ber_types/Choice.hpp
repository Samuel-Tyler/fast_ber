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
EncodeResult encode_if(const absl::Span<uint8_t>&, const Choice<Variants...>&, ID) noexcept
{
    // No substitutions found, fail
    return EncodeResult{false, 0};
}

template <typename... T>
constexpr inline ExplicitIdentifier<UniversalTag::choice> identifier(const Choice<T...>*,
                                                                     IdentifierAdlToken = IdentifierAdlToken{}) noexcept
{
    return {};
}

template <typename... T>
constexpr inline ExplicitIdentifier<UniversalTag::choice>
explicit_identifier(const Choice<T...>*, IdentifierAdlToken = IdentifierAdlToken{}) noexcept
{
    return {};
}

template <size_t index, size_t max_depth, typename... Variants, typename ID,
          typename std::enable_if<(index < max_depth), int>::type = 0>
EncodeResult encode_if(const absl::Span<uint8_t>& buffer, const Choice<Variants...>& choice, ID id) noexcept
{
    using T = typename absl::variant_alternative<index, Choice<Variants...>>::type;
    if (choice.index() == index)
    {
        const auto*    child    = absl::get_if<index>(&choice);
        constexpr auto child_id = identifier(static_cast<T*>(nullptr));

        assert(child);

        const auto header_length_guess = 2;
        auto       child_buffer        = buffer;
        child_buffer.remove_prefix(header_length_guess);
        const EncodeResult& inner_encode_result = encode(child_buffer, *child, child_id);
        if (!inner_encode_result.success)
        {
            return inner_encode_result;
        }
        return wrap_with_ber_header(buffer, inner_encode_result.length, id, header_length_guess);
    }
    else
    {
        return encode_if<index + 1, max_depth>(buffer, choice, id);
    }
}

template <typename... Variants, typename ID = ExplicitIdentifier<UniversalTag::choice>>
EncodeResult encode(const absl::Span<uint8_t>& buffer, const Choice<Variants...>& choice, ID id = ID{}) noexcept
{
    constexpr auto depth =
        static_cast<int>(absl::variant_size<typename std::remove_reference<decltype(choice)>::type>::value);
    return encode_if<0, depth>(buffer, choice, id);
}

template <int index, int max_depth, typename... Variants, typename ID,
          typename std::enable_if<(!(index < max_depth)), int>::type = 0>
DecodeResult decode_if(BerViewIterator&, Choice<Variants...>&, ID) noexcept
{
    // No substitutions found, fail
    return DecodeResult{false};
}

template <size_t index, size_t max_depth, typename... Variants, typename ID,
          typename std::enable_if<(index < max_depth), int>::type = 0>
DecodeResult decode_if(BerViewIterator& input, Choice<Variants...>& output, ID id) noexcept
{
    using T                 = typename absl::variant_alternative<index, Choice<Variants...>>::type;
    constexpr auto child_id = identifier(static_cast<T*>(nullptr));
    if (input->tag() == val(child_id.tag()))
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
DecodeResult decode(BerViewIterator& input, Choice<Variants...>& output, ID id = ID{}) noexcept
{
    if (!input->is_valid() || input->tag() != val(id.tag()))
    {
        return DecodeResult{false};
    }

    auto child = input->begin();
    if (!child->is_valid())
    {
        return DecodeResult{false};
    }

    constexpr auto     depth  = absl::variant_size<typename std::remove_reference<decltype(output)>::type>::value;
    const DecodeResult result = decode_if<0, depth>(child, output, id);
    ++input;
    return result;
}

} // namespace fast_ber
