#pragma once

#include <cstdint>

namespace fast_ber
{

enum class Class
{
    universal        = 0,
    application      = 1,
    context_specific = 2,
    private_         = 3
};

constexpr inline Class get_class(uint8_t first_byte) { return static_cast<Class>(first_byte >> 6); }

constexpr inline uint8_t add_class(uint8_t first_byte, Class class_)
{
    return (static_cast<int>(class_) << 6) | (first_byte & 0x3F);
}

} // namespace fast_ber
