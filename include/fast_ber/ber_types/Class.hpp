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

constexpr size_t class_offset = 6;
constexpr size_t class_mask   = 0xC0;

constexpr inline Class get_class(uint8_t first_byte) { return static_cast<Class>(first_byte >> class_offset); }

constexpr inline uint8_t add_class(uint8_t first_byte, Class class_)
{
    return static_cast<uint8_t>((static_cast<uint8_t>(class_) << class_offset) | (first_byte & ~class_mask));
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

namespace abbreviations
{
constexpr Class uni = Class::universal;
constexpr Class app = Class::application;
constexpr Class ctx = Class::context_specific;
constexpr Class pri = Class::private_;
} // namespace abbreviations

} // namespace fast_ber
