#pragma once

#include "absl/types/span.h"
#include "fast_ber/ber_types/Class.hpp"
#include "fast_ber/ber_types/Construction.hpp"
#include "fast_ber/util/BerView.hpp"

#include <iostream>

namespace fast_ber
{

template <typename T>
int val(T t) noexcept
{
    return static_cast<int>(t);
}

template <typename T>
bool primitive_decode_impl(BerViewIterator& input, T& output, Tag tag)
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

bool decode(BerViewIterator& input, Boolean& output, Tag tag) noexcept
{
    return primitive_decode_impl(input, output, tag);
}

bool decode(BerViewIterator& input, Integer& output, Tag tag) noexcept
{
    return primitive_decode_impl(input, output, tag);
}

bool decode(BerViewIterator& input, OctetString& output, Tag tag) noexcept
{
    return primitive_decode_impl(input, output, tag);
}

bool decode_combine_impl(BerViewIterator&) noexcept { return true; }

template <typename... Args, typename T>
bool decode_combine_impl(BerViewIterator& input, T& object, Tag tag, Args&&... args) noexcept
{
    bool success = decode(input, object, tag);
    if (!success)
    {
        return false;
    }
    return decode_combine_impl(input, args...);
}

template <typename... Args>
bool decode_combine(const BerView& input, Tag tag, Args&&... args) noexcept
{
    if (!input.is_valid())
    {
        return false;
    }

    auto iterator = input.begin();
    return decode_combine_impl(iterator, args...);
}

} // namespace fast_ber
