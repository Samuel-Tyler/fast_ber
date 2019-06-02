#include "fast_ber/compiler/Identifier.hpp"

TaggingInfo identifier(const AnyType&, const Module&, const Asn1Tree&)
{
    return TaggingInfo{"ExplicitIdentifier<UniversalTag::choice>", true};
}
TaggingInfo identifier(const BitStringType&, const Module&, const Asn1Tree&)
{
    return TaggingInfo{"ExplicitIdentifier<UniversalTag::bit_string>", true};
}
TaggingInfo identifier(const BooleanType&, const Module&, const Asn1Tree&)
{
    return TaggingInfo{"ExplicitIdentifier<UniversalTag::boolean>", true};
}
TaggingInfo identifier(const CharacterStringType&, const Module&, const Asn1Tree&)
{
    return TaggingInfo{"ExplicitIdentifier<UniversalTag::bit_string>", true};
}
TaggingInfo identifier(const ChoiceType&, const Module&, const Asn1Tree&)
{
    return TaggingInfo{"ExplicitIdentifier<UniversalTag::choice>", true};
}
TaggingInfo identifier(const DateType&, const Module&, const Asn1Tree&)
{
    return TaggingInfo{"ExplicitIdentifier<UniversalTag::date_type>", true};
}
TaggingInfo identifier(const DateTimeType&, const Module&, const Asn1Tree&)
{
    return TaggingInfo{"ExplicitIdentifier<UniversalTag::date_time>", true};
}
TaggingInfo identifier(const DurationType&, const Module&, const Asn1Tree&)
{
    return TaggingInfo{"ExplicitIdentifier<UniversalTag::duration>", true};
}
TaggingInfo identifier(const EmbeddedPDVType&, const Module&, const Asn1Tree&)
{
    return TaggingInfo{"ExplicitIdentifier<UniversalTag::embedded_pdv>", true};
}
TaggingInfo identifier(const EnumeratedType&, const Module&, const Asn1Tree&)
{
    return TaggingInfo{"ExplicitIdentifier<UniversalTag::enumerated>", true};
}
TaggingInfo identifier(const ExternalType&, const Module&, const Asn1Tree&)
{
    return TaggingInfo{"ExplicitIdentifier<UniversalTag::external>", true};
}
TaggingInfo identifier(const GeneralizedTimeType&, const Module&, const Asn1Tree&)
{
    return TaggingInfo{"ExplicitIdentifier<UniversalTag::generalized_time>", true};
}
TaggingInfo identifier(const InstanceOfType&, const Module&, const Asn1Tree&)
{
    return TaggingInfo{"ExplicitIdentifier<UniversalTag::instance_of>", true};
}
TaggingInfo identifier(const IntegerType&, const Module&, const Asn1Tree&)
{
    return TaggingInfo{"ExplicitIdentifier<UniversalTag::integer>", true};
}
TaggingInfo identifier(const IRIType&, const Module&, const Asn1Tree&)
{
    return TaggingInfo{"ExplicitIdentifier<UniversalTag::iri>", true};
}
TaggingInfo identifier(const NullType&, const Module&, const Asn1Tree&)
{
    return TaggingInfo{"ExplicitIdentifier<UniversalTag::null>", true};
}
TaggingInfo identifier(const ObjectClassFieldType&, const Module&, const Asn1Tree&)
{
    return TaggingInfo{"ExplicitIdentifier<UniversalTag::object_class_field>", true};
}
TaggingInfo identifier(const ObjectDescriptorType&, const Module&, const Asn1Tree&)
{
    return TaggingInfo{"ExplicitIdentifier<UniversalTag::object_descriptor>", true};
}
TaggingInfo identifier(const ObjectIdentifierType&, const Module&, const Asn1Tree&)
{
    return TaggingInfo{"ExplicitIdentifier<UniversalTag::object_identifier>", true};
}
TaggingInfo identifier(const OctetStringType&, const Module&, const Asn1Tree&)
{
    return TaggingInfo{"ExplicitIdentifier<UniversalTag::octet_string>", true};
}
TaggingInfo identifier(const RealType&, const Module&, const Asn1Tree&)
{
    return TaggingInfo{"ExplicitIdentifier<UniversalTag::real>", true};
}
TaggingInfo identifier(const RelativeIRIType&, const Module&, const Asn1Tree&)
{
    return TaggingInfo{"ExplicitIdentifier<UniversalTag::relative_iri>", true};
}
TaggingInfo identifier(const RelativeOIDType&, const Module&, const Asn1Tree&)
{
    return TaggingInfo{"ExplicitIdentifier<UniversalTag::relative_oid>", true};
}
TaggingInfo identifier(const SequenceType&, const Module&, const Asn1Tree&)
{
    return TaggingInfo{"ExplicitIdentifier<UniversalTag::sequence>", true};
}
TaggingInfo identifier(const SequenceOfType&, const Module&, const Asn1Tree&)
{
    return TaggingInfo{"ExplicitIdentifier<UniversalTag::sequence_of>", true};
}
TaggingInfo identifier(const SetType&, const Module&, const Asn1Tree&)
{
    return TaggingInfo{"ExplicitIdentifier<UniversalTag::set>", true};
}
TaggingInfo identifier(const SetOfType&, const Module&, const Asn1Tree&)
{
    return TaggingInfo{"ExplicitIdentifier<UniversalTag::set_of>", true};
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

    if (is_explicit)
    {
        tag = "TaggedExplicitIdentifier<Class::" + to_string(tagged_type.tag.class_) + ", " +
              std::to_string(tagged_type.tag.tag_number) + ", " +
              identifier(tagged_type.type, current_module, tree).tag + ">";
    }
    else
    {
        tag = "ImplicitIdentifier<Class::" + to_string(tagged_type.tag.class_) + ", " +
              std::to_string(tagged_type.tag.tag_number) + ">";
    }
    return TaggingInfo{tag, false};
}
TaggingInfo identifier(const TimeType&, const Module&, const Asn1Tree&)
{
    return TaggingInfo{"ExplicitIdentifier<UniversalTag::time>", true};
}
TaggingInfo identifier(const TimeOfDayType&, const Module&, const Asn1Tree&)
{
    return TaggingInfo{"ExplicitIdentifier<UniversalTag::time_of_day", true};
}
TaggingInfo identifier(const UTCTimeType&, const Module&, const Asn1Tree&)
{
    return TaggingInfo{"ExplicitIdentifier<UniversalTag::utc_time>", true};
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

std::string fully_tagged_type(const Type& type, const Module& current_module, const Asn1Tree& tree)
{
    const TaggingInfo& tagging_info = identifier(type, current_module, tree);
    if (tagging_info.is_default_tagged)
    {
        return type_as_string(type, current_module, tree);
    }

    return "TaggedType<" + type_as_string(type, current_module, tree) + ", " + tagging_info.tag + ">";
}

TaggingInfo identifier(const DefinedType& defined, const Module& current_module, const Asn1Tree&)
{
    const std::string& module_ref =
        (defined.module_reference) ? *defined.module_reference : current_module.module_reference;
    const std::string& type_string =
        "decltype(identifier(static_cast<" + module_ref + "::" + defined.type_reference + "*>(nullptr)))";

    return {type_string, true};
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
