#pragma once

#include "fast_ber/compiler/CompilerTypes.hpp"

TaggingInfo identifier(const AnyType&, TaggingMode);
TaggingInfo identifier(const BitStringType&, TaggingMode);
TaggingInfo identifier(const BooleanType&, TaggingMode);
TaggingInfo identifier(const CharacterStringType&, TaggingMode);
TaggingInfo identifier(const ChoiceType&, TaggingMode);
TaggingInfo identifier(const DateType&, TaggingMode);
TaggingInfo identifier(const DateTimeType&, TaggingMode);
TaggingInfo identifier(const DurationType&, TaggingMode);
TaggingInfo identifier(const EmbeddedPDVType&, TaggingMode);
TaggingInfo identifier(const EnumeratedType&, TaggingMode);
TaggingInfo identifier(const ExternalType&, TaggingMode);
TaggingInfo identifier(const InstanceOfType&, TaggingMode);
TaggingInfo identifier(const IntegerType&, TaggingMode);
TaggingInfo identifier(const IRIType&, TaggingMode);
TaggingInfo identifier(const NullType&, TaggingMode);
TaggingInfo identifier(const ObjectClassFieldType&, TaggingMode);
TaggingInfo identifier(const ObjectDescriptorType&, TaggingMode);
TaggingInfo identifier(const ObjectIdentifierType&, TaggingMode);
TaggingInfo identifier(const OctetStringType&, TaggingMode);
TaggingInfo identifier(const RealType&, TaggingMode);
TaggingInfo identifier(const RelativeIRIType&, TaggingMode);
TaggingInfo identifier(const RelativeOIDType&, TaggingMode);
TaggingInfo identifier(const SequenceType&, TaggingMode);
TaggingInfo identifier(const SequenceOfType&, TaggingMode);
TaggingInfo identifier(const SetType&, TaggingMode);
TaggingInfo identifier(const SetOfType&, TaggingMode);
TaggingInfo identifier(const PrefixedType&, TaggingMode);
TaggingInfo identifier(const TaggedType& tagged_type, TaggingMode);
TaggingInfo identifier(const TimeType&, TaggingMode);
TaggingInfo identifier(const TimeOfDayType&, TaggingMode);
TaggingInfo identifier(const DefinedType&, TaggingMode);
TaggingInfo identifier(const BuiltinType& type, TaggingMode);
TaggingInfo identifier(const Type& type, TaggingMode);

