#pragma once

#include "absl/types/optional.h"
#include "absl/types/span.h"

#include "fast_ber/ber_types/Class.hpp"
#include "fast_ber/ber_types/Identifier.hpp"
#include "fast_ber/ber_types/Tag.hpp"
#include "fast_ber/util/EncodeHelpers.hpp"

namespace fast_ber
{
template <typename T>
using Optional = absl::optional<T>;

template <typename T, typename ID>
EncodeResult encode(absl::Span<uint8_t> buffer, const Optional<T>& optional_type,
                                     const ID& id) noexcept
{
    if (optional_type.has_value())
    {
        return encode(buffer, *optional_type, id);
    }
    else
    {
        return {true, 0};
    }
}

template <typename T, typename ID>
bool decode(BerViewIterator& input, Optional<T>& output, const ID& id) noexcept
{
    if (input->is_valid() && input->tag() == val(reference_tag(id)))
    {
        output.emplace(T());
        return decode(input, *output, id) > 0;
    }
    else
    {
        output = absl::nullopt;
        return true;
    }
}

} // namespace fast_ber
