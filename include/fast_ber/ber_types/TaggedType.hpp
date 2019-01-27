#pragma once

#include "fast_ber/ber_types/Choice.hpp"
#include "fast_ber/util/DecodeHelpers.hpp"
#include "fast_ber/util/EncodeHelpers.hpp"

namespace fast_ber
{

template <typename Type, typename TagType>
class TaggedType : public Type
{
  public:
    template <typename T1, typename T2>
    TaggedType(const TaggedType<T1, T2>& t) : Type(t)
    {
    }

    template <typename T>
    TaggedType(const T& t) : Type(t)
    {
    }

    // No members, destructor doesn't need to be virtual
    TaggedType()           = default; // Noexcept fails on g++4.8
    ~TaggedType() noexcept = default;
};

template <typename Type, typename TagType>
constexpr TagType identifier(const TaggedType<Type, TagType>*) noexcept
{
    return {};
}

template <typename T, typename DefaultTag, typename ID = DefaultTag>
EncodeResult encode(absl::Span<uint8_t> output, const TaggedType<T, DefaultTag>& object, const ID& id = ID{})
{
    return encode(output, *dynamic_cast<const T*>(&object), id);
}

template <typename T, typename DefaultTag, typename ID = DefaultTag>
bool decode(absl::Span<const uint8_t> output, TaggedType<T, DefaultTag>& object, const ID& id = ID{})
{
    return decode(output, *dynamic_cast<T*>(&object), id);
}

} // namespace fast_ber
