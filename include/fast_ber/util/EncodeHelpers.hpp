#pragma once

#include "fast_ber/ber_types/Identifier.hpp"
#include "fast_ber/util/Definitions.hpp"
#include "fast_ber/util/EncodeIdentifiers.hpp"

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

template <Class T1, Tag T2>
EncodeResult encode_header(absl::Span<uint8_t> buffer, size_t content_length, Id<T1, T2> id, Construction construction)
{
    size_t header_length = encode_header(buffer, construction, id.class_(), id.tag(), content_length);
    if (header_length == 0)
    {
        return EncodeResult{false, 0};
    }
    return EncodeResult{true, header_length + content_length};
}

template <typename Identifier, size_t content_length = 0,
          size_t encoded_length = encoded_length(content_length, Identifier{})>
std::array<uint8_t, encoded_length> encoded_header() noexcept
{
    std::array<uint8_t, encoded_length> data = {};
    encode_header(absl::Span<uint8_t>(data), content_length, Identifier{}, Construction::primitive);
    return data;
}

template <typename OuterId, typename InnerId>
EncodeResult encode_header(absl::Span<uint8_t> buffer, size_t content_length, DoubleId<OuterId, InnerId> id,
                           Construction construction)
{
    size_t inner_header_length = encoded_header_length(content_length, id.inner_id());
    size_t outer_header_length = encoded_header_length(inner_header_length + content_length, id.outer_id());

    if (buffer.size() < outer_header_length + inner_header_length + content_length)
    {
        return EncodeResult{false, 0};
    }

    auto inner = buffer;
    inner.remove_prefix(outer_header_length);

    encode_header(inner, content_length, id.inner_id(), construction);
    return encode_header(buffer, inner_header_length + content_length, id.outer_id(), Construction::constructed);
}

// Creates a BER header with provided ID around the data currently in the buffer
// The data of interest should be located in the buffer, with length "content_length"

// An offset if the data in the buffer can specified by "content_offset"
// Building the data at a specfic content_offset will remove the need for moving memory after the header
// has been added.

template <typename Identifier>
EncodeResult wrap_with_ber_header(absl::Span<uint8_t> buffer, size_t content_length, Identifier id,
                                  size_t content_offset = 0, Construction construction = Construction::constructed)
{
    size_t header_length = encoded_header_length(content_length, id);
    if (header_length + content_length > buffer.length())
    {
        return EncodeResult{false, 0};
    }

    assert(buffer.length() >= content_length + content_offset);
    if (content_offset != header_length)
    {
        std::memmove(buffer.data() + header_length, buffer.data() + content_offset, content_length);
    }
    return encode_header(buffer, content_length, id, construction);
}

} // namespace fast_ber
