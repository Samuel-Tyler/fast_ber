#pragma once

#include "absl/types/span.h"
#include "fast_ber/ber_types/Class.hpp"
#include "fast_ber/ber_types/Construction.hpp"
#include "fast_ber/ber_types/Identifier.hpp"
#include "fast_ber/util/Create.hpp"
#include "fast_ber/util/DecodeHelpers.hpp"
#include "fast_ber/util/EncodeHelpers.hpp"

#include <cstring> // For std::memmove
#include <iostream>

namespace fast_ber
{

inline bool encode_set_combine_impl(absl::Span<uint8_t>&, size_t&) noexcept { return true; }

template <typename... Args, typename T, typename ID>
bool encode_set_combine_impl(absl::Span<uint8_t>& output, size_t& encoding_length, const T& object, const ID& id,
                             const Args&... args) noexcept
{
    const EncodeResult result = encode(output, object, id);
    if (!result.success)
    {
        std::cerr << "Failed encoding packet, tag = " << reference_tag(id) << std::endl;
        return false;
    }

    output.remove_prefix(result.length);
    encoding_length += result.length;
    return encode_set_combine_impl(output, encoding_length, args...);
}

template <typename... Args, typename ID>
EncodeResult encode_set_combine(const absl::Span<uint8_t> output, const ID& id, const Args&... args) noexcept
{
    auto   encoding_output = output;
    size_t encode_length   = 0;
    bool   success         = encode_set_combine_impl(encoding_output, encode_length, args...);
    if (!success)
    {
        return EncodeResult{false, 0};
    }

    return wrap_with_ber_header(output, encode_length, id);
}

template <const char* parent_name>
bool decode_set_combine_impl(BerViewIterator&) noexcept
{
    return true;
}

template <const char* parent_name, typename T, typename ID, typename... Args>
bool decode_set_combine_impl(BerViewIterator& input, T& object, const ID& id, Args&&... args) noexcept
{
    bool success = decode(input, object, id);
    if (!success)
    {
        std::cerr << "Error decoding " << parent_name << ": could not decode field with tag " << reference_tag(id)
                  << "\n";
        return false;
    }
    return decode_set_combine_impl<parent_name>(input, args...);
}

template <const char* parent_name, typename ID, typename... Args>
bool decode_set_combine(const BerView& input, const ID& id, Args&&... args) noexcept
{
    if (!input.is_valid())
    {
        std::cerr << "Error decoding " << parent_name << ": Input is not valid ber\n";
        return false;
    }
    if (input.tag() != reference_tag(id))
    {
        std::cerr << "Error decoding " << parent_name << ": Expected tag = " << reference_tag(id) << " got "
                  << input.tag() << "\n";
        return false;
    }

    auto iterator = input.begin();
    return decode_set_combine_impl<parent_name>(iterator, args...);
}

} // namespace fast_ber
