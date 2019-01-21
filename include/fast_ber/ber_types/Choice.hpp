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

template <int index, int max_depth, typename... Variants, typename ID, typename... IDs,
          typename std::enable_if<(!(index < max_depth)), int>::type = 0>
EncodeResult encode_if(absl::Span<uint8_t>&, const Choice<Variants...>&, const ChoiceId<ID, IDs...>&) noexcept
{
    // No substitutions found, fail
    return EncodeResult{false, 0};
}

template <int index, int max_depth, typename... Variants, typename ID, typename... IDs,
          typename std::enable_if<(index < max_depth), int>::type = 0>
EncodeResult encode_if(absl::Span<uint8_t>& buffer, const Choice<Variants...>& choice,
                       const ChoiceId<ID, IDs...>& ids) noexcept
{
    if (choice.index() == index)
    {
        const EncodeResult& inner_encode_result =
            encode_with_specific_id(buffer, absl::get<index>(choice), std::get<index>(ids.ids()));
        if (!inner_encode_result.success)
        {
            return inner_encode_result;
        }

        return wrap_with_ber_header(buffer, reference_class(ids.outer_id()), reference_tag(ids.outer_id()),
                                    inner_encode_result.length);
    }
    else
    {
        return encode_if<index + 1, max_depth>(buffer, choice, ids);
    }
}

template <typename... Variants, typename ID, typename... IDs>
EncodeResult encode_with_specific_id(absl::Span<uint8_t> buffer, const Choice<Variants...> choice,
                                     const ChoiceId<ID, IDs...>& id) noexcept
{
    constexpr auto depth = static_cast<int>(std::tuple_size<decltype(id.ids())>::value);
    return encode_if<0, depth>(buffer, choice, id);
}

template <int index, int max_depth, typename... Variants, typename... IDs,
          typename std::enable_if<(!(index < max_depth)), int>::type = 0>
bool decode_if(BerViewIterator&, Choice<Variants...>&, const ChoiceId<IDs...>&) noexcept
{
    // No substitutions found, fail
    return false;
}

template <int index, int max_depth, typename... Variants, typename ID, typename... IDs,
          typename std::enable_if<(index < max_depth), int>::type = 0>
bool decode_if(BerViewIterator& input, Choice<Variants...>& output, const ChoiceId<ID, IDs...>& ids) noexcept
{
    if (input->tag() == reference_tag(std::get<index>(ids.ids())))
    {
        output = Choice<Variants...>(absl::in_place_index_t<index>());
        return decode_with_specific_id(input, absl::get<index>(output), std::get<index>(ids.ids()));
    }
    else
    {
        return decode_if<index + 1, max_depth>(input, output, ids);
    }
}

template <typename... Variants, typename ID, typename... IDs>
bool decode_with_specific_id(BerViewIterator& input, Choice<Variants...>& output,
                             const ChoiceId<ID, IDs...>& id) noexcept
{
    if (!input->is_valid() || input->tag() != reference_tag(id.outer_id()))
    {
        return false;
    }

    auto child = input->begin();
    if (!child->is_valid())
    {
        return false;
    }

    constexpr auto depth   = static_cast<int>(std::tuple_size<decltype(id.ids())>::value);
    const bool     success = decode_if<0, depth>(child, output, id);
    ++input;
    return success;
}

} // namespace fast_ber
