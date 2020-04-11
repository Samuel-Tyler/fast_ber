#pragma once

#include <cstdint>
#include <iostream>

namespace fast_ber
{

enum class Construction
{
    primitive   = 0,
    constructed = 1
};

constexpr size_t construction_offset = 5;
constexpr size_t construction_mask = 0x20;

constexpr inline Construction get_construction(uint8_t first_byte)
{
    return (first_byte & construction_mask) ? Construction::constructed : Construction::primitive;
}

constexpr inline uint8_t add_construction(uint8_t first_byte, Construction construction)

{
    return static_cast<uint8_t>((static_cast<uint8_t>(construction) << construction_offset) | (first_byte & ~construction_mask));
}

inline std::ostream& operator<<(std::ostream& os, const Construction& c) noexcept
{
    switch (c)
    {
    case Construction::primitive:
        return os << "Primitive";
    case Construction::constructed:
        return os << "Constructed";
    default:
        return os << "Unknown Construction";
    }
}

} // namespace fast_ber
