#pragma once

#include "absl/types/span.h"
#include "fast_ber/ber_types/Class.hpp"
#include "fast_ber/ber_types/Construction.hpp"
#include "fast_ber/util/BerView.hpp"
#include "fast_ber/util/DecodeHelpers.hpp"

#include <iostream>

namespace fast_ber
{

template <const char* parent_name>
inline bool decode_combine_impl(BerViewIterator&) noexcept
{
    return true;
}

template <const char* parent_name, typename... Args, typename T>
bool decode_combine_impl(BerViewIterator& input, T& object, Tag tag, Args&&... args) noexcept
{
    bool success = decode_with_specific_id(input, object, tag);
    if (!success)
    {
        std::cerr << "Error decoding " << parent_name << ": could not decode field with tag " << tag << "\n";
        return false;
    }
    return decode_combine_impl<parent_name>(input, args...);
}

template <const char* parent_name, typename... Args>
bool decode_combine(const BerView& input, Tag tag, Args&&... args) noexcept
{
    if (!input.is_valid())
    {
        std::cerr << "Error decoding " << parent_name << ": Input is not valid ber\n";
        return false;
    }
    if (input.tag() != tag)
    {
        std::cerr << "Error decoding " << parent_name << ": Expected tag = " << tag << " got " << input.tag() << "\n";
        return false;
    }

    auto iterator = input.begin();
    return decode_combine_impl<parent_name>(iterator, args...);
}

} // namespace fast_ber
