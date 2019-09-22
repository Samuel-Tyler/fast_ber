#pragma once

#include "fast_ber/ber_types/Class.hpp"
#include "fast_ber/ber_types/Construction.hpp"
#include "fast_ber/ber_types/Tag.hpp"

#include <tuple>

namespace fast_ber
{

// Token used to suggest default tagging should be used
struct DefaultTagging
{
};

// Class is always universal
template <UniversalTag ExplicitTag>
struct ExplicitIdentifier
{
    constexpr static Class        class_() { return Class::universal; }
    constexpr static UniversalTag tag() { return ExplicitTag; }
};

// Inner class is always universal
template <Class OuterClass, Tag OuterTag, typename ExplicitTag>
struct TaggedExplicitIdentifier
{
    constexpr static Class class_() { return OuterClass; }
    constexpr static Tag   tag() { return OuterTag; }
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

// Used to ensure identifier() functions can be found in the fast_ber namespace using ADL
struct IdentifierAdlToken
{
};

template <Class T1, Tag T2, typename T3>
constexpr bool is_explicit_tagged(TaggedExplicitIdentifier<T1, T2, T3>)
{
    return true;
}

template <typename Identifier, typename ExplicitIdentifier,
          typename E = typename std::enable_if<is_explicit_tagged(Identifier{})>::type>
constexpr ExplicitIdentifier select_explicit_identifier_impl(Identifier, ExplicitIdentifier)
{
    return {};
}

template <typename Identifier, typename ExplicitIdentifier,
          typename E = typename std::enable_if<!is_explicit_tagged(Identifier{})>::type>
constexpr Identifier select_explicit_identifier_impl(Identifier, ExplicitIdentifier)
{
    return {};
}

template <typename T>
constexpr auto explicit_identifier(const T*, IdentifierAdlToken = IdentifierAdlToken{}) noexcept
    -> decltype(select_explicit_identifier_impl(typename T::Id{}, typename T::ExplicitId{}))
{
    return {};
}

template <typename T, typename E = typename std::enable_if<std::is_same<typename T::Id, DefaultTagging>::value>::type>
constexpr typename T::ExplicitId identifier(const T*, IdentifierAdlToken = IdentifierAdlToken{}) noexcept
{
    return {};
}

template <typename T, typename E = typename std::enable_if<!std::is_same<typename T::Id, DefaultTagging>::value>::type>
constexpr typename T::Id identifier(const T*, IdentifierAdlToken = IdentifierAdlToken{}) noexcept
{
    return {};
}

template <typename T>
constexpr bool is_explicit_tagged(T)
{
    return false;
}

static_assert(is_explicit_tagged(TaggedExplicitIdentifier<Class::universal, 2, DefaultTagging>{}), "!");
static_assert(!is_explicit_tagged(DefaultTagging{}), "!");

} // namespace fast_ber
