#pragma once

#include "fast_ber/ber_types/Class.hpp"
#include "fast_ber/ber_types/Construction.hpp"
#include "fast_ber/ber_types/Tag.hpp"
#include "fast_ber/util/Definitions.hpp"

#include <iosfwd>
#include <tuple>

namespace fast_ber
{

struct RuntimeId
{
    constexpr RuntimeId(Class c, Tag t) : m_class(c), m_tag(t) {}
    constexpr RuntimeId(UniversalTag t) : m_class(Class::universal), m_tag(static_cast<Tag>(t)) {}

    constexpr Class class_() const { return m_class; }
    constexpr Tag   tag() const { return m_tag; }

    Class m_class;
    Tag   m_tag;

    bool operator==(const RuntimeId& rhs) const { return m_class == rhs.m_class && m_tag == rhs.m_tag; }
    bool operator!=(const RuntimeId& rhs) const { return !(*this == rhs); }
};

// Any class or tag is valid
template <Class class_1, Tag tag_1>
struct Id
{
    constexpr static Class class_() { return class_1; }
    constexpr static Tag   tag() { return tag_1; }

    constexpr static size_t depth() { return 1; }
    constexpr static bool   check_id_match(Class c, Tag t) { return c == class_() && t == tag(); }
};

template <typename OuterId, typename InnerId>
struct DoubleId
{
    constexpr static Class   class_() { return OuterId::class_(); }
    constexpr static Tag     tag() { return OuterId::tag(); }
    constexpr static OuterId outer_id() { return {}; }
    constexpr static InnerId inner_id() { return {}; }

    constexpr static size_t depth() { return 2; }
    constexpr static bool   check_id_match(Class c, Tag t) { return c == class_() && t == tag(); }
};

template <Class class_1, Tag tag_1>
bool operator==(Id<class_1, tag_1> lhs, const RuntimeId& rhs)
{
    return lhs.class_() == rhs.class_() && lhs.tag() == rhs.tag();
}

template <Class class_1, Tag tag_1>
bool operator!=(Id<class_1, tag_1> lhs, const RuntimeId& rhs)
{
    return !(lhs == rhs);
}

template <Class class_1, Tag tag_1>
bool operator==(const RuntimeId& lhs, Id<class_1, tag_1> rhs)
{
    return lhs.class_() == rhs.class_() && lhs.tag() == rhs.tag();
}

template <Class class_1, Tag tag_1>
bool operator!=(const RuntimeId& lhs, Id<class_1, tag_1> rhs)
{
    return !(lhs == rhs);
}

// Class is always universal
template <UniversalTag explicit_tag>
using ExplicitId = Id<Class::universal, as_underlying(explicit_tag)>;

template <typename Identifier>
struct InnerIdentifierT;

template <typename Identifier>
struct OuterIdentifierT;

template <typename OuterId, typename InnerId>
struct InnerIdentifierT<DoubleId<OuterId, InnerId>>
{
    using Type = InnerId;
};

template <Class class_, Tag tag>
struct InnerIdentifierT<Id<class_, tag>>
{
    using Type = Id<class_, tag>;
};

template <typename OuterId, typename InnerId>
struct OuterIdentifierT<DoubleId<OuterId, InnerId>>
{
    using Type = OuterId;
};

template <Class class_, Tag tag>
struct OuterIdentifierT<Id<class_, tag>>
{
    using Type = Id<class_, tag>;
};

template <typename Identifier>
using InnerIdentifier = typename InnerIdentifierT<Identifier>::Type;

template <typename Identifier>
using OuterIdentifier = typename OuterIdentifierT<Identifier>::Type;

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

template <typename OuterId, typename InnerId>
constexpr OuterId outer_identifier(DoubleId<OuterId, InnerId>)
{
    return {};
}

template <Class class_, Tag tag>
constexpr Id<class_, tag> outer_identifier(Id<class_, tag>)
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

inline std::ostream& operator<<(std::ostream& os, RuntimeId id) noexcept
{
    if (id.class_() == Class::universal)
    {
        UniversalTag tag = static_cast<UniversalTag>(id.tag());
        return os << "{ \"class\": \"" << id.class_() << "\", \"tag\": \"" << tag << "\"}";
    }
    return os << "{ \"class\": \"" << id.class_() << "\", \"tag\": " << id.tag() << "}";
}

template <Class class_1, Tag tag_1>
std::ostream& operator<<(std::ostream& os, Id<class_1, tag_1> id) noexcept
{
    return os << RuntimeId{id.class_(), id.tag()};
}

template <typename OuterId, typename InnerId>
std::ostream& operator<<(std::ostream& os, DoubleId<OuterId, InnerId> id) noexcept
{
    return os << "{ \"outer\": " << id.outer_id() << ", \"inner\": " << id.inner_id() << "}";
}

} // namespace fast_ber
