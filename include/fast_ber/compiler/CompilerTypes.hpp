#pragma once

#include "absl/memory/memory.h"
#include "absl/types/optional.h"
#include "absl/types/variant.h"

#include <algorithm>
#include <fstream>
#include <list>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

enum class TaggingMode
{
    explicit_,
    implicit,
    automatic
};

enum class Class
{
    universal,
    application,
    context_specific,
    private_,
};

struct ComponentType;
struct TaggedType;
using ComponentTypeList = std::vector<ComponentType>;
using SequenceType      = ComponentTypeList;

struct NamedType;
struct NamedNumber
{
    std::string name;
    long long   number;
};
struct EnumerationValue
{
    std::string               name;
    absl::optional<long long> value;
};

struct BitStringType
{
};
struct BooleanType
{
};
struct CharacterStringType
{
};
struct ChoiceType;
struct DateType
{
};
struct DateTimeType
{
};
struct DurationType
{
};
struct EmbeddedPDVType
{
};
struct EnumeratedType
{
    std::vector<EnumerationValue> enum_values;
    bool                          accept_anything;
};
struct ExternalType
{
};
struct InstanceOfType
{
};
struct IntegerType
{
};
struct IRIType
{
};
struct NullType
{
};
struct ObjectClassFieldType
{
};
struct ObjectIdentifierType
{
};
struct OctetStringType
{
};
struct RealType
{
};
struct RelativeIRIType
{
};
struct RelativeOIDType
{
};
struct SequenceOfType;

struct SetType
{
};
struct SetOfType
{
};
// unique ptr used to avoid circular reference on stack
struct PrefixedType
{
    PrefixedType() = default;
    PrefixedType(const TaggedType& type) : tagged_type(absl::make_unique<TaggedType>(type)) {}
    PrefixedType(const PrefixedType& rhs)
    {
        if (rhs.tagged_type)
        {
            tagged_type = absl::make_unique<TaggedType>(*rhs.tagged_type);
        }
    }
    PrefixedType& operator=(const PrefixedType& rhs)
    {
        if (rhs.tagged_type)
        {
            tagged_type = absl::make_unique<TaggedType>(*rhs.tagged_type);
        }
        return *this;
    }
    std::unique_ptr<TaggedType> tagged_type;
};
struct TimeType
{
};
struct TimeOfDayType
{
};
struct DefinedType
{
    std::string name;
};

using BuiltinType = absl::variant<BitStringType, BooleanType, CharacterStringType, ChoiceType, DateType, DateTimeType,
                                  DurationType, EmbeddedPDVType, EnumeratedType, ExternalType, InstanceOfType,
                                  IntegerType, IRIType, NullType, ObjectClassFieldType, ObjectIdentifierType,
                                  OctetStringType, RealType, RelativeIRIType, RelativeOIDType, SequenceType,
                                  SequenceOfType, SetType, SetOfType, PrefixedType, TimeType, TimeOfDayType>;
using Type        = absl::variant<BuiltinType, DefinedType>;

struct SequenceOfType
{
    // Shared pointers used to prevent circular references
    bool                       has_name;
    std::shared_ptr<NamedType> named_type;
    std::shared_ptr<Type>      type;
};
struct ChoiceType
{
    std::vector<NamedType> choices;
};

struct NamedType
{
    std::string name;
    Type        type;
};

struct Value
{
};

struct ComponentType
{
    NamedType             named_type;
    bool                  is_optional;
    absl::optional<Value> value;
};

struct Tag
{
    Class class_;
    int   tag_number;
};

struct TaggedType
{
    Tag         tag;
    TaggingMode tagging_mode;
    Type        type;
};

struct Assignment
{
    std::string name;
    Type        type;
};

struct Asn1Tree
{
    std::string             module_reference;
    TaggingMode             tagging_default;
    std::vector<Assignment> assignments;
};

