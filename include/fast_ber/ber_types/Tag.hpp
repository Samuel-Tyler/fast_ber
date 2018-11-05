#pragma once

namespace fast_ber
{

using Tag = long;

enum class UniversalTag : Tag
{
    sequence_of = 16,
};

template <typename T>
constexpr int val(T t) noexcept
{
    return static_cast<int>(t);
}

} // namespace fast_ber
