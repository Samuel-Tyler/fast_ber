#pragma once

#include "fast_ber/ber_types/Class.hpp"
#include "fast_ber/ber_types/Construction.hpp"
#include "fast_ber/ber_types/Tag.hpp"

#include <tuple>

namespace fast_ber
{

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
    constexpr static Class       outer_class() { return OuterClass; }
    constexpr static Tag         outer_tag() { return OuterTag; }
    constexpr static ExplicitTag inner_id() { return ExplicitTag(); }
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

template <Class T1, Tag T2, typename T3>
constexpr Tag reference_tag(const TaggedExplicitIdentifier<T1, T2, T3>& id)
{
    return id.outer_tag();
}

template <Class T1, Tag T2>
constexpr Tag reference_tag(const ImplicitIdentifier<T1, T2>& id)
{
    return id.tag();
}

template <UniversalTag T>
constexpr Class reference_class(const ExplicitIdentifier<T>& id)
{
    return id.class_();
}

template <Class T1, Tag T2, typename T3>
constexpr Class reference_class(const TaggedExplicitIdentifier<T1, T2, T3>& id)
{
    return id.outer_class();
}

template <Class T1, Tag T2>
constexpr Class reference_class(const ImplicitIdentifier<T1, T2>& id)
{
    return id.class_();
}

// Used to ensure identifier() functions can be found in the fast_ber namespace using ADL
struct IdentifierAdlToken
{
};

} // namespace fast_ber
