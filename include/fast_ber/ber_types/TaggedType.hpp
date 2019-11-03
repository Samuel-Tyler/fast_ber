#pragma once

#include "fast_ber/ber_types/Identifier.hpp"
#include "fast_ber/util/DecodeHelpers.hpp"
#include "fast_ber/util/EncodeHelpers.hpp"

#include "fast_ber/ber_types/Integer.hpp"

namespace fast_ber
{

template <typename Type, typename TagType, typename Enable = void>
struct TaggedType : public Type
{
    using Type::Type;

    TaggedType() = default;
    TaggedType(const Type& t) : Type(t) {}
    TaggedType(Type&& t) : Type(std::move(t)) {}

    template <typename T1, typename T2>
    TaggedType(const TaggedType<T1, T2>& t) : Type(t.get_base())
    {
    }

    Type&       get_base() { return *this; }
    const Type& get_base() const { return *this; }

    using Id       = TagType;
    using BaseType = Type;
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
    bool operator==(const Type& t) const { return enumerated == t; }
    bool operator!=(const Type& t) const { return enumerated != t; }

                operator Type&() { return enumerated; }
                operator const Type&() const { return enumerated; }
    Type&       get_base() { return enumerated; }
    const Type& get_base() const { return enumerated; }

    using Id       = TagType;
    using BaseType = Type;
};

/*
template <typename Type, typename TagType>
struct TaggedType<fast_ber::Integer<Type>, TagType> : fast_ber::Integer<TagType>
{
    TaggedType(const fast_ber::Integer<TagType>& t) : fast_ber::Integer<TagType>(t) {}
    TaggedType(Type&& t) : fast_ber::Integer<TagType>(std::move(t)) {}
    template <typename... Args>
    TaggedType(Args&&... t) : fast_ber::Integer<TagType>{std::forward<Args>(t)...}
    {
    }
    template <typename T1, typename T2>
    TaggedType(const TaggedType<T1, T2>& t) : fast_ber::Integer<TagType>(t.get_base())
    {
    }

    fast_ber::Integer<TagType>&       get_base() { return *this; }
    const fast_ber::Integer<TagType>& get_base() const { return *this; }

    using Id       = TagType;
    using BaseType = Type;
};*/

template <typename T, typename TagType, typename ID = TagType>
EncodeResult encode(absl::Span<uint8_t> output, const TaggedType<T, TagType>& object, ID id = ID{})
{
    return encode(output, object.get_base(), id);
}

template <typename T, typename TagType, typename ID = TagType>
DecodeResult decode(BerViewIterator& input, TaggedType<T, TagType>& object, ID id = ID{})
{
    return decode(input, object.get_base(), id);
}

} // namespace fast_ber
