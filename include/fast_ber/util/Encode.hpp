#pragma once

#include "absl/types/span.h"
#include "fast_ber/ber_types/Class.hpp"
#include "fast_ber/ber_types/Construction.hpp"
#include "fast_ber/util/Create.hpp"
#include "fast_ber/util/EncodeResult.hpp"

#include <cstring> // For std::memmove
#include <iostream>

namespace fast_ber
{

bool encode_combine_impl(absl::Span<uint8_t>&, size_t&) noexcept { return true; }

template <typename... Args, typename T>
bool encode_combine_impl(absl::Span<uint8_t>& output, size_t& encoding_length, const T& object, Class class_, int tag,
                         const Args&... args) noexcept
{
    const EncodeResult result = encode_with_new_id(output, object, class_, tag);
    if (!result.success)
    {
        std::cerr << "Failed encoding packet, tag = " << tag << std::endl;
        return false;
    }

    output.remove_prefix(result.encode_length);
    encoding_length += result.encode_length;
    return encode_combine_impl(output, encoding_length, args...);
}

template <typename... Args>
EncodeResult encode_combine(const absl::Span<uint8_t> output, Class class_, int tag, const Args&... args) noexcept
{
    auto   encoding_output = output;
    size_t encode_length   = 0;
    bool   success         = encode_combine_impl(encoding_output, encode_length, args...);
    if (!success)
    {
        return EncodeResult{false, 0};
    }

    std::array<uint8_t, 30> buffer;
    size_t header_length = create_header(absl::MakeSpan(buffer.data(), buffer.size()), Construction::constructed,
                                         class_, tag, encode_length);
    assert(header_length != 0);
    if (header_length + encode_length > output.length())
    {
        return EncodeResult{false, 0};
    }

    std::memmove(output.data() + header_length, output.data(), encode_length);
    std::memcpy(output.data(), buffer.data(), header_length);
    return EncodeResult{true, header_length + encode_length};
}

} // namespace fast_ber
