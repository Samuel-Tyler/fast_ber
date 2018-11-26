#pragma once

#include "fast_ber/ber_types/Class.hpp"
#include "fast_ber/ber_types/Construction.hpp"
#include "fast_ber/ber_types/Tag.hpp"

#include <tuple>

namespace fast_ber
{

// Class is always universal
struct ExplicitIdentifier
{
    static const Class class_ = Class::universal;
    UniversalTag       tag;
};

// Inner class is always universal
struct TaggedExplicitIdentifier
{
    Class              outer_class;
    Tag                outer_tag;
    ExplicitIdentifier inner_id;
};

// Any class or tag is valid
struct ImplicitIdentifier
{
    Class class_;
    Tag   tag;
};

template <typename ID1, typename ID2>
struct SequenceId
{
    ID1 outer_id;
    ID2 inner_id;
};

template <typename... IDs>
struct ChoiceID
{
    std::tuple<IDs...> ids;
};

template <typename ID1, typename ID2>
SequenceId<ID1, ID2> make_sequence_id(const ID1& outer, const ID2& inner)
{
    return SequenceId<ID1, ID2>{outer, inner};
}

template <typename... IDs>
ChoiceID<IDs...> make_choice_id(const IDs&... ids)
{
    const auto       tuple = std::make_tuple(ids...);
    ChoiceID<IDs...> choice;
    choice.ids = tuple;
    return choice;
}

inline Tag reference_tag(const ExplicitIdentifier& id) { return val(id.tag); }
inline Tag reference_tag(const TaggedExplicitIdentifier& id) { return id.outer_tag; }
inline Tag reference_tag(const ImplicitIdentifier& id) { return id.tag; }
template <typename ID1, typename ID2>
Tag reference_tag(const SequenceId<ID1, ID2>& id)
{
    return reference_tag(id.outer_id);
}
template <typename... IDs>
inline Tag reference_tag(const ChoiceID<IDs...>& id)
{
    return reference_tag(std::get<0>(id.ids));
}

} // namespace fast_ber
