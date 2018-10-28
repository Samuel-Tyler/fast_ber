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

inline Class get_class(uint8_t first_byte) { return static_cast<Class>(first_byte >> 6); }

inline void set_class(uint8_t& first_byte, Class class_)
{
    first_byte &= 0x3F;
    first_byte |= (static_cast<int>(class_) << 6);
}

} // namespace fast_ber
