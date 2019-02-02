#pragma once

#include "fast_ber/ber_types/Class.hpp"
#include "fast_ber/ber_types/Construction.hpp"
#include "fast_ber/ber_types/Identifier.hpp"
#include "fast_ber/ber_types/Tag.hpp"

#include "absl/types/span.h"

namespace fast_ber
{

// Encode ber tag in provided buffer
// Return the size of the encoded tag field or zero on fail
inline size_t encode_tag(absl::Span<uint8_t> output, Tag tag) noexcept;
inline size_t encode_tag(absl::Span<uint8_t> output, UniversalTag tag) noexcept;
inline size_t encoded_tag_length(Tag tag) noexcept;
inline size_t encoded_tag_length(UniversalTag tag) noexcept;

// Encode ber identifier octets
// Return the size of the encoded tag field or zero on fail
inline size_t encode_identifier(absl::Span<uint8_t> output, Construction construction, Class class_, Tag tag) noexcept;
inline size_t encode_identifier(absl::Span<uint8_t> output, Construction construction, Class class_,
                                UniversalTag tag) noexcept;
inline size_t encoded_identifier_length(Construction construction, Class class_, Tag tag) noexcept;
inline size_t encoded_identifier_length(Construction construction, Class class_, UniversalTag tag) noexcept;

// Encode ber length octets
// Return the size of the encoded tag field or zero on fail
inline size_t encode_length(absl::Span<uint8_t> output, uint64_t length) noexcept;
inline size_t encoded_length_length(size_t length) noexcept;

// Encode a ber header consisting of construction, class, tag and size
// Return the size of the encoded header or zero on fail
inline size_t encode_header(absl::Span<uint8_t> output, Construction construction, Class class_, Tag tag,
                            size_t length) noexcept;
inline size_t encode_header(absl::Span<uint8_t> output, Construction construction, Class class_, UniversalTag tag,
                            size_t length) noexcept;
inline size_t encoded_header_length(Construction construction, Class class_, Tag tag, size_t length) noexcept;
inline size_t encoded_header_length(Construction construction, Class class_, UniversalTag tag, size_t length) noexcept;

constexpr inline uint8_t add_short_tag(uint8_t first_byte, Tag tag) noexcept { return tag | (first_byte & 0xE0); }

inline size_t encode_tag(absl::Span<uint8_t> output, Tag tag) noexcept
{
    if (tag < 0 || output.length() == 0)
    {
        return 0;
    }

    if (tag <= 30)
    {
        output[0] = add_short_tag(output[0], tag);
        return 1;
    }
    else
    {
        output[0] |= 0x1F;

        for (size_t i = 1; i < output.size(); i++)
        {
            output[i] = tag % 0x80;
            tag /= 0x80;

            if (i != 1)
            {
                output[i] |= 0x80;
            }

            if (tag == 0)
            {
                std::reverse(output.data() + 1, output.data() + 1 + i);
                return i + 1;
            }
        }
        return 0;
    }
}
inline size_t encode_tag(absl::Span<uint8_t> output, UniversalTag tag) noexcept { return encode_tag(output, val(tag)); }
inline size_t encoded_tag_length(Tag tag) noexcept
{
    if (tag < 0)
    {
        return 0;
    }

    if (tag <= 30)
    {
        return 1;
    }
    else
    {
        for (size_t i = 1;; i++)
        {
            tag /= 0x80;
            if (tag == 0)
            {
                return i + 1;
            }
        }
    }
}
inline size_t encoded_tag_length(UniversalTag tag) noexcept { return encoded_tag_length(val(tag)); }

constexpr inline uint8_t encode_short_identifier(Construction construction, Class class_, Tag tag)
{
    return add_short_tag(add_class(add_construction(0, construction), class_), tag);
}
constexpr inline uint8_t encode_short_identifier(Construction construction, Class class_, UniversalTag tag)
{
    return add_short_tag(add_class(add_construction(0, construction), class_), val(tag));
}
inline size_t encode_identifier(absl::Span<uint8_t> output, Construction construction, Class class_, Tag tag) noexcept
{
    if (output.size() < 1)
    {
        return false;
    }

    output[0] = 0;

    output[0] = add_construction(output[0], construction);
    output[0] = add_class(output[0], class_);

    return encode_tag(output, tag);
}
inline size_t encode_identifier(absl::Span<uint8_t> output, Construction construction, Class class_,
                                UniversalTag tag) noexcept
{
    return encode_identifier(output, construction, class_, val(tag));
}
inline size_t encoded_identifier_length(Construction, Class, Tag tag) noexcept { return encoded_tag_length(tag); }
inline size_t encoded_identifier_length(Construction, Class, UniversalTag tag) noexcept
{
    return encoded_tag_length(tag);
}

inline size_t encode_length(absl::Span<uint8_t> output, uint64_t length) noexcept
{
    if (length < 0x80 && output.length() > 0)
    {
        output[0] = (length >> 0) & 0xFF;
        return 1;
    }
    else if (length <= 0xFF && output.length() > 1)
    {
        output[0] = 0x81;
        output[1] = (length >> 0) & 0xFF;
        return 2;
    }
    else if (length <= 0xFFFF && output.length() > 2)
    {
        output[0] = 0x82;
        output[1] = (length >> 8) & 0xFF;
        output[2] = (length >> 0) & 0xFF;
        return 3;
    }
    else if (length <= 0xFFFFFF && output.length() > 3)
    {
        output[0] = 0x83;
        output[1] = (length >> 16) & 0xFF;
        output[2] = (length >> 8) & 0xFF;
        output[3] = (length >> 0) & 0xFF;
        return 4;
    }
    else if (length <= 0xFFFFFFFF && output.length() > 4)
    {
        output[0] = 0x84;
        output[1] = (length >> 24) & 0xFF;
        output[2] = (length >> 16) & 0xFF;
        output[3] = (length >> 8) & 0xFF;
        output[4] = (length >> 0) & 0xFF;
        return 5;
    }
    else if (length <= 0xFFFFFFFFFF && output.length() > 5)
    {
        output[0] = 0x85;
        output[1] = (length >> 32) & 0xFF;
        output[2] = (length >> 24) & 0xFF;
        output[3] = (length >> 16) & 0xFF;
        output[4] = (length >> 8) & 0xFF;
        output[5] = (length >> 0) & 0xFF;
        return 6;
    }
    else if (length <= 0xFFFFFFFFFFFF && output.length() > 6)
    {
        output[0] = 0x86;
        output[1] = (length >> 40) & 0xFF;
        output[2] = (length >> 32) & 0xFF;
        output[3] = (length >> 24) & 0xFF;
        output[4] = (length >> 16) & 0xFF;
        output[5] = (length >> 8) & 0xFF;
        output[6] = (length >> 0) & 0xFF;
        return 7;
    }
    else if (length <= 0xFFFFFFFFFFFFFF && output.length() > 7)
    {
        output[0] = 0x87;
        output[1] = (length >> 48) & 0xFF;
        output[2] = (length >> 40) & 0xFF;
        output[3] = (length >> 32) & 0xFF;
        output[4] = (length >> 24) & 0xFF;
        output[5] = (length >> 16) & 0xFF;
        output[6] = (length >> 8) & 0xFF;
        output[7] = (length >> 0) & 0xFF;
        return 8;
    }
    else if (length <= 0xFFFFFFFFFFFFFFFF && output.length() > 8)
    {
        output[0] = 0x88;
        output[1] = (length >> 56) & 0xFF;
        output[2] = (length >> 48) & 0xFF;
        output[3] = (length >> 40) & 0xFF;
        output[4] = (length >> 32) & 0xFF;
        output[5] = (length >> 24) & 0xFF;
        output[6] = (length >> 16) & 0xFF;
        output[7] = (length >> 8) & 0xFF;
        output[8] = (length >> 0) & 0xFF;
        return 9;
    }

    return 0;
}
inline size_t encoded_length_length(size_t length) noexcept
{
    if (length < 0x80)
    {
        return 1;
    }
    else if (length <= 0xFF)
    {
        return 2;
    }
    else if (length <= 0xFFFF)
    {
        return 3;
    }
    else if (length <= 0xFFFFFF)
    {
        return 4;
    }
    else if (length <= 0xFFFFFFFF)
    {
        return 5;
    }
    else if (length <= 0xFFFFFFFFFF)
    {
        return 6;
    }
    else if (length <= 0xFFFFFFFFFFFF)
    {
        return 7;
    }
    else if (length <= 0xFFFFFFFFFFFFFF)
    {
        return 8;
    }
    else if (length <= 0xFFFFFFFFFFFFFFFF)
    {
        return 9;
    }

    return 0;
}

inline size_t encode_header(absl::Span<uint8_t> output, Construction construction, Class class_, Tag tag,
                            size_t length) noexcept
{
    if (output.size() < 2)
    {
        return false;
    }

    size_t id_length = encode_identifier(output, construction, class_, tag);
    output.remove_prefix(id_length);
    size_t length_length = encode_length(output, length);

    if (id_length == 0 || length_length == 0)
    {
        return 0;
    }

    return id_length + length_length;
}
inline size_t encode_header(absl::Span<uint8_t> output, Construction construction, Class class_, UniversalTag tag,
                            size_t length) noexcept
{
    return encode_header(output, construction, class_, val(tag), length);
}
inline size_t encoded_header_length(Construction construction, Class class_, Tag tag, size_t length) noexcept
{
    return encoded_identifier_length(construction, class_, tag) + encoded_length_length(length);
}
inline size_t encoded_header_length(Construction construction, Class class_, UniversalTag tag, size_t length) noexcept
{
    return encoded_identifier_length(construction, class_, tag) + encoded_length_length(length);
}

} // namespace fast_ber
