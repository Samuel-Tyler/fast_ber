#pragma once

#include <cstdint>

namespace fast_ber
{

enum class Construction
{
    primitive   = 0,
    constructed = 1
};

constexpr inline Construction get_construction(uint8_t first_byte)
{
    return (first_byte & 0x20) ? Construction::constructed : Construction::primitive;
}

inline void set_construction(uint8_t& first_byte, Construction construction)
{
    first_byte &= 0xDF;
    first_byte |= (static_cast<int>(construction) << 5);
}

} // namespace fast_ber
