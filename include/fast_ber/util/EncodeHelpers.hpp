#pragma once

#include "fast_ber/ber_types/Identifier.hpp"

#include "absl/types/span.h"

#include <cstddef> // for uint8_t
#include <cstring> // for std::memmove

namespace fast_ber
{

struct EncodeResult
{
    bool   success;
    size_t length;
};

// Creates a BER header with provided ID around the data currently in the buffer
// The data of interest should be located in the buffer, with length "content_length"

// A offset if the data in the buffer can specified by "content_offset"
// Building the data at a specfic content_offset will remove the need for moving memory after the header
// has been added.
// Reasonable guesses for content_offset:
// 2 for ExplicitIdentifier and ImplicitIdentifier, 4 for TaggedExplicitIdentifier
// These are the samllest ID lengths, optimising for small sized ber headers.

template <UniversalTag T>
EncodeResult wrap_with_ber_header(absl::Span<uint8_t> buffer, size_t content_length, ExplicitIdentifier<T>,
                                  size_t content_offset = 0)
{
    constexpr auto tag    = ExplicitIdentifier<T>::tag();
    constexpr auto class_ = ExplicitIdentifier<T>::class_();

    size_t header_length = encoded_header_length(Construction::constructed, class_, tag, content_length);
    if (header_length + content_length > buffer.length())
    {
        return EncodeResult{false, 0};
    }
    assert(buffer.length() >= content_length + content_offset);

    if (content_offset != header_length)
    {
        std::memmove(buffer.data() + header_length, buffer.data() + content_offset, content_length);
    }
    encode_header(absl::MakeSpan(buffer.data(), buffer.size()), Construction::constructed, class_, tag, content_length);
    return EncodeResult{true, header_length + content_length};
}

template <Class T1, Tag T2, typename T3>
EncodeResult wrap_with_ber_header(absl::Span<uint8_t> buffer, size_t           content_length,
                                  TaggedExplicitIdentifier<T1, T2, T3>, size_t content_offset = 0)
{
    constexpr auto tag    = TaggedExplicitIdentifier<T1, T2, T3>::tag();
    constexpr auto class_ = TaggedExplicitIdentifier<T1, T2, T3>::class_();

    size_t header_length = encoded_header_length(Construction::constructed, class_, tag, content_length);
    if (header_length + content_length > buffer.length())
    {
        return EncodeResult{false, 0};
    }

    assert(buffer.length() >= content_length + content_offset);

    if (content_offset != header_length)
    {
        std::memmove(buffer.data() + header_length, buffer.data() + content_offset, content_length);
    }
    encode_header(absl::MakeSpan(buffer.data(), buffer.size()), Construction::constructed, class_, tag, content_length);
    return EncodeResult{true, header_length + content_length};
}

template <Class T1, Tag T2>
EncodeResult wrap_with_ber_header(absl::Span<uint8_t> buffer, size_t content_length, ImplicitIdentifier<T1, T2>,
                                  size_t content_offset = 0)
{
    constexpr auto tag    = ImplicitIdentifier<T1, T2>::tag();
    constexpr auto class_ = ImplicitIdentifier<T1, T2>::class_();

    size_t header_length = encoded_header_length(Construction::constructed, class_, tag, content_length);
    if (header_length + content_length > buffer.length())
    {
        return EncodeResult{false, 0};
    }

    assert(buffer.length() >= content_length + content_offset);

    if (content_offset != header_length)
    {
        std::memmove(buffer.data() + header_length, buffer.data() + content_offset, content_length);
    }
    encode_header(absl::MakeSpan(buffer.data(), buffer.size()), Construction::constructed, class_, tag, content_length);
    return EncodeResult{true, header_length + content_length};
}

template <typename ExplicitId, typename T, UniversalTag T2>
EncodeResult encode_impl(absl::Span<uint8_t> output, const T& object, ExplicitIdentifier<T2>)
{
    constexpr auto tag    = ExplicitIdentifier<T2>::tag();
    constexpr auto class_ = ExplicitIdentifier<T2>::class_();

    if (output.empty())
    {
        return EncodeResult{false, 0};
    }
    constexpr auto id_length  = 1;
    constexpr auto encoded_id = encode_short_identifier(Construction::primitive, class_, tag);
    static_assert(val(tag) < 31, "Tag must be short form!");

    output[0] = encoded_id;
    output.remove_prefix(id_length);

    EncodeResult encode_res = object.encode_content_and_length(output);
    encode_res.length += id_length;
    return encode_res;
}

template <typename ExplicitId, typename T, Class T2, Tag T3, typename T4>
EncodeResult encode_impl(absl::Span<uint8_t> output, const T& object, TaggedExplicitIdentifier<T2, T3, T4> id)
{
    const auto header_length_guess = 2;
    auto       inner_buffer        = output;
    inner_buffer.remove_prefix(header_length_guess);
    EncodeResult inner_encoding = encode(inner_buffer, object, id.inner_id());
    if (!inner_encoding.success)
    {
        return EncodeResult{false, 0};
    }

    return wrap_with_ber_header(output, inner_encoding.length, id, header_length_guess);
}

template <typename ExplicitId, typename T, Class T2, Tag T3>
EncodeResult encode_impl(absl::Span<uint8_t> output, const T& object, ImplicitIdentifier<T2, T3> id)
{
    size_t id_length = encode_identifier(output, Construction::primitive, id.class_(), id.tag());
    if (id_length == 0)
    {
        return EncodeResult{false, 0};
    }
    assert(id_length <= output.size());

    output.remove_prefix(id_length);

    EncodeResult encode_res = object.encode_content_and_length(output);
    encode_res.length += id_length;
    return encode_res;
}

} // namespace fast_ber
