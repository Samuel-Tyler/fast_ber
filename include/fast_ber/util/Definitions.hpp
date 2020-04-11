#pragma once

namespace fast_ber
{

enum class EncodingMode
{
    ber,
    der
};

enum class StorageMode
{
    static_,
    small_buffer_optimised,
    dynamic,
};

} // namespace fast_ber
