#pragma once

#include "absl/types/span.h"
#include "fast_ber/ber_types/Class.hpp"
#include "fast_ber/ber_types/Construction.hpp"
#include "fast_ber/ber_types/Identifier.hpp"
#include "fast_ber/util/DecodeHelpers.hpp"
#include "fast_ber/util/EncodeHelpers.hpp"
#include "fast_ber/util/EncodeIdentifiers.hpp"

#include <cstring> // For std::memmove
#include <iostream>

namespace fast_ber
{

inline EncodeResult encode_sequence_combine_impl(absl::Span<uint8_t>&, size_t encoding_length) noexcept
{
    return EncodeResult{true, encoding_length};
}

template <typename... Args, typename T, typename ID>
EncodeResult encode_sequence_combine_impl(absl::Span<uint8_t>& output, size_t encoding_length, const T& object,
                                          const ID& id, const Args&... args) noexcept
{
    const EncodeResult result = encode(output, object, id);
    if (!result.success)
    {
        std::cerr << "Failed encoding packet, tag = " << reference_tag(id) << std::endl;
        return EncodeResult{false, result.length};
    }

    output.remove_prefix(result.length);
    encoding_length += result.length;
    return encode_sequence_combine_impl(output, encoding_length, args...);
}

template <typename... Args, typename ID>
EncodeResult encode_sequence_combine(const absl::Span<uint8_t> output, const ID& id, const Args&... args) noexcept
{
    auto   encoding_output     = output;
    size_t header_length_guess = 2;
    encoding_output.remove_prefix(header_length_guess);
    EncodeResult result = encode_sequence_combine_impl(encoding_output, 0, args...);
    if (!result.success)
    {
        return result;
    }

    return wrap_with_ber_header(output, result.length, id, header_length_guess);
}

inline DecodeResult decode_sequence_combine_impl(BerViewIterator&, const char*) noexcept { return DecodeResult{true}; }

template <typename T, typename ID, typename... Args>
DecodeResult decode_sequence_combine_impl(BerViewIterator& input, const char* parent_name, T& object, const ID& id,
                                          Args&&... args) noexcept
{
    DecodeResult result = decode(input, object, id);
    if (!result.success)
    {
        std::cerr << "Error decoding " << parent_name << ": could not decode field with tag " << reference_tag(id)
                  << "\n";
        return DecodeResult{false};
    }
    return decode_sequence_combine_impl(input, parent_name, args...);
}

template <typename ID, typename... Args>
DecodeResult decode_sequence_combine(const BerView& input, const char* parent_name, const ID& id,
                                     Args&&... args) noexcept
{
    if (!input.is_valid())
    {
        std::cerr << "Error decoding " << parent_name << ": Input is not valid ber\n";
        return DecodeResult{false};
    }
    if (input.tag() != reference_tag(id))
    {
        std::cerr << "Error decoding " << parent_name << ": Expected tag = " << reference_tag(id) << " got "
                  << input.tag() << "\n";
        return DecodeResult{false};
    }

    auto iterator = input.begin();
    return decode_sequence_combine_impl(iterator, parent_name, args...);
}

} // namespace fast_ber