std::string to_string(Class class_)
{
    switch (class_)
    {
    case Class::universal:
        return "universal";
    case Class::application:
        return "application";
    case Class::context_specific:
        return "context_specific";
    case Class::private_:
        return "private_";
    default:
        return "";
    }
}
std::string to_string(const BitStringType&);
std::string to_string(const BooleanType&);
std::string to_string(const CharacterStringType&);
std::string to_string(const ChoiceType&);
std::string to_string(const DateType&);
std::string to_string(const DateTimeType&);
std::string to_string(const DurationType&);
std::string to_string(const EmbeddedPDVType&);
std::string to_string(const EnumeratedType&);
std::string to_string(const ExternalType&);
std::string to_string(const InstanceOfType&);
std::string to_string(const IntegerType&);
std::string to_string(const IRIType&);
std::string to_string(const NullType&);
std::string to_string(const ObjectClassFieldType&);
std::string to_string(const ObjectIdentifierType&);
std::string to_string(const OctetStringType&);
std::string to_string(const RealType&);
std::string to_string(const RelativeIRIType&);
std::string to_string(const RelativeOIDType&);
std::string to_string(const SequenceType&);
std::string to_string(const SequenceOfType&);
std::string to_string(const SetType&);
std::string to_string(const SetOfType&);
std::string to_string(const PrefixedType&);
std::string to_string(const TimeType&);
std::string to_string(const TimeOfDayType&);
std::string to_string(const DefinedType&);
std::string to_string(const BuiltinType& type);
std::string to_string(const Type& type);
std::string to_string(const BitStringType&) { return "BitString"; }
std::string to_string(const BooleanType&) { return "Boolean"; }
std::string to_string(const CharacterStringType&) { return "CharacterString"; }
std::string to_string(const ChoiceType& choice)
{
    bool        is_first = true;
    std::string res      = "Choice<";
    for (const auto& named_type : choice.choices)
    {
        if (!is_first)
        {
            res += ", ";
        }
        res += to_string(named_type.type);
        is_first = false;
    }
    res += ">";
    return res;
}
std::string to_string(const DateType&) { return "Date"; }
std::string to_string(const DateTimeType&) { return "DateTime"; }
std::string to_string(const DurationType&) { return "Duration"; }
std::string to_string(const EmbeddedPDVType&) { return "EmbeddedPDV"; }
std::string to_string(const EnumeratedType& enumerated)
{
    std::string res = " {\n";
    for (const EnumerationValue& enum_value : enumerated.enum_values)
    {
        res += "    " + enum_value.name;
        if (enum_value.value)
        {
            res += " = " + std::to_string(*enum_value.value);
        }
        res += ",\n";
    }
    res += "};\n\n";

    return res;
}
std::string to_string(const ExternalType&) { return "External"; }
std::string to_string(const InstanceOfType&) { return "InstanceOf"; }
std::string to_string(const IntegerType&) { return "Integer"; }
std::string to_string(const IRIType&) { return "IRI"; }
std::string to_string(const NullType&) { return "Null"; }
std::string to_string(const ObjectClassFieldType&) { return "ObjectClassField"; }
std::string to_string(const ObjectIdentifierType&) { return "ObjectIdentifier"; }
std::string to_string(const OctetStringType&) { return "OctetString"; }
std::string to_string(const RealType&) { return "Real"; }
std::string to_string(const RelativeIRIType&) { return "RelativeIRI"; }
std::string to_string(const RelativeOIDType&) { return "RelativeOID"; }
std::string make_type_optional(const std::string& type) { return "Optional<" + type + ">"; }
std::string to_string(const SequenceType& sequence)
{
    std::string res = " {\n";

    for (const ComponentType& component : sequence)
    {
        std::string component_type = to_string(component.named_type.type);
        if (component.is_optional)
        {
            component_type = make_type_optional(component_type);
        }
        res += "    " + component_type + " " + component.named_type.name + ";\n";
    }
    res += "};\n\n";

    return res;
}
std::string to_string(const SequenceOfType& sequence)
{
    if (sequence.has_name)
    {
        return "SequenceOf<" + sequence.named_type->name + ">";
    }
    else
    {
        return "SequenceOf<" + to_string(*sequence.type) + ">";
    }
}
std::string to_string(const SetType&) { return "Set"; }
std::string to_string(const SetOfType&) { return "SetOf"; }
std::string to_string(const PrefixedType& prefixed_type) { return to_string(prefixed_type.tagged_type->type); }
std::string to_string(const TimeType&) { return "Time"; }
std::string to_string(const TimeOfDayType&) { return "TimeOfDay"; }

