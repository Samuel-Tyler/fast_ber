#pragma once

#include <cstddef>

namespace fast_ber
{

struct EncodeResult
{
    bool   success;
    size_t encode_length;
};

} // namespace fast_ber
