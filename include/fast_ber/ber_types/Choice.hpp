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

template <int index, int max_depth, typename... Variants, typename... IDs,
          typename std::enable_if<(!(index < max_depth)), int>::type = 0>
EncodeResult encode_if(absl::Span<uint8_t>&, const Choice<Variants...>&, const ChoiceID<IDs...>&) noexcept
{
    // No substitutions found, fail
    return EncodeResult{false, 0};
}

template <int index, int max_depth, typename... Variants, typename... IDs,
          typename std::enable_if<(index < max_depth), int>::type = 0>
EncodeResult encode_if(absl::Span<uint8_t>& buffer, const Choice<Variants...>& choice,
                       const ChoiceID<IDs...>& ids) noexcept
{
    if (choice.index() == index)
    {
        return encode_with_specific_id(buffer, absl::get<index>(choice), std::get<index>(ids.ids));
    }
    else
    {

        return encode_if<index + 1, max_depth>(buffer, choice, ids);
    }
}

template <typename... Variants, typename... IDs>
EncodeResult encode_with_specific_id(absl::Span<uint8_t> buffer, const Choice<Variants...> choice,
                                     const ChoiceID<IDs...>& id) noexcept
{
    // static_assert(absl::variant_size<decltype(choice.variant)>() == std::tuple_size(id.ids));
    constexpr int depth = (int)std::tuple_size<decltype(id.ids)>::value;
    return encode_if<0, depth>(buffer, choice, id);
}
/*
template <typename T>
EncodeResult encode(const absl::Span<uint8_t> buffer, const SequenceOf<T>& sequence) noexcept
{
    return encode_with_specific_id(buffer, sequence, ExplicitIdentifier{UniversalTag::sequence_of});
}



*/

template <int index, int max_depth, typename... Variants, typename... IDs,
          typename std::enable_if<(!(index < max_depth)), int>::type = 0>
bool decode_if(BerViewIterator&, Choice<Variants...>&, const ChoiceID<IDs...>&) noexcept
{
    // No substitutions found, fail
    return false;
}

template <int index, int max_depth, typename... Variants, typename... IDs,
          typename std::enable_if<(index < max_depth), int>::type = 0>
bool decode_if(BerViewIterator& input, Choice<Variants...>& output, const ChoiceID<IDs...>& ids) noexcept
{
    if (input->tag() == reference_tag(std::get<index>(ids.ids)))
    {
        output = Choice<Variants...>(absl::in_place_index_t<index>());
        return decode_with_specific_id(input, absl::get<index>(output), std::get<index>(ids.ids));
    }
    else
    {
        return decode_if<index + 1, max_depth>(input, output, ids);
    }
}

template <typename... Variants, typename... IDs>
bool decode_with_specific_id(BerViewIterator& input, Choice<Variants...>& output, const ChoiceID<IDs...>& id) noexcept
{
    if (!input->is_valid())
    {
        return false;
    }

    constexpr int depth = (int)std::tuple_size<decltype(id.ids)>::value;
    return decode_if<0, depth>(input, output, id);
}
/*
template <typename T>
bool decode(BerViewIterator& input, SequenceOf<T>& output) noexcept
{
    return decode_with_specific_id(input, output, ExplicitIdentifier{UniversalTag::sequence_of},
                                   ExplicitIdentifier{UniversalTag::sequence_of});
}*/

} // namespace fast_ber