std::string universal_tag(const BitStringType&, TaggingMode);
std::string universal_tag(const BooleanType&, TaggingMode);
std::string universal_tag(const CharacterStringType&, TaggingMode);
std::string universal_tag(const ChoiceType&, TaggingMode);
std::string universal_tag(const DateType&, TaggingMode);
std::string universal_tag(const DateTimeType&, TaggingMode);
std::string universal_tag(const DurationType&, TaggingMode);
std::string universal_tag(const EmbeddedPDVType&, TaggingMode);
std::string universal_tag(const EnumeratedType&, TaggingMode);
std::string universal_tag(const ExternalType&, TaggingMode);
std::string universal_tag(const InstanceOfType&, TaggingMode);
std::string universal_tag(const IntegerType&, TaggingMode);
std::string universal_tag(const IRIType&, TaggingMode);
std::string universal_tag(const NullType&, TaggingMode);
std::string universal_tag(const ObjectClassFieldType&, TaggingMode);
std::string universal_tag(const ObjectIdentifierType&, TaggingMode);
std::string universal_tag(const OctetStringType&, TaggingMode);
std::string universal_tag(const RealType&, TaggingMode);
std::string universal_tag(const RelativeIRIType&, TaggingMode);
std::string universal_tag(const RelativeOIDType&, TaggingMode);
std::string universal_tag(const SequenceType&, TaggingMode);
std::string universal_tag(const SequenceOfType&, TaggingMode);
std::string universal_tag(const SetType&, TaggingMode);
std::string universal_tag(const SetOfType&, TaggingMode);
std::string universal_tag(const PrefixedType&, TaggingMode);
std::string universal_tag(const TaggedType& tagged_type, TaggingMode);
std::string universal_tag(const TimeType&, TaggingMode);
std::string universal_tag(const TimeOfDayType&, TaggingMode);
std::string universal_tag(const DefinedType&, TaggingMode);
std::string universal_tag(const BuiltinType& type, TaggingMode);
std::string universal_tag(const Type& type, TaggingMode);
std::string universal_tag(const BitStringType&, TaggingMode) { return "ExplicitIdentifier<UniversalTag::bit_string>"; }
std::string universal_tag(const BooleanType&, TaggingMode) { return "ExplicitIdentifier<UniversalTag::boolean>"; }
std::string universal_tag(const CharacterStringType&, TaggingMode)
{
    return "ExplicitIdentifier<UniversalTag::bit_string>";
}
std::string universal_tag(const ChoiceType& choice, TaggingMode tagging_mode)
{
    bool        is_first = true;
    std::string res      = "ChoiceId<";
    for (const auto& named_type : choice.choices)
    {
        if (!is_first)
        {
            res += ", ";
        }
        res += universal_tag(named_type.type, tagging_mode);
        is_first = false;
    }
    res += ">";
    return res;
}
std::string universal_tag(const DateType&, TaggingMode) { return "ExplicitIdentifier<UniversalTag::date_type>"; }
std::string universal_tag(const DateTimeType&, TaggingMode) { return "ExplicitIdentifier<UniversalTag::date_time>"; }
std::string universal_tag(const DurationType&, TaggingMode) { return "ExplicitIdentifier<UniversalTag::duration>"; }
std::string universal_tag(const EmbeddedPDVType&, TaggingMode)
{
    return "ExplicitIdentifier<UniversalTag::embedded_pdv>";
}
std::string universal_tag(const EnumeratedType&, TaggingMode) { return "ExplicitIdentifier<UniversalTag::enumerated>"; }
std::string universal_tag(const ExternalType&, TaggingMode) { return "ExplicitIdentifier<UniversalTag::external>"; }
std::string universal_tag(const InstanceOfType&, TaggingMode)
{
    return "ExplicitIdentifier<U{niversalTag::instance_of>";
}
std::string universal_tag(const IntegerType&, TaggingMode) { return "ExplicitIdentifier<UniversalTag::integer>"; }
std::string universal_tag(const IRIType&, TaggingMode) { return "ExplicitIdentifier<UniversalTag::iri>"; }
std::string universal_tag(const NullType&, TaggingMode) { return "ExplicitIdentifier<UniversalTag::null>"; }
std::string universal_tag(const ObjectClassFieldType&, TaggingMode)
{
    return "ExplicitIdentifier<UniversalTag::object_class_field>";
}
std::string universal_tag(const ObjectIdentifierType&, TaggingMode)
{
    return "ExplicitIdentifier<UniversalTag::object_identifier>";
}
std::string universal_tag(const OctetStringType&, TaggingMode)
{
    return "ExplicitIdentifier<UniversalTag::octet_string>";
}
std::string universal_tag(const RealType&, TaggingMode) { return "ExplicitIdentifier<UniversalTag::real>"; }
std::string universal_tag(const RelativeIRIType&, TaggingMode)
{
    return "ExplicitIdentifier<UniversalTag::relative_iri>";
}
std::string universal_tag(const RelativeOIDType&, TaggingMode)
{
    return "ExplicitIdentifier<UniversalTag::relative_oid>";
}
std::string universal_tag(const SequenceType&, TaggingMode) { return "ExplicitIdentifier<UniversalTag::sequence>"; }
std::string universal_tag(const SequenceOfType& sequence, TaggingMode tagging_mode)
{
    return "SequenceId<ExplicitIdentifier<UniversalTag::sequence_of>, " + universal_tag(*sequence.type, tagging_mode) +
           ">";
}
std::string universal_tag(const SetType&, TaggingMode) { return "ExplicitIdentifier<UniversalTag::set>"; }
std::string universal_tag(const SetOfType&, TaggingMode) { return "ExplicitIdentifier<UniversalTag::set_of>"; }
std::string universal_tag(const PrefixedType& prefixed, TaggingMode tagging_mode)
{
    assert(prefixed.tagged_type);
    return universal_tag(*prefixed.tagged_type, tagging_mode);
}
std::string universal_tag(const TaggedType& tagged_type, TaggingMode tagging_mode)
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
        is_explicit = (tagging_mode == TaggingMode::explicit_ || tagging_mode == TaggingMode::automatic);
    }

    if (is_explicit)
    {
        return "TaggedExplicitIdentifier<Class::" + to_string(tagged_type.tag.class_) + ", " +
               std::to_string(tagged_type.tag.tag_number) + ", " + universal_tag(tagged_type.type, tagging_mode) + ">";
    }
    else
    {
        return "ImplicitIdentifier<Class::" + to_string(tagged_type.tag.class_) + ", " +
               std::to_string(tagged_type.tag.tag_number) + ">";
    }
}
std::string universal_tag(const TimeType&, TaggingMode) { return "ExplicitIdentifier<UniversalTag::time>"; }
std::string universal_tag(const TimeOfDayType&, TaggingMode) { return "ExplicitIdentifier<UniversalTag::time_of_day"; }

std::string to_string(const DefinedType& type) { return type.name; }

struct ToStringHelper
{
    template <typename T>
    std::string operator()(const T& t)
    {
        return to_string(t);
    }
};

struct UniversalTagHelper
{
    template <typename T>
    std::string operator()(const T& t)
    {
        return universal_tag(t, tagging_mode);
    }

    TaggingMode tagging_mode;
};

ToStringHelper string_helper;
std::string    to_string(const BuiltinType& type) { return absl::visit(string_helper, type); }
std::string    to_string(const Type& type) { return absl::visit(string_helper, type); }
std::string universal_tag(const DefinedType&, TaggingMode) { return "ExplicitIdentifier<UniversalTag::sequence_of>"; }
std::string universal_tag(const BuiltinType& type, TaggingMode tagging_mode)
{
    UniversalTagHelper tag_helper{tagging_mode};
    return absl::visit(tag_helper, type);
}
std::string universal_tag(const Type& type, TaggingMode tagging_mode)
{
    UniversalTagHelper tag_helper{tagging_mode};
    return absl::visit(tag_helper, type);
}

struct Context;
