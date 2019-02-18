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

constexpr inline uint8_t add_construction(uint8_t first_byte, Construction construction)

{
    return static_cast<uint8_t>((static_cast<uint8_t>(construction) << 5) | (first_byte & 0xDF));
}

} // namespace fast_ber
