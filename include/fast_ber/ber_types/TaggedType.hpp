#pragma once

#include "fast_ber/ber_types/Choice.hpp"
#include "fast_ber/util/DecodeHelpers.hpp"
#include "fast_ber/util/EncodeHelpers.hpp"

namespace fast_ber
{

template <typename Type, typename TagType, typename Enable = void>
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

    Type&       get() { return *this; }
    const Type& get() const { return *this; }
};

// Special template required for enums as they can't be inhereted from
template <typename Type, typename TagType>
struct TaggedType<Type, TagType, typename std::enable_if<std::is_enum<Type>::value>::type>
{
    Type enumerated;

    TaggedType() = default;
    TaggedType(const Type& t) : enumerated(t) {}
    TaggedType& operator=(const Type& t)
    {
        enumerated = t;
        return *this;
    }

                operator Type&() { return enumerated; }
                operator const Type&() const { return enumerated; }
    Type&       get() { return enumerated; }
    const Type& get() const { return enumerated; }
};

template <typename Type, typename TagType>
constexpr TagType identifier(const TaggedType<Type, TagType>*) noexcept
{
    return {};
}

template <typename T, typename DefaultTag, typename ID = DefaultTag>
EncodeResult encode(absl::Span<uint8_t> output, const TaggedType<T, DefaultTag>& object, const ID& id = ID{})
{
    return encode(output, object.get(), id);
}

template <typename T, typename DefaultTag, typename ID = DefaultTag>
DecodeResult decode(absl::Span<const uint8_t> output, TaggedType<T, DefaultTag>& object, const ID& id = ID{})
{
    return decode(output, object.get(), id);
}

} // namespace fast_ber
