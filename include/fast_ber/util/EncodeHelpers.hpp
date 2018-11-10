#pragma once

#include <cstddef>

#include "fast_ber/ber_types/Boolean.hpp"
#include "fast_ber/ber_types/Identifier.hpp"
#include "fast_ber/ber_types/Integer.hpp"
#include "fast_ber/ber_types/OctetString.hpp"

namespace fast_ber
{

struct EncodeResult
{
    bool   success;
    size_t length;
};

inline EncodeResult wrap_with_ber_header(absl::Span<uint8_t> output, Class class_, int tag,
                                         size_t content_length) noexcept
{
    std::array<uint8_t, 30> buffer;
    size_t header_length = create_header(absl::MakeSpan(buffer.data(), buffer.size()), Construction::constructed,
                                         class_, tag, content_length);
    assert(header_length != 0);
    if (header_length + content_length > output.length())
    {
        return EncodeResult{false, 0};
    }

    std::memmove(output.data() + header_length, output.data(), content_length);
    std::memcpy(output.data(), buffer.data(), header_length);
    return EncodeResult{true, header_length + content_length};
}

template <typename ID>
EncodeResult encode_with_specific_id(absl::Span<uint8_t>& output, const Integer& object, const ID& id)
{
    size_t encode_length = object.encode_with_specific_id(output, id.class_, val(id.tag));
    return EncodeResult{encode_length > 0, encode_length};
}

template <typename ID>
EncodeResult encode_with_specific_id(absl::Span<uint8_t>& output, const OctetString& object, const ID& id)
{
    size_t encode_length = object.encode_with_specific_id(output, id.class_, val(id.tag));
    return EncodeResult{encode_length > 0, encode_length};
}

template <typename ID>
EncodeResult encode_with_specific_id(absl::Span<uint8_t>& output, const Boolean& object, const ID& id)
{
    size_t encode_length = object.encode_with_specific_id(output, id.class_, val(id.tag));
    return EncodeResult{encode_length > 0, encode_length};
}

} // namespace fast_ber
