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
    static Class        class_() { return Class::universal; }
    static UniversalTag tag() { return ExplicitTag; }
};

// Inner class is always universal
template <Class OuterClass, Tag OuterTag, typename ExplicitTag>
struct TaggedExplicitIdentifier
{
    static Class       outer_class() { return OuterClass; }
    static Tag         outer_tag() { return OuterTag; }
    static ExplicitTag inner_id() { return ExplicitTag(); }
};

// Any class or tag is valid
template <Class ImplicitClass, Tag ImplicitTag>
struct ImplicitIdentifier
{
    static Class class_() { return ImplicitClass; }
    static Tag   tag() { return ImplicitTag; }
};

template <typename ID1, typename ID2>
struct SequenceId
{
    static ID1 outer_id() { return ID1{}; }
    static ID2 inner_id() { return ID2{}; }
};

template <typename OuterID, typename... IDs>
struct ChoiceId
{
    static OuterID            outer_id() { return OuterID{}; }
    static std::tuple<IDs...> ids() { return std::tuple<IDs...>{}; }
};

template <UniversalTag T>
Tag reference_tag(const ExplicitIdentifier<T>& id)
{
    return val(id.tag());
}

template <Class T1, Tag T2, typename T3>
Tag reference_tag(const TaggedExplicitIdentifier<T1, T2, T3>& id)
{
    return id.outer_tag();
}

template <Class T1, Tag T2>
Tag reference_tag(const ImplicitIdentifier<T1, T2>& id)
{
    return id.tag();
}

template <typename ID1, typename ID2>
Tag reference_tag(const SequenceId<ID1, ID2>& id)
{
    return reference_tag(id.outer_id());
}

template <typename... IDs>
Tag reference_tag(const ChoiceId<IDs...>& id)
{
    return reference_tag(std::get<0>(id.ids()));
}

template <UniversalTag T>
Class reference_class(const ExplicitIdentifier<T>& id)
{
    return id.class_();
}

template <Class T1, Tag T2, typename T3>
Class reference_class(const TaggedExplicitIdentifier<T1, T2, T3>& id)
{
    return id.outer_class();
}

template <Class T1, Tag T2>
Class reference_class(const ImplicitIdentifier<T1, T2>& id)
{
    return id.class_();
}

template <typename ID1, typename ID2>
Class reference_class(const SequenceId<ID1, ID2>& id)
{
    return class_(id.outer_id());
}

template <typename ID, typename... IDs>
Class reference_class(const ChoiceId<ID, IDs...>& id)
{
    return class_(id.outer_id());
}

} // namespace fast_ber