TaggingInfo identifier(const AnyType&, TaggingMode)
{
    return TaggingInfo{"ExplicitIdentifier<UniversalTag::any>", true};
}
TaggingInfo identifier(const BitStringType&, TaggingMode)
{
    return TaggingInfo{"ExplicitIdentifier<UniversalTag::bit_string>", true};
}
TaggingInfo identifier(const BooleanType&, TaggingMode)
{
    return TaggingInfo{"ExplicitIdentifier<UniversalTag::boolean>", true};
}
TaggingInfo identifier(const CharacterStringType&, TaggingMode)
{
    return TaggingInfo{"ExplicitIdentifier<UniversalTag::bit_string>", true};
}
TaggingInfo identifier(const ChoiceType&, TaggingMode)
{
    return TaggingInfo{"ExplicitIdentifier<UniversalTag::choice>", true};
}
TaggingInfo identifier(const DateType&, TaggingMode)
{
    return TaggingInfo{"ExplicitIdentifier<UniversalTag::date_type>", true};
}
TaggingInfo identifier(const DateTimeType&, TaggingMode)
{
    return TaggingInfo{"ExplicitIdentifier<UniversalTag::date_time>", true};
}
TaggingInfo identifier(const DurationType&, TaggingMode)
{
    return TaggingInfo{"ExplicitIdentifier<UniversalTag::duration>", true};
}
TaggingInfo identifier(const EmbeddedPDVType&, TaggingMode)
{
    return TaggingInfo{"ExplicitIdentifier<UniversalTag::embedded_pdv>", true};
}
TaggingInfo identifier(const EnumeratedType&, TaggingMode)
{
    return TaggingInfo{"ExplicitIdentifier<UniversalTag::enumerated>", true};
}
TaggingInfo identifier(const ExternalType&, TaggingMode)
{
    return TaggingInfo{"ExplicitIdentifier<UniversalTag::external>", true};
}
TaggingInfo identifier(const GeneralizedTimeType&, TaggingMode)
{
    return TaggingInfo{"ExplicitIdentifier<UniversalTag::generalized_time>", true};
}
TaggingInfo identifier(const InstanceOfType&, TaggingMode)
{
    return TaggingInfo{"ExplicitIdentifier<UniversalTag::instance_of>", true};
}
TaggingInfo identifier(const IntegerType&, TaggingMode)
{
    return TaggingInfo{"ExplicitIdentifier<UniversalTag::integer>", true};
}
TaggingInfo identifier(const IRIType&, TaggingMode)
{
    return TaggingInfo{"ExplicitIdentifier<UniversalTag::iri>", true};
}
TaggingInfo identifier(const NullType&, TaggingMode)
{
    return TaggingInfo{"ExplicitIdentifier<UniversalTag::null>", true};
}
TaggingInfo identifier(const ObjectClassFieldType&, TaggingMode)
{
    return TaggingInfo{"ExplicitIdentifier<UniversalTag::object_class_field>", true};
}
TaggingInfo identifier(const ObjectDescriptorType&, TaggingMode)
{
    return TaggingInfo{"ExplicitIdentifier<UniversalTag::object_descriptor>", true};
}
TaggingInfo identifier(const ObjectIdentifierType&, TaggingMode)
{
    return TaggingInfo{"ExplicitIdentifier<UniversalTag::object_identifier>", true};
}
TaggingInfo identifier(const OctetStringType&, TaggingMode)
{
    return TaggingInfo{"ExplicitIdentifier<UniversalTag::octet_string>", true};
}
TaggingInfo identifier(const RealType&, TaggingMode)
{
    return TaggingInfo{"ExplicitIdentifier<UniversalTag::real>", true};
}
TaggingInfo identifier(const RelativeIRIType&, TaggingMode)
{
    return TaggingInfo{"ExplicitIdentifier<UniversalTag::relative_iri>", true};
}
TaggingInfo identifier(const RelativeOIDType&, TaggingMode)
{
    return TaggingInfo{"ExplicitIdentifier<UniversalTag::relative_oid>", true};
}
TaggingInfo identifier(const SequenceType&, TaggingMode)
{
    return TaggingInfo{"ExplicitIdentifier<UniversalTag::sequence>", true};
}
TaggingInfo identifier(const SequenceOfType&, TaggingMode)
{
    return TaggingInfo{"ExplicitIdentifier<UniversalTag::sequence_of>", true};
}
TaggingInfo identifier(const SetType&, TaggingMode)
{
    return TaggingInfo{"ExplicitIdentifier<UniversalTag::set>", true};
}
TaggingInfo identifier(const SetOfType&, TaggingMode)
{
    return TaggingInfo{"ExplicitIdentifier<UniversalTag::set_of>", true};
}
TaggingInfo identifier(const PrefixedType& prefixed, TaggingMode tagging_mode)
{
    assert(prefixed.tagged_type);
    return identifier(*prefixed.tagged_type, tagging_mode);
}
TaggingInfo identifier(const TaggedType& tagged_type, TaggingMode tagging_mode)
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
        is_explicit = (tagging_mode == TaggingMode::explicit_ || tagging_mode == TaggingMode::automatic);
    }

    if (is_explicit)
    {
        tag = "TaggedExplicitIdentifier<Class::" + to_string(tagged_type.tag.class_) + ", " +
              std::to_string(tagged_type.tag.tag_number) + ", " + identifier(tagged_type.type, tagging_mode).tag + ">";
    }
    else
    {
        tag = "ImplicitIdentifier<Class::" + to_string(tagged_type.tag.class_) + ", " +
              std::to_string(tagged_type.tag.tag_number) + ">";
    }
    return TaggingInfo{tag, false};
}
TaggingInfo identifier(const TimeType&, TaggingMode)
{
    return TaggingInfo{"ExplicitIdentifier<UniversalTag::time>", true};
}
TaggingInfo identifier(const TimeOfDayType&, TaggingMode)
{
    return TaggingInfo{"ExplicitIdentifier<UniversalTag::time_of_day", true};
}
TaggingInfo identifier(const UTCTimeType&, TaggingMode)
{
    return TaggingInfo{"ExplicitIdentifier<UniversalTag::utc_time>", true};
}

struct IdentifierHelper
{
    template <typename T>
    TaggingInfo operator()(const T& t) const
    {
        return identifier(t, tagging_mode);
    }

    TaggingMode tagging_mode;
};

std::string fully_tagged_type(const Type& type, TaggingMode tagging_mode)
{
    const TaggingInfo& tagging_info = identifier(type, tagging_mode);
    if (tagging_info.is_default_tagged)
    {
        return to_string(type);
    }

    return "TaggedType<" + to_string(type) + ", " + tagging_info.tag + ">";
}

TaggingInfo identifier(const DefinedType&, TaggingMode) { return TaggingInfo{"Unknown tag!", true}; }
TaggingInfo identifier(const BuiltinType& type, TaggingMode tagging_mode)
{
    IdentifierHelper tag_helper{tagging_mode};
    return absl::visit(tag_helper, type);
}
TaggingInfo identifier(const Type& type, TaggingMode tagging_mode)
{
    IdentifierHelper tag_helper{tagging_mode};
    return absl::visit(tag_helper, type);
}
