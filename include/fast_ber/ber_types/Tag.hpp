#pragma once

#include <cstdint>

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

} // namespace fast_ber
