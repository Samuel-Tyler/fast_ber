#pragma once

#include "fast_ber/ber_types/Class.hpp"
#include "fast_ber/ber_types/Construction.hpp"
#include "fast_ber/ber_types/Tag.hpp"

#include <iostream>
#include <tuple>

namespace fast_ber
{

template <typename OuterId, typename InnerId>
struct DoubleId
{
    constexpr static Class   class_() { return OuterId::class_(); }
    constexpr static Tag     tag() { return OuterId::tag(); }
    constexpr static OuterId outer_id() { return {}; }
    constexpr static InnerId inner_id() { return {}; }
};

// Any class or tag is valid
template <Class ImplicitClass, Tag ImplicitTag>
struct ImplicitIdentifier
{
    constexpr static Class class_() { return ImplicitClass; }
    constexpr static Tag   tag() { return ImplicitTag; }
};

// Class is always universal
template <UniversalTag explicit_tag>
using ExplicitId = ImplicitIdentifier<Class::universal, val(explicit_tag)>;

template <typename OuterId, typename InnerId>
constexpr InnerId inner_identifier(DoubleId<OuterId, InnerId>)
{
    return {};
}

template <Class ImplicitClass, Tag ImplicitTag>
constexpr ImplicitIdentifier<ImplicitClass, ImplicitTag>
    inner_identifier(ImplicitIdentifier<ImplicitClass, ImplicitTag>)
{
    return {};
}

template <Class ImplicitClass, Tag ImplicitTag, typename Default>
constexpr ImplicitIdentifier<ImplicitClass, ImplicitTag> resolve_default(ImplicitIdentifier<ImplicitClass, ImplicitTag>,
                                                                         Default)
{
    return {};
}

template <typename T>
struct IdentifierType
{
    using type = typename T::Id;
};

template <typename T>
using Identifier = typename IdentifierType<T>::type;

template <Class ImplicitClass, Tag ImplicitTag>
std::ostream& operator<<(std::ostream& os, const ImplicitIdentifier<ImplicitClass, ImplicitTag>& id) noexcept
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

namespace abbreviations
{
template <Class ImplicitClass, Tag ImplicitTag>
using Id = ImplicitIdentifier<ImplicitClass, ImplicitTag>;
} // namespace abbreviations

} // namespace fast_ber
