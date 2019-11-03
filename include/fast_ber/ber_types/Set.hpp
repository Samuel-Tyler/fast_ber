#pragma once

#include "fast_ber/ber_types/Sequence.hpp"

namespace fast_ber
{

// Sets can be encoded in any order, but choosing the same as Sequences is the most logical
template <typename... Args, typename ID>
EncodeResult encode_set_combine(const absl::Span<uint8_t> output, ID id, const Args&... args) noexcept
{
    return encode_sequence_combine(output, id, args...);
}

inline DecodeResult decode_set_combine_impl(BerViewIterator&, const char*) noexcept { return DecodeResult{true}; }

template <typename T, typename... Args>
DecodeResult decode_set_combine_impl(BerViewIterator& input, const char* parent_name, T& object,
                                     Args&&... args) noexcept
{
    Identifier<T> id;
    DecodeResult  result = decode(input, object, id);
    if (!result.success)
    {
        std::cerr << "Error decoding " << parent_name << ": could not decode field with tag " << val(id.tag()) << "\n";
        return DecodeResult{false};
    }
    return decode_set_combine_impl(input, parent_name, args...);
}

template <typename ID, typename... Args>
DecodeResult decode_set_combine(const BerView& input, const char* parent_name, ID id, Args&&... args) noexcept
{
    if (!input.is_valid())
    {
        std::cerr << "Error decoding " << parent_name << ": Input is not valid ber\n";
        return DecodeResult{false};
    }
    if (input.tag() != val(id.tag()))
    {
        std::cerr << "Error decoding " << parent_name << ": Expected tag = " << val(id.tag()) << " got " << input.tag()
                  << "\n";
        return DecodeResult{false};
    }

    auto iterator = input.begin();
    return decode_set_combine_impl(iterator, parent_name, args...);
}

} // namespace fast_ber
