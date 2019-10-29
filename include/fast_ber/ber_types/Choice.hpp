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

template <typename... Args>
struct Choice : public absl::variant<Args...>
{
    using absl::variant<Args...>::variant;
    using absl::variant<Args...>::operator=;

    using Base = absl::variant<Args...>;
    const absl::variant<Args...>& base() const { return *static_cast<const absl::variant<Args...>*>(this); }

    Choice() : absl::variant<Args...>() {}
    Choice(const absl::variant<Args...>& rhs) : absl::variant<Args...>(rhs) {}
    Choice(absl::variant<Args...>&& rhs) : absl::variant<Args...>(rhs) {}
};

template <typename T>
using variant_size = absl::variant_size<typename T::Base>;

template <std::size_t n, typename T>
using variant_alternative = absl::variant_alternative<n, typename T::Base>;

template <typename... Args>
bool operator==(const Choice<Args...>& lhs, const Choice<Args...>& rhs)
{
    return lhs.base() == rhs.base();
}

template <typename... Args>
bool operator!=(const Choice<Args...>& lhs, const Choice<Args...>& rhs)
{
    return lhs.base() != rhs.base();
}

template <size_t index, size_t max_depth, typename... Variants, typename ID,
          typename std::enable_if<(!(index < max_depth)), int>::type = 0>
EncodeResult encode_if(const absl::Span<uint8_t>&, const Choice<Variants...>&, ID) noexcept
{
    // No substitutions found, fail
    return EncodeResult{false, 0};
}

template <typename... Args>
constexpr ExplicitIdentifier<UniversalTag::choice> identifier(const Choice<Args...>*,
                                                              IdentifierAdlToken = IdentifierAdlToken{}) noexcept
{
    return {};
}

template <size_t index, size_t max_depth, typename... Variants, typename ID,
          typename std::enable_if<(index < max_depth), int>::type = 0>
EncodeResult encode_if(const absl::Span<uint8_t>& buffer, const Choice<Variants...>& choice, ID id) noexcept
{
    using T = typename fast_ber::variant_alternative<index, Choice<Variants...>>::type;
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
    constexpr auto depth = static_cast<int>(fast_ber::variant_size<Choice<Variants...>>::value);
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
    using T                 = typename fast_ber::variant_alternative<index, Choice<Variants...>>::type;
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

    constexpr auto     depth  = fast_ber::variant_size<typename std::remove_reference<decltype(output)>::type>::value;
    const DecodeResult result = decode_if<0, depth>(child, output, id);
    ++input;
    return result;
}

} // namespace fast_ber
