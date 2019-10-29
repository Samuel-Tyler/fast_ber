#pragma once

#include "fast_ber/ber_types/Class.hpp"
#include "fast_ber/ber_types/Construction.hpp"
#include "fast_ber/ber_types/Tag.hpp"

#include <iostream>
#include <tuple>

namespace fast_ber
{

// Class is always universal
template <UniversalTag explicit_tag>
struct ExplicitIdentifier
{
    constexpr static Class        class_() { return Class::universal; }
    constexpr static UniversalTag tag() { return explicit_tag; }
};

template <Class OuterClass, Tag OuterTag, typename ExplicitTag>
struct TaggedExplicitIdentifier
{
    constexpr static Class       class_() { return OuterClass; }
    constexpr static Tag         tag() { return OuterTag; }
    constexpr static ExplicitTag inner_id() { return {}; }
};

// Any class or tag is valid
template <Class ImplicitClass, Tag ImplicitTag>
struct ImplicitIdentifier
{
    constexpr static Class class_() { return ImplicitClass; }
    constexpr static Tag   tag() { return ImplicitTag; }
};

template <UniversalTag T>
constexpr Tag reference_tag(const ExplicitIdentifier<T>& id)
{
    return val(id.tag());
}

template <UniversalTag ExplicitTag>
constexpr ExplicitIdentifier<ExplicitTag> inner_identifier(ExplicitIdentifier<ExplicitTag>)
{
    return {};
}

template <Class OuterClass, Tag OuterTag, typename ExplicitTag>
constexpr ExplicitTag inner_identifier(TaggedExplicitIdentifier<OuterClass, OuterTag, ExplicitTag>)
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

// Used to ensure identifier() functions can be found in the fast_ber namespace using ADL
struct IdentifierAdlToken
{
};

template <UniversalTag ExplicitTag>
constexpr bool is_explicit(ExplicitIdentifier<ExplicitTag>)
{
    return true;
}

template <typename T>
constexpr bool is_explicit(T)
{
    return false;
}

template <Class T1, Tag T2, typename T3>
constexpr bool is_explicit_tagged(TaggedExplicitIdentifier<T1, T2, T3>)
{
    return true;
}

template <typename T>
constexpr bool is_explicit_tagged(T)
{
    return false;
}

template <typename T>
constexpr auto identifier(const T*, IdentifierAdlToken = IdentifierAdlToken{}) noexcept -> typename T::Id
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

template <UniversalTag ExplicitTag>
std::ostream& operator<<(std::ostream& os, const ExplicitIdentifier<ExplicitTag>& id) noexcept
{
    return os << "Explicit(" << id.tag() << ", " << id.class_() << ")";
}

template <Class OuterClass, Tag OuterTag, typename ExplicitTag>
std::ostream& operator<<(std::ostream&                                                      os,
                         const TaggedExplicitIdentifier<OuterClass, OuterTag, ExplicitTag>& id) noexcept
{
    return os << "TaggedExplicit(" << id.tag() << ", " << id.class_() << ", " << id.inner_id() << ")";
}

template <Class ImplicitClass, Tag ImplicitTag>
std::ostream& operator<<(std::ostream& os, const ImplicitIdentifier<ImplicitClass, ImplicitTag>& id) noexcept
{
    return os << "Implicit(" << id.tag() << ", " << id.class_() << ")";
}

namespace abbreviations
{
template <UniversalTag explicit_tag>
using EId = ExplicitIdentifier<explicit_tag>;
template <Class OuterClass, Tag OuterTag, typename ExplicitTag>
using TId = TaggedExplicitIdentifier<OuterClass, OuterTag, ExplicitTag>;
template <Class ImplicitClass, Tag ImplicitTag>
using IId = ImplicitIdentifier<ImplicitClass, ImplicitTag>;
} // namespace abbreviations

} // namespace fast_ber
