#include "fast_ber/compiler/Identifier.hpp"
#include "fast_ber/compiler/ResolveType.hpp"

TaggingInfo identifier(const AnyType&, const Module&, const Asn1Tree&)
{
    std::string id = "fast_ber::ChoiceId<"
                     "fast_ber::Id<fast_ber::Class::universal, 3>, fast_ber::Id<fast_ber::Class::universal, 1>,"
                     "fast_ber::Id<fast_ber::Class::universal, 29>, fast_ber::Id<fast_ber::Class::universal, 24>,"
                     "fast_ber::Id<fast_ber::Class::universal, 24>, fast_ber::Id<fast_ber::Class::universal, 24>,"
                     "fast_ber::Id<fast_ber::Class::universal, 24>, fast_ber::Id<fast_ber::Class::universal, 2>,"
                     "fast_ber::Id<fast_ber::Class::universal, 5>, fast_ber::Id<fast_ber::Class::universal, 6>,"
                     "fast_ber::Id<fast_ber::Class::universal, 4>, fast_ber::Id<fast_ber::Class::universal, 9>,"
                     "fast_ber::Id<fast_ber::Class::universal, 23>, fast_ber::Id<fast_ber::Class::universal, 23>,"
                     "fast_ber::Id<fast_ber::Class::universal, 23>, fast_ber::Id<fast_ber::Class::universal, 26>>";
    return TaggingInfo{{}, id, true};
}
TaggingInfo identifier(const BitStringType&, const Module&, const Asn1Tree&)
{
    return TaggingInfo{{}, "ExplicitId<UniversalTag::bit_string>", true};
}
TaggingInfo identifier(const BooleanType&, const Module&, const Asn1Tree&)
{
    return TaggingInfo{{}, "ExplicitId<UniversalTag::boolean>", true};
}
TaggingInfo identifier(const CharacterStringType& type, const Module&, const Asn1Tree&)
{
    return TaggingInfo{{}, "ExplicitId<UniversalTag::" + to_string_snake_case(type) + ">", true};
}
TaggingInfo identifier(const ChoiceType& choice, const Module& module, const Asn1Tree& tree)
{
    std::string id    = "ChoiceId<";
    bool        first = true;

    for (const NamedType& named_type : choice.choices)
    {
        if (!first)
        {
            id += ", ";
        }

        id += identifier(named_type.type, module, tree).name();
        first = false;
    }

    id += ">";
    return TaggingInfo{{}, id, true};
}
TaggingInfo identifier(const DateType&, const Module&, const Asn1Tree&)
{
    return TaggingInfo{{}, "ExplicitId<UniversalTag::date_type>", true};
}
TaggingInfo identifier(const DateTimeType&, const Module&, const Asn1Tree&)
{
    return TaggingInfo{{}, "ExplicitId<UniversalTag::date_time>", true};
}
TaggingInfo identifier(const DurationType&, const Module&, const Asn1Tree&)
{
    return TaggingInfo{{}, "ExplicitId<UniversalTag::duration>", true};
}
TaggingInfo identifier(const EmbeddedPDVType&, const Module&, const Asn1Tree&)
{
    return TaggingInfo{{}, "ExplicitId<UniversalTag::embedded_pdv>", true};
}
TaggingInfo identifier(const EnumeratedType&, const Module&, const Asn1Tree&)
{
    return TaggingInfo{{}, "ExplicitId<UniversalTag::enumerated>", true};
}
TaggingInfo identifier(const ExternalType&, const Module&, const Asn1Tree&)
{
    return TaggingInfo{{}, "ExplicitId<UniversalTag::external>", true};
}
TaggingInfo identifier(const GeneralizedTimeType&, const Module&, const Asn1Tree&)
{
    return TaggingInfo{{}, "ExplicitId<UniversalTag::generalized_time>", true};
}
TaggingInfo identifier(const InstanceOfType&, const Module&, const Asn1Tree&)
{
    return TaggingInfo{{}, "ExplicitId<UniversalTag::instance_of>", true};
}
TaggingInfo identifier(const IntegerType&, const Module&, const Asn1Tree&)
{
    return TaggingInfo{{}, "ExplicitId<UniversalTag::integer>", true};
}
TaggingInfo identifier(const IRIType&, const Module&, const Asn1Tree&)
{
    return TaggingInfo{{}, "ExplicitId<UniversalTag::iri>", true};
}
TaggingInfo identifier(const NullType&, const Module&, const Asn1Tree&)
{
    return TaggingInfo{{}, "ExplicitId<UniversalTag::null>", true};
}
TaggingInfo identifier(const ObjectClassFieldType&, const Module&, const Asn1Tree&)
{
    return TaggingInfo{{}, "ExplicitId<UniversalTag::object_class_field>", true};
}
TaggingInfo identifier(const ObjectDescriptorType&, const Module&, const Asn1Tree&)
{
    return TaggingInfo{{}, "ExplicitId<UniversalTag::object_descriptor>", true};
}
TaggingInfo identifier(const ObjectIdentifierType&, const Module&, const Asn1Tree&)
{
    return TaggingInfo{{}, "ExplicitId<UniversalTag::object_identifier>", true};
}
TaggingInfo identifier(const OctetStringType&, const Module&, const Asn1Tree&)
{
    return TaggingInfo{{}, "ExplicitId<UniversalTag::octet_string>", true};
}
TaggingInfo identifier(const RealType&, const Module&, const Asn1Tree&)
{
    return TaggingInfo{{}, "ExplicitId<UniversalTag::real>", true};
}
TaggingInfo identifier(const RelativeIRIType&, const Module&, const Asn1Tree&)
{
    return TaggingInfo{{}, "ExplicitId<UniversalTag::relative_iri>", true};
}
TaggingInfo identifier(const RelativeOIDType&, const Module&, const Asn1Tree&)
{
    return TaggingInfo{{}, "ExplicitId<UniversalTag::relative_oid>", true};
}
TaggingInfo identifier(const SequenceType&, const Module&, const Asn1Tree&)
{
    return TaggingInfo{{}, "ExplicitId<UniversalTag::sequence>", true};
}
TaggingInfo identifier(const SequenceOfType&, const Module&, const Asn1Tree&)
{
    return TaggingInfo{{}, "ExplicitId<UniversalTag::sequence_of>", true};
}
TaggingInfo identifier(const SetType&, const Module&, const Asn1Tree&)
{
    return TaggingInfo{{}, "ExplicitId<UniversalTag::set>", true};
}
TaggingInfo identifier(const SetOfType&, const Module&, const Asn1Tree&)
{
    return TaggingInfo{{}, "ExplicitId<UniversalTag::set_of>", true};
}
TaggingInfo identifier(const PrefixedType& prefixed, const Module& current_module, const Asn1Tree& tree)
{
    assert(prefixed.tagged_type);
    return identifier(*prefixed.tagged_type, current_module, tree);
}
TaggingInfo identifier(const TaggedType& tagged_type, const Module& current_module, const Asn1Tree& tree)
{
    std::string tag         = "";
    bool        is_explicit = false;
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
    const TaggingInfo& child_tag = identifier(tagged_type.type, current_module, tree);
    if (is_explicit || child_tag.outer_tag)
    {
        return TaggingInfo{Identifier{tagged_type.tag.class_, tagged_type.tag.tag_number}, child_tag.inner_tag, false};
    }
    else
    {
        return TaggingInfo{{}, Identifier{tagged_type.tag.class_, tagged_type.tag.tag_number}.name(), false};
    }
}
TaggingInfo identifier(const TimeType&, const Module&, const Asn1Tree&)
{
    return TaggingInfo{{}, "ExplicitId<UniversalTag::time>", true};
}
TaggingInfo identifier(const TimeOfDayType&, const Module&, const Asn1Tree&)
{
    return TaggingInfo{{}, "ExplicitId<UniversalTag::time_of_day", true};
}
TaggingInfo identifier(const UTCTimeType&, const Module&, const Asn1Tree&)
{
    return TaggingInfo{{}, "ExplicitId<UniversalTag::utc_time>", true};
}

struct IdentifierHelper
{
    template <typename T>
    TaggingInfo operator()(const T& t) const
    {
        return identifier(t, current_module, tree);
    }
    const Module&   current_module;
    const Asn1Tree& tree;
};

TaggingInfo identifier(const DefinedType& defined, const Module& current_module, const Asn1Tree& tree)
{
    const Type&       resolved_type = type(resolve(tree, current_module.module_reference, defined));
    const Module&     mod           = find_module(tree, current_module, defined);
    const TaggingInfo info          = identifier(resolved_type, mod, tree);

    return info;
}
TaggingInfo identifier(const BuiltinType& type, const Module& current_module, const Asn1Tree& tree)
{
    IdentifierHelper tag_helper{current_module, tree};
    return absl::visit(tag_helper, type);
}
TaggingInfo identifier(const Type& type, const Module& current_module, const Asn1Tree& tree)
{
    IdentifierHelper tag_helper{current_module, tree};
    return absl::visit(tag_helper, type);
}
