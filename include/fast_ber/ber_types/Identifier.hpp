#pragma once

#include "fast_ber/ber_types/Class.hpp"
#include "fast_ber/ber_types/Construction.hpp"
#include "fast_ber/ber_types/Tag.hpp"
#include "fast_ber/util/Definitions.hpp"

#include <iostream>
#include <tuple>

namespace fast_ber
{

template <typename OuterId, typename InnerId>
struct DoubleId
{
    constexpr static bool    check_id_match(Class c, Tag t) { return c == class_() && t == tag(); }
    constexpr static Class   class_() { return OuterId::class_(); }
    constexpr static Tag     tag() { return OuterId::tag(); }
    constexpr static OuterId outer_id() { return {}; }
    constexpr static InnerId inner_id() { return {}; }
};

// Any class or tag is valid
template <Class class_1, Tag tag_1>
struct Id
{
    constexpr static bool  check_id_match(Class c, Tag t) { return c == class_() && t == tag(); }
    constexpr static Class class_() { return class_1; }
    constexpr static Tag   tag() { return tag_1; }
};

// Class is always universal
template <UniversalTag explicit_tag>
using ExplicitId = Id<Class::universal, val(explicit_tag)>;

template <typename OuterId, typename InnerId>
constexpr InnerId inner_identifier(DoubleId<OuterId, InnerId>)
{
    return {};
}

template <Class class_, Tag tag>
constexpr Id<class_, tag> inner_identifier(Id<class_, tag>)
{
    return {};
}

template <typename T>
struct IdentifierType
{
    using type = typename T::AsnId;
};

template <typename T>
using Identifier = typename IdentifierType<T>::type;

template <Class class_1, Tag tag_1>
std::ostream& operator<<(std::ostream& os, const Id<class_1, tag_1>& id) noexcept
{
    if (id.class_() == Class::universal)
    {
        UniversalTag tag = static_cast<UniversalTag>(id.tag());
        return os << "Identifier(" << id.class_() << ", " << tag << ")";
    }
    return os << "Identifier(" << id.class_() << ", " << id.tag() << ")";
}

template <typename OuterId, typename InnerId>
std::ostream& operator<<(std::ostream& os, const DoubleId<OuterId, InnerId>& id) noexcept
{
    return os << "DoubleIdentifier(" << id.outer_id() << ", " << id.inner_id() << ")";
}

} // namespace fast_ber
