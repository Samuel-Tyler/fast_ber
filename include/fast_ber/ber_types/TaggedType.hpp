#pragma once

#include "fast_ber/ber_types/Identifier.hpp"
#include "fast_ber/util/DecodeHelpers.hpp"
#include "fast_ber/util/EncodeHelpers.hpp"

namespace fast_ber
{

template <typename OriginalIdentifier, typename OverrideIndetifier,
          typename E = typename std::enable_if<is_explicit_tagged(OriginalIdentifier{})>::type>
constexpr auto resultant_identifier(OriginalIdentifier, OverrideIndetifier override)
    -> TaggedExplicitIdentifier<override.class_(), override.tag(), DefaultTagging>
{
    return {};
}

template <typename OriginalIdentifier, typename OverrideIndetifier,
          typename E = typename std::enable_if<!is_explicit_tagged(OriginalIdentifier{})>::type>
constexpr auto resultant_identifier(OriginalIdentifier, OverrideIndetifier) -> OverrideIndetifier
{
    return {};
}

/*
template <typename OriginalIdentifier, OverrideIndetifier>
    auto resultant_type*/

template <typename Type, typename TagType, typename Enable = void>
class TaggedType : public Type
{
  public:
    template <typename T1, typename T2>
    TaggedType(const TaggedType<T1, T2>& t) : Type(t)
    {
    }

    template <typename... T>
    TaggedType(T&&... t) : Type{std::forward<T>(t)...}
    {
    }

    // No members, destructor doesn't need to be virtual
    TaggedType()           = default; // Noexcept fails on g++4.8
    ~TaggedType() noexcept = default;

    Type&       get() { return *this; }
    const Type& get() const { return *this; }

    using ExplicitId = decltype(explicit_identifier(static_cast<Type*>(nullptr), IdentifierAdlToken{}));
    using Id         = decltype(resultant_identifier(TaggedType::ExplicitId{}, TagType{}));
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

    using ExplicitId = decltype(explicit_identifier(static_cast<Type*>(nullptr), IdentifierAdlToken{}));
    using Id         = decltype(resultant_identifier(TaggedType::ExplicitId{}, TagType{}));
};

template <typename Type, typename TagType>
constexpr auto identifier(const TaggedType<Type, TagType>*, IdentifierAdlToken = IdentifierAdlToken{}) noexcept ->
    typename TaggedType<Type, TagType>::Identifier
{
    return {};
}

template <typename Type, typename TagType, typename std::enable_if<std::is_enum<Type>::value>::type>
constexpr auto identifier(const TaggedType<Type, TagType>*, IdentifierAdlToken = IdentifierAdlToken{}) noexcept ->
    typename TaggedType<Type, TagType>::Identifier
{
    return {};
}

template <typename T, typename DefaultTag, typename ID = DefaultTag>
EncodeResult encode(absl::Span<uint8_t> output, const TaggedType<T, DefaultTag>& object, ID id = ID{})
{
    return encode(output, object.get(), id);
}

template <typename T, typename DefaultTag, typename ID = DefaultTag>
DecodeResult decode(BerViewIterator& input, TaggedType<T, DefaultTag>& object, ID id = ID{})
{
    return decode(input, object.get(), id);
}

} // namespace fast_ber
