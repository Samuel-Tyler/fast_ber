#pragma once

#include "absl/types/span.h"
#include "fast_ber/util/BerView.hpp"
#include "fast_ber/ber_types/OctetString.hpp"
#include "fast_ber/ber_types/Integer.hpp"
#include "fast_ber/ber_types/Boolean.hpp"

#include <iostream>

namespace fast_ber
{

template <typename T>
bool primitive_decode_impl(BerViewIterator& input, T& output, Tag tag) noexcept
{
    if (!input->is_valid())
    {
        std::cerr << "Invalid packet found when decoding. Expected tag = " << tag << "\n";
        return false;
    }
    if (tag != input->tag())
    {
        std::cerr << "Unexpected tag found when decoding. Expected tag = " << tag << " got " << input->tag() << "\n";
        return false;
    }
    bool success = output.assign_ber(*input) > 0;
    ++input;
    return success;
}

inline bool decode(BerViewIterator& input, Boolean& output, Tag tag) noexcept
{
    return primitive_decode_impl(input, output, tag);
}

inline bool decode(BerViewIterator& input, Integer& output, Tag tag) noexcept
{
    return primitive_decode_impl(input, output, tag);
}

inline bool decode(BerViewIterator& input, OctetString& output, Tag tag) noexcept
{
    return primitive_decode_impl(input, output, tag);
}

} // namespace fast_ber
