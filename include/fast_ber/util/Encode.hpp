#pragma once

#include "absl/types/span.h"
#include "fast_ber/ber_types/Class.hpp"
#include "fast_ber/ber_types/Construction.hpp"
#include "fast_ber/ber_types/Identifier.hpp"
#include "fast_ber/util/Create.hpp"
#include "fast_ber/util/EncodeHelpers.hpp"

#include <cstring> // For std::memmove
#include <iostream>

namespace fast_ber
{

inline bool encode_combine_impl(absl::Span<uint8_t>&, size_t&) noexcept { return true; }

template <typename... Args, typename T, typename ID>
bool encode_combine_impl(absl::Span<uint8_t>& output, size_t& encoding_length, const T& object, const ID& id,
                         const Args&... args) noexcept
{
    const EncodeResult result = encode_with_specific_id(output, object, id);
    if (!result.success)
    {
        std::cerr << "Failed encoding packet, tag = " << reference_tag(id) << std::endl;
        return false;
    }

    output.remove_prefix(result.length);
    encoding_length += result.length;
    return encode_combine_impl(output, encoding_length, args...);
}

template <typename... Args, typename ID>
EncodeResult encode_combine(const absl::Span<uint8_t> output, const ID& id, const Args&... args) noexcept
{
    auto   encoding_output = output;
    size_t encode_length   = 0;
    bool   success         = encode_combine_impl(encoding_output, encode_length, args...);
    if (!success)
    {
        return EncodeResult{false, 0};
    }

    return wrap_with_ber_header(output, reference_class(id), reference_tag(id), encode_length);
}

} // namespace fast_ber
