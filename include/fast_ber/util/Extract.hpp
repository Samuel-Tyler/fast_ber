#pragma once

#include "fast_ber/ber_types/Class.hpp"
#include "fast_ber/ber_types/Construction.hpp"

#include "absl/types/span.h"

namespace fast_ber
{

// Extract the construction of a ber packet
// Return true on success, false on fail
inline bool extract_construction(absl::Span<const uint8_t> input, Construction& construction) noexcept;

// Extract the class of a ber packet
// Return true on success, false on fail
inline bool extract_class(absl::Span<const uint8_t> input, Class& class_) noexcept;

// Extract the tag of a ber packet
// Return length of the identifier octets on success, false on fail
inline size_t extract_tag(absl::Span<const uint8_t> input, long& tag) noexcept;

// Extract the length of a ber packet
// Return the length of the length octets on success, false on fail
inline size_t extract_length(absl::Span<const uint8_t> input, uint64_t& length, size_t length_offset) noexcept;

inline bool extract_construction(absl::Span<const uint8_t> input, Construction& construction) noexcept
{
    if (input.size() == 0)
    {
        return false;
    }

    construction = get_construction(input[0]);
    return true;
}

inline bool extract_class(absl::Span<const uint8_t> input, Class& class_) noexcept
{
    if (input.size() == 0)
    {
        return false;
    }

    class_ = get_class(input[0]);
    return true;
}

inline size_t extract_tag(absl::Span<const uint8_t> input, long& tag) noexcept
{
    if (input.size() == 0)
    {
        return false;
    }

    if ((input[0] & 0x1F) != 0x1F)
    {
        tag = input[0] & 0x1F;
        return true;
    }
    else
    {
        tag = 0;
        for (size_t i = 1; i < input.length(); i++)
        {
            tag += (input[i] & 0x7F);

            if ((input[i] & 0x80) == 0)
            {
                return true;
            }

            tag *= 0x80;
        }
        return false;
    }
}

inline size_t extract_length(absl::Span<const uint8_t> input, uint64_t& length, size_t length_offset) noexcept
{
    input.remove_prefix(length_offset);
    if (input.size() == 0)
    {
        return 0;
    }

    if ((input[0] & 0x80) == 0)
    {
        length = input[0];
        return 1;
    }
    else
    {
        const size_t tag_length = input[0] & 0x7F;
        if (tag_length > 8 || tag_length == 0 || tag_length + 1 > input.length())
        {
            return false;
        }

        length = 0;
        for (size_t i = 1; i < tag_length + 1; i++)
        {
            length *= 0x100;
            length += input[i];
        }
        return tag_length + 1;
    }
}

} // namespace fast_ber
