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
    using Type::operator=;

    TaggedType() = default;
    TaggedType(const Type& t) : Type(t) {}
    TaggedType(Type&& t) : Type(std::move(t)) {}
    TaggedType(const TaggedType&) = default;
    TaggedType(TaggedType&&)      = default;
    template <typename T1, typename T2>
    TaggedType(const TaggedType<T1, T2>& t) : Type(t.get_base())
    {
    }

    TaggedType& operator=(const TaggedType&) = default;

    Type&       get_base() { return *this; }
    const Type& get_base() const { return *this; }

    using AsnId    = TagType;
    using BaseType = Type;
};

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
