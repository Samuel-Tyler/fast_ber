#pragma once

#include <cstdint>
#include <iostream>

namespace fast_ber
{

using Tag = int64_t;

enum class UniversalTag : Tag
{
    reserved          = 0,
    boolean           = 1,
    integer           = 2,
    bit_string        = 3,
    octet_string      = 4,
    null              = 5,
    object_identifier = 6,
    object_descriptor = 7,
    instance_of       = 8,
    external          = 8,
    real              = 9,
    enumerated        = 10,
    embedded_pdv      = 11,
    utf8_string       = 12,
    relative_oid      = 13,
    choice            = 16,
    sequence          = 16,
    sequence_of       = 16,
    set               = 17,
    set_of            = 17,
    numeric_string    = 18,
    printable_string  = 19,
    teletex_string    = 20,
    t161_string       = 20,
    videotex_string   = 21,
    ia5_string        = 22,
    utc_time          = 23,
    generalized_time  = 24,
    graphic_string    = 25,
    visible_string    = 26,
    iso646_string     = 26,
    general_string    = 27,
    universal_string  = 28,
    character_string  = 29,
    bmp_string        = 30
};

template <typename T>
constexpr int val(T t) noexcept
{
    return static_cast<int>(t);
}

inline std::ostream& operator<<(std::ostream& os, const UniversalTag& t) noexcept
{
    switch (t)
    {
    case UniversalTag::reserved:
        return os << "Reserved";
    case UniversalTag::boolean:
        return os << "Boolean";
    case UniversalTag::integer:
        return os << "Integer";
    case UniversalTag::bit_string:
        return os << "Bit String";
    case UniversalTag::octet_string:
        return os << "Octet String";
    case UniversalTag::null:
        return os << "Null";
    case UniversalTag::object_identifier:
        return os << "Object Identifier";
    case UniversalTag::object_descriptor:
        return os << "Object Descriptor";
    case UniversalTag::instance_of:
        return os << "Instance Of / External";
    case UniversalTag::real:
        return os << "Real";
    case UniversalTag::enumerated:
        return os << "Enumerated";
    case UniversalTag::embedded_pdv:
        return os << "Embedded_pdv";
    case UniversalTag::utf8_string:
        return os << "UTF8 String";
    case UniversalTag::relative_oid:
        return os << "Relative OID";
    case UniversalTag::sequence:
        return os << "Sequence / Sequence Of";
    case UniversalTag::set:
        return os << "Set Set_of";
    case UniversalTag::numeric_string:
        return os << "Numeric String";
    case UniversalTag::printable_string:
        return os << "Printable String";
    case UniversalTag::teletex_string:
        return os << "Teletex String / T161 String";
    case UniversalTag::videotex_string:
        return os << "Videotex String";
    case UniversalTag::ia5_string:
        return os << "IA5 String";
    case UniversalTag::utc_time:
        return os << "UTC Time";
    case UniversalTag::generalized_time:
        return os << "generalized_time";
    case UniversalTag::graphic_string:
        return os << "graphic_string";
    case UniversalTag::visible_string:
        return os << "visible_string/iso646_string";
    case UniversalTag::general_string:
        return os << "General String";
    case UniversalTag::universal_string:
        return os << "Universal String";
    case UniversalTag::character_string:
        return os << "Character String";
    case UniversalTag::bmp_string:
        return os << "BMP String";
    default:
        return os << "Unknown Universal Tag";
    }
}

} // namespace fast_ber
