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
    TaggedType(const Type& t) : Type(t) {}
    TaggedType(Type&& t) : Type(std::move(t)) {}
    template <typename... T>
    TaggedType(T&&... t) : Type{std::forward<T>(t)...}
    {
    }
    template <typename T1, typename T2>
    TaggedType(const TaggedType<T1, T2>& t) : Type(t.get())
    {
    }

    Type&       get() { return *this; }
    const Type& get() const { return *this; }

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
    Type&       get() { return enumerated; }
    const Type& get() const { return enumerated; }

    using Id       = TagType;
    using BaseType = Type;
};

template <typename Type, typename TagType>
struct TaggedType<fast_ber::Integer<Type>, TagType> : fast_ber::Integer<TagType>
{
    TaggedType(const fast_ber::Integer<TagType>& t) : fast_ber::Integer<TagType>(t) {}
    TaggedType(Type&& t) : fast_ber::Integer<TagType>(std::move(t)) {}
    template <typename... T>
    TaggedType(T&&... t) : fast_ber::Integer<TagType>{std::forward<T>(t)...}
    {
    }
    template <typename T1, typename T2>
    TaggedType(const TaggedType<T1, T2>& t) : fast_ber::Integer<TagType>(t.get())
    {
    }

    fast_ber::Integer<TagType>&       get() { return *this; }
    const fast_ber::Integer<TagType>& get() const { return *this; }

    using Id       = TagType;
    using BaseType = Type;
};

template <typename T1, typename T2, typename T3>
bool operator==(const TaggedType<T1, T2>& lhs, const T3& rhs)
{
    return lhs.get() == rhs;
}

template <typename T1, typename T2, typename T3>
bool operator==(const T1& lhs, const TaggedType<T2, T3>& rhs)
{
    return lhs == rhs.get();
}

template <typename T1, typename T2, typename T3, typename T4>
bool operator==(const TaggedType<T1, T2>& lhs, const TaggedType<T3, T4>& rhs)
{
    return lhs.get() == rhs.get();
}

template <typename T1, typename T2, typename T3>
bool operator!=(const TaggedType<T1, T2>& lhs, const T3& rhs)
{
    return lhs.get() != rhs;
}

template <typename T1, typename T2, typename T3>
bool operator!=(const T1& lhs, const TaggedType<T2, T3>& rhs)
{
    return lhs != rhs.get();
}

template <typename T1, typename T2, typename T3, typename T4>
bool operator!=(const TaggedType<T1, T2>& lhs, const TaggedType<T3, T4>& rhs)
{
    return lhs.get() != rhs.get();
}

template <typename T, typename DefaultTag, typename ID = typename TaggedType<T, DefaultTag>::Id>
EncodeResult encode(absl::Span<uint8_t> output, const TaggedType<T, DefaultTag>& object, ID id = ID{})
{
    return encode(output, object.get(), id);
}

template <typename T, typename DefaultTag, typename ID = typename TaggedType<T, DefaultTag>::Id>
DecodeResult decode(BerViewIterator& input, TaggedType<T, DefaultTag>& object, ID id = ID{})
{
    return decode(input, object.get(), id);
}

} // namespace fast_ber
