#include "fast_ber/compiler/Identifier.hpp"
#include "fast_ber/compiler/ResolveType.hpp"

TaggingInfo identifier(const AnyType&, const Module&, const Asn1Tree&, IdentifierState*)
{
    TaggingInfo info;
    info.is_default_tagged = true;

    info.choice_ids = {Identifier(UniversalTag::bit_string),
                       Identifier(UniversalTag::boolean),
                       Identifier(UniversalTag::character_string),
                       Identifier(UniversalTag::generalized_time),
                       Identifier(UniversalTag::integer),
                       Identifier(UniversalTag::null),
                       Identifier(UniversalTag::object_identifier),
                       Identifier(UniversalTag::octet_string),
                       Identifier(UniversalTag::real),
                       Identifier(UniversalTag::utc_time),
                       Identifier(UniversalTag::visible_string)};
    return info;
}
TaggingInfo identifier(const BitStringType&, const Module&, const Asn1Tree&, IdentifierState*)
{
    return TaggingInfo{{}, Identifier(UniversalTag::bit_string), {}, true};
}
TaggingInfo identifier(const BooleanType&, const Module&, const Asn1Tree&, IdentifierState*)
{
    return TaggingInfo{{}, Identifier(UniversalTag::boolean), {}, true};
}
TaggingInfo identifier(const CharacterStringType& type, const Module&, const Asn1Tree&, IdentifierState*)
{
    return TaggingInfo{{}, Identifier(universal_tag(type)), {}, true};
}
TaggingInfo identifier(const ChoiceType& choice, const Module& module, const Asn1Tree& tree, IdentifierState* state)
{
    TaggingInfo info;
    info.is_default_tagged = true;

    IdentifierState local_state;

    if (state == nullptr)
    {
        state = &local_state;
    }

    if (state->visited_choices.count(typeid(choice).hash_code()) == 0)
    {
        state->visited_choices.insert(typeid(choice).hash_code());

        for (const NamedType& named_type : choice.choices)
        {
            auto outer_tags = identifier(named_type.type, module, tree, state).outer_tags();
            info.choice_ids.insert(info.choice_ids.end(), outer_tags.begin(), outer_tags.end());
        }
    }

    return info;
}
TaggingInfo identifier(const DateType&, const Module&, const Asn1Tree&, IdentifierState*)
{
    assert("Not implemented");
    return {};
}
TaggingInfo identifier(const DateTimeType&, const Module&, const Asn1Tree&, IdentifierState*)
{
    assert("Not implemented");
    return {};
}
TaggingInfo identifier(const DurationType&, const Module&, const Asn1Tree&, IdentifierState*)
{
    assert("Not implemented");
    return {};
}
TaggingInfo identifier(const EmbeddedPDVType&, const Module&, const Asn1Tree&, IdentifierState*)
{
    return TaggingInfo{{}, Identifier(UniversalTag::embedded_pdv), {}, true};
}
TaggingInfo identifier(const EnumeratedType&, const Module&, const Asn1Tree&, IdentifierState*)
{
    return TaggingInfo{{}, Identifier(UniversalTag::enumerated), {}, true};
}
TaggingInfo identifier(const ExternalType&, const Module&, const Asn1Tree&, IdentifierState*)
{
    return TaggingInfo{{}, Identifier(UniversalTag::external), {}, true};
}
TaggingInfo identifier(const GeneralizedTimeType&, const Module&, const Asn1Tree&, IdentifierState*)
{
    return TaggingInfo{{}, Identifier(UniversalTag::generalized_time), {}, true};
}
TaggingInfo identifier(const InstanceOfType&, const Module&, const Asn1Tree&, IdentifierState*)
{
    return TaggingInfo{{}, Identifier(UniversalTag::instance_of), {}, true};
}
TaggingInfo identifier(const IntegerType&, const Module&, const Asn1Tree&, IdentifierState*)
{
    return TaggingInfo{{}, Identifier(UniversalTag::integer), {}, true};
}
TaggingInfo identifier(const IRIType&, const Module&, const Asn1Tree&, IdentifierState*)
{
    assert("Not implemented");
    return {};
}
TaggingInfo identifier(const NullType&, const Module&, const Asn1Tree&, IdentifierState*)
{
    return TaggingInfo{{}, Identifier(UniversalTag::null), {}, true};
}
TaggingInfo identifier(const ObjectClassFieldType&, const Module&, const Asn1Tree&, IdentifierState*)
{
    assert("Not implemented");
    return {};
}
TaggingInfo identifier(const ObjectDescriptorType&, const Module&, const Asn1Tree&, IdentifierState*)
{
    return TaggingInfo{{}, Identifier(UniversalTag::object_descriptor), {}, true};
}
TaggingInfo identifier(const ObjectIdentifierType&, const Module&, const Asn1Tree&, IdentifierState*)
{
    return TaggingInfo{{}, Identifier(UniversalTag::object_identifier), {}, true};
}
TaggingInfo identifier(const OctetStringType&, const Module&, const Asn1Tree&, IdentifierState*)
{
    return TaggingInfo{{}, Identifier(UniversalTag::octet_string), {}, true};
}
TaggingInfo identifier(const RealType&, const Module&, const Asn1Tree&, IdentifierState*)
{
    return TaggingInfo{{}, Identifier(UniversalTag::real), {}, true};
}
TaggingInfo identifier(const RelativeIRIType&, const Module&, const Asn1Tree&, IdentifierState*)
{
    return TaggingInfo{{}, {}, {}, true};
}
TaggingInfo identifier(const RelativeOIDType&, const Module&, const Asn1Tree&, IdentifierState*)
{
    return TaggingInfo{{}, Identifier(UniversalTag::relative_oid), {}, true};
}
TaggingInfo identifier(const SequenceType&, const Module&, const Asn1Tree&, IdentifierState*)
{
    return TaggingInfo{{}, Identifier(UniversalTag::sequence), {}, true};
}
TaggingInfo identifier(const SequenceOfType&, const Module&, const Asn1Tree&, IdentifierState*)
{
    return TaggingInfo{{}, Identifier(UniversalTag::sequence_of), {}, true};
}
TaggingInfo identifier(const SetType&, const Module&, const Asn1Tree&, IdentifierState*)
{
    return TaggingInfo{{}, Identifier(UniversalTag::set), {}, true};
}
TaggingInfo identifier(const SetOfType&, const Module&, const Asn1Tree&, IdentifierState*)
{
    return TaggingInfo{{}, Identifier(UniversalTag::set_of), {}, true};
}
TaggingInfo identifier(const PrefixedType& prefixed, const Module& current_module, const Asn1Tree& tree,
                       IdentifierState* state)
{
    assert(prefixed.tagged_type);
    return identifier(*prefixed.tagged_type, current_module, tree, state);
}
TaggingInfo identifier(const TaggedType& tagged_type, const Module& current_module, const Asn1Tree& tree,
                       IdentifierState* state)
{
    bool is_explicit = false;
    if (tagged_type.tagging_mode == TaggingMode::explicit_)
    {
        is_explicit = true;
    }
    else if (tagged_type.tagging_mode == TaggingMode::implicit)
    {
        is_explicit = false;
    }
    else
    {
        is_explicit = (current_module.tagging_default == TaggingMode::explicit_ ||
                       current_module.tagging_default == TaggingMode::automatic);
    }
    const TaggingInfo& child_tag = identifier(tagged_type.type, current_module, tree, state);
    if (is_explicit || child_tag.outer_tag)
    {
        return TaggingInfo{
            Identifier(tagged_type.tag.class_, tagged_type.tag.tag_number), child_tag.inner_tag, {}, false};
    }
    else
    {
        return TaggingInfo{{}, Identifier(tagged_type.tag.class_, tagged_type.tag.tag_number), {}, false};
    }
}
TaggingInfo identifier(const TimeType&, const Module&, const Asn1Tree&, IdentifierState*)
{
    assert("Not implemented");
    return {};
}
TaggingInfo identifier(const TimeOfDayType&, const Module&, const Asn1Tree&, IdentifierState*)
{
    assert("Not implemented");
    return {};
}
TaggingInfo identifier(const UTCTimeType&, const Module&, const Asn1Tree&, IdentifierState*)
{
    return TaggingInfo{{}, Identifier(UniversalTag::utc_time), {}, true};
}

struct IdentifierHelper
{
    template <typename T>
    TaggingInfo operator()(const T& t) const
    {
        return identifier(t, current_module, tree, state);
    }
    const Module&    current_module;
    const Asn1Tree&  tree;
    IdentifierState* state;
};

TaggingInfo identifier(const DefinedType& defined, const Module& current_module, const Asn1Tree& tree,
                       IdentifierState* state)
{
    const Type&       resolved_type = type(resolve(tree, current_module.module_reference, defined));
    const Module&     mod           = find_module(tree, current_module, defined);
    const TaggingInfo info          = identifier(resolved_type, mod, tree, state);

    return info;
}
TaggingInfo identifier(const BuiltinType& type, const Module& current_module, const Asn1Tree& tree,
                       IdentifierState* state)
{
    IdentifierHelper tag_helper{current_module, tree, state};
    return absl::visit(tag_helper, type);
}
TaggingInfo identifier(const Type& type, const Module& current_module, const Asn1Tree& tree, IdentifierState* state)
{
    IdentifierHelper tag_helper{current_module, tree, state};
    return absl::visit(tag_helper, type);
}
