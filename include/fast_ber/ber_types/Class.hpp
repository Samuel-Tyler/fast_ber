#pragma once

#include <cstdint>
#include <iostream>

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
    return static_cast<uint8_t>((static_cast<uint8_t>(class_) << 6) | (first_byte & 0x3F));
}

inline std::ostream& operator<<(std::ostream& os, const Class& c) noexcept
{
    switch (c)
    {
    case Class::universal:
        return os << "Universal";
    case Class::application:
        return os << "Application";
    case Class::context_specific:
        return os << "Context Specific";
    case Class::private_:
        return os << "Private";
    default:
        return os << "Unknown Class";
    }
}

} // namespace fast_ber
