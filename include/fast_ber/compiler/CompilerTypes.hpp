#pragma once

#include "fast_ber/compiler/CppGeneration.hpp"

#include "absl/memory/memory.h"
#include "absl/types/optional.h"
#include "absl/types/variant.h"

#include <algorithm>
#include <fstream>
#include <list>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

std::string santize_name(const std::string& name)
{
    auto copy = name;
    std::replace(copy.begin(), copy.end(), '-', '_');
    return copy;
}

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

struct NamedType;
struct NamedNumber
{
    std::string name;
    int64_t     number;
};
struct EnumerationValue
{
    std::string             name;
    absl::optional<int64_t> value;
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
struct GeneralizedTimeType
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
struct ObjectDescriptorType
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
struct SequenceType
{
    ComponentTypeList components;
};
struct SequenceOfType;
struct SetType
{
    ComponentTypeList components;
};
struct SetOfType;

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
struct UTCTimeType
{
};
struct DefinedType;

using BuiltinType =
    absl::variant<BitStringType, BooleanType, CharacterStringType, ChoiceType, DateType, DateTimeType, DurationType,
                  EmbeddedPDVType, EnumeratedType, ExternalType, GeneralizedTimeType, InstanceOfType, IntegerType,
                  IRIType, NullType, ObjectClassFieldType, ObjectDescriptorType, ObjectIdentifierType, OctetStringType,
                  RealType, RelativeIRIType, RelativeOIDType, SequenceType, SequenceOfType, SetType, SetOfType,
                  PrefixedType, TimeType, TimeOfDayType, UTCTimeType>;
using Type = absl::variant<BuiltinType, DefinedType>;

struct DefinedType
{
    std::string       name;
    std::vector<Type> parameters;
};
struct SequenceOfType
{
    // Shared pointers used to prevent circular references
    bool                       has_name;
    std::shared_ptr<NamedType> named_type;
    std::shared_ptr<Type>      type;
};
struct SetOfType
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

struct DefinedValue
{
    std::string reference;
};

struct Value
{
    absl::optional<DefinedValue>                                         defined_value;
    absl::variant<std::vector<Value>, std::string, int64_t, NamedNumber> value_selection;
};

struct NamedType
{
    std::string name;
    Type        type;
};

struct NamedValue
{
    std::string name;
    Value       type;
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
    std::string              name;
    Type                     type;
    absl::optional<Value>    value;
    std::vector<std::string> depends_on;
    std::set<std::string>    parameters;
};

struct Import
{
    std::string              module_reference;
    std::vector<std::string> imports;
};

struct Export
{
};

struct Module
{
    std::string             module_reference;
    TaggingMode             tagging_default;
    std::vector<Export>     exports;
    std::vector<Import>     imports;
    std::vector<Assignment> assignments;
};

struct Asn1Tree
{
    std::vector<Module> modules;
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

struct TaggingInfo
{
    std::string tag;
    bool        is_default_tagged;
};

struct ObjectIdComponentValue
{
    absl::optional<std::string> name;
    absl::optional<int64_t>     value;
};

struct ObjectIdComponents
{
    ObjectIdComponents(const Value& value)
    {
        if (!absl::holds_alternative<std::vector<Value>>(value.value_selection))
        {
            throw std::runtime_error("Failed to interpret value as object identifier");
        }
        const std::vector<Value>& value_list = absl::get<std::vector<Value>>(value.value_selection);
        components.reserve(value_list.size());
        for (const Value& component : value_list)
        {
            if (absl::holds_alternative<std::string>(component.value_selection))
            {
                const std::string& name = absl::get<std::string>(component.value_selection);
                components.push_back(ObjectIdComponentValue{name, absl::nullopt});
            }
            else if (absl::holds_alternative<int64_t>(component.value_selection))
            {
                const int64_t& number = absl::get<int64_t>(component.value_selection);
                components.push_back(ObjectIdComponentValue{absl::nullopt, number});
            }
            else if (absl::holds_alternative<NamedNumber>(component.value_selection))
            {
                const NamedNumber& named_number = absl::get<NamedNumber>(component.value_selection);
                components.push_back(ObjectIdComponentValue{named_number.name, named_number.number});
            }
            else
            {
                throw std::runtime_error("Failed to interpret value as object identifier");
            }
        }
    }
    std::vector<ObjectIdComponentValue> components;
};

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
std::string to_string(const GeneralizedTimeType& type);
std::string to_string(const InstanceOfType&);
std::string to_string(const IntegerType&);
std::string to_string(const IRIType&);
std::string to_string(const NullType&);
std::string to_string(const ObjectClassFieldType&);
std::string to_string(const ObjectDescriptorType&);
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
std::string to_string(const UTCTimeType& type);
std::string to_string(const DefinedType&);
std::string to_string(const BuiltinType& type);
std::string to_string(const Type& type);
TaggingInfo universal_tag(const BitStringType&, TaggingMode);
TaggingInfo universal_tag(const BooleanType&, TaggingMode);
TaggingInfo universal_tag(const CharacterStringType&, TaggingMode);
TaggingInfo universal_tag(const ChoiceType&, TaggingMode);
TaggingInfo universal_tag(const DateType&, TaggingMode);
TaggingInfo universal_tag(const DateTimeType&, TaggingMode);
TaggingInfo universal_tag(const DurationType&, TaggingMode);
TaggingInfo universal_tag(const EmbeddedPDVType&, TaggingMode);
TaggingInfo universal_tag(const EnumeratedType&, TaggingMode);
TaggingInfo universal_tag(const ExternalType&, TaggingMode);
TaggingInfo universal_tag(const InstanceOfType&, TaggingMode);
TaggingInfo universal_tag(const IntegerType&, TaggingMode);
TaggingInfo universal_tag(const IRIType&, TaggingMode);
TaggingInfo universal_tag(const NullType&, TaggingMode);
TaggingInfo universal_tag(const ObjectClassFieldType&, TaggingMode);
TaggingInfo universal_tag(const ObjectDescriptorType&, TaggingMode);
TaggingInfo universal_tag(const ObjectIdentifierType&, TaggingMode);
TaggingInfo universal_tag(const OctetStringType&, TaggingMode);
TaggingInfo universal_tag(const RealType&, TaggingMode);
TaggingInfo universal_tag(const RelativeIRIType&, TaggingMode);
TaggingInfo universal_tag(const RelativeOIDType&, TaggingMode);
TaggingInfo universal_tag(const SequenceType&, TaggingMode);
TaggingInfo universal_tag(const SequenceOfType&, TaggingMode);
TaggingInfo universal_tag(const SetType&, TaggingMode);
TaggingInfo universal_tag(const SetOfType&, TaggingMode);
TaggingInfo universal_tag(const PrefixedType&, TaggingMode);
TaggingInfo universal_tag(const TaggedType& tagged_type, TaggingMode);
TaggingInfo universal_tag(const TimeType&, TaggingMode);
TaggingInfo universal_tag(const TimeOfDayType&, TaggingMode);
TaggingInfo universal_tag(const DefinedType&, TaggingMode);
TaggingInfo universal_tag(const BuiltinType& type, TaggingMode);
TaggingInfo universal_tag(const Type& type, TaggingMode);
std::string fully_tagged_type(const Type& type, TaggingMode tagging_mode);

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
        res += fully_tagged_type(named_type.type, TaggingMode::implicit);
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
std::string to_string(const GeneralizedTimeType&) { return "GeneralizedTime"; }
std::string to_string(const InstanceOfType&) { return "InstanceOf"; }
std::string to_string(const IntegerType&) { return "Integer"; }
std::string to_string(const IRIType&) { return "IRI"; }
std::string to_string(const NullType&) { return "Null"; }
std::string to_string(const ObjectClassFieldType&) { return "ObjectClassField"; }
std::string to_string(const ObjectDescriptorType&) { return "ObjectDescriptor"; }
std::string to_string(const ObjectIdentifierType&) { return "ObjectIdentifier"; }
std::string to_string(const OctetStringType&) { return "OctetString"; }
std::string to_string(const RealType&) { return "Real"; }
std::string to_string(const RelativeIRIType&) { return "RelativeIRI"; }
std::string to_string(const RelativeOIDType&) { return "RelativeOID"; }
std::string make_type_optional(const std::string& type) { return "Optional<" + type + ">"; }
std::string to_string(const SequenceType& sequence)
{
    std::string res = " {\n";

    for (const ComponentType& component : sequence.components)
    {
        std::string component_type = to_string(component.named_type.type);
        if (absl::holds_alternative<BuiltinType>(component.named_type.type) &&
            absl::holds_alternative<SequenceType>(absl::get<BuiltinType>(component.named_type.type)))
        {
            res += "struct " + component.named_type.name + "_type " + component_type;
            res += "    " + component.named_type.name + "_type " + component.named_type.name + ";\n";
        }
        else
        {
            if (component.is_optional)
            {
                component_type = make_type_optional(component_type);
            }
            res += "    " + component_type + " " + component.named_type.name + ";\n";
        }
    }
    res += "};\n";

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
std::string to_string(const SetType& set)
{
    std::string res = " {\n";

    for (const ComponentType& component : set.components)
    {
        std::string component_type = to_string(component.named_type.type);
        if (absl::holds_alternative<BuiltinType>(component.named_type.type) &&
            absl::holds_alternative<SequenceType>(absl::get<BuiltinType>(component.named_type.type)))
        {
            res += "    struct " + component.named_type.name + "_type " + component_type + "\n;";
            res += component.named_type.name + "_type " + component.named_type.name + ";\n";
        }
        else
        {
            if (component.is_optional)
            {
                component_type = make_type_optional(component_type);
            }
            res += "    " + component_type + " " + component.named_type.name + ";\n";
        }
    }
    res += "};\n";

    return res;
}
std::string to_string(const SetOfType& set)
{
    if (set.has_name)
    {
        return "SetOf<" + set.named_type->name + ">";
    }
    else
    {
        assert(set.type);
        return "SetOf<" + to_string(*set.type) + ">";
    }
}
std::string to_string(const PrefixedType& prefixed_type) { return to_string(prefixed_type.tagged_type->type); }
std::string to_string(const TimeType&) { return "Time"; }
std::string to_string(const TimeOfDayType&) { return "TimeOfDay"; }
std::string to_string(const UTCTimeType&) { return "UTCTime"; }

TaggingInfo universal_tag(const BitStringType&, TaggingMode)
{
    return TaggingInfo{"ExplicitIdentifier<UniversalTag::bit_string>", true};
}
TaggingInfo universal_tag(const BooleanType&, TaggingMode)
{
    return TaggingInfo{"ExplicitIdentifier<UniversalTag::boolean>", true};
}
TaggingInfo universal_tag(const CharacterStringType&, TaggingMode)
{
    return TaggingInfo{"ExplicitIdentifier<UniversalTag::bit_string>", true};
}
TaggingInfo universal_tag(const ChoiceType&, TaggingMode)
{
    return TaggingInfo{"ExplicitIdentifier<UniversalTag::choice>", true};
}
TaggingInfo universal_tag(const DateType&, TaggingMode)
{
    return TaggingInfo{"ExplicitIdentifier<UniversalTag::date_type>", true};
}
TaggingInfo universal_tag(const DateTimeType&, TaggingMode)
{
    return TaggingInfo{"ExplicitIdentifier<UniversalTag::date_time>", true};
}
TaggingInfo universal_tag(const DurationType&, TaggingMode)
{
    return TaggingInfo{"ExplicitIdentifier<UniversalTag::duration>", true};
}
TaggingInfo universal_tag(const EmbeddedPDVType&, TaggingMode)
{
    return TaggingInfo{"ExplicitIdentifier<UniversalTag::embedded_pdv>", true};
}
TaggingInfo universal_tag(const EnumeratedType&, TaggingMode)
{
    return TaggingInfo{"ExplicitIdentifier<UniversalTag::enumerated>", true};
}
TaggingInfo universal_tag(const ExternalType&, TaggingMode)
{
    return TaggingInfo{"ExplicitIdentifier<UniversalTag::external>", true};
}
TaggingInfo universal_tag(const GeneralizedTimeType&, TaggingMode)
{
    return TaggingInfo{"ExplicitIdentifier<UniversalTag::generalized_time>", true};
}
TaggingInfo universal_tag(const InstanceOfType&, TaggingMode)
{
    return TaggingInfo{"ExplicitIdentifier<UniversalTag::instance_of>", true};
}
TaggingInfo universal_tag(const IntegerType&, TaggingMode)
{
    return TaggingInfo{"ExplicitIdentifier<UniversalTag::integer>", true};
}
TaggingInfo universal_tag(const IRIType&, TaggingMode)
{
    return TaggingInfo{"ExplicitIdentifier<UniversalTag::iri>", true};
}
TaggingInfo universal_tag(const NullType&, TaggingMode)
{
    return TaggingInfo{"ExplicitIdentifier<UniversalTag::null>", true};
}
TaggingInfo universal_tag(const ObjectClassFieldType&, TaggingMode)
{
    return TaggingInfo{"ExplicitIdentifier<UniversalTag::object_class_field>", true};
}
TaggingInfo universal_tag(const ObjectDescriptorType&, TaggingMode)
{
    return TaggingInfo{"ExplicitIdentifier<UniversalTag::object_descriptor>", true};
}
TaggingInfo universal_tag(const ObjectIdentifierType&, TaggingMode)
{
    return TaggingInfo{"ExplicitIdentifier<UniversalTag::object_identifier>", true};
}
TaggingInfo universal_tag(const OctetStringType&, TaggingMode)
{
    return TaggingInfo{"ExplicitIdentifier<UniversalTag::octet_string>", true};
}
TaggingInfo universal_tag(const RealType&, TaggingMode)
{
    return TaggingInfo{"ExplicitIdentifier<UniversalTag::real>", true};
}
TaggingInfo universal_tag(const RelativeIRIType&, TaggingMode)
{
    return TaggingInfo{"ExplicitIdentifier<UniversalTag::relative_iri>", true};
}
TaggingInfo universal_tag(const RelativeOIDType&, TaggingMode)
{
    return TaggingInfo{"ExplicitIdentifier<UniversalTag::relative_oid>", true};
}
TaggingInfo universal_tag(const SequenceType&, TaggingMode)
{
    return TaggingInfo{"ExplicitIdentifier<UniversalTag::sequence>", true};
}
TaggingInfo universal_tag(const SequenceOfType&, TaggingMode)
{
    return TaggingInfo{"ExplicitIdentifier<UniversalTag::sequence_of>", true};
}
TaggingInfo universal_tag(const SetType&, TaggingMode)
{
    return TaggingInfo{"ExplicitIdentifier<UniversalTag::set>", true};
}
TaggingInfo universal_tag(const SetOfType&, TaggingMode)
{
    return TaggingInfo{"ExplicitIdentifier<UniversalTag::set_of>", true};
}
TaggingInfo universal_tag(const PrefixedType& prefixed, TaggingMode tagging_mode)
{
    assert(prefixed.tagged_type);
    return universal_tag(*prefixed.tagged_type, tagging_mode);
}
TaggingInfo universal_tag(const TaggedType& tagged_type, TaggingMode tagging_mode)
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
              std::to_string(tagged_type.tag.tag_number) + ", " + universal_tag(tagged_type.type, tagging_mode).tag +
              ">";
    }
    else
    {
        tag = "ImplicitIdentifier<Class::" + to_string(tagged_type.tag.class_) + ", " +
              std::to_string(tagged_type.tag.tag_number) + ">";
    }
    return TaggingInfo{tag, false};
}
TaggingInfo universal_tag(const TimeType&, TaggingMode)
{
    return TaggingInfo{"ExplicitIdentifier<UniversalTag::time>", true};
}
TaggingInfo universal_tag(const TimeOfDayType&, TaggingMode)
{
    return TaggingInfo{"ExplicitIdentifier<UniversalTag::time_of_day", true};
}
TaggingInfo universal_tag(const UTCTimeType&, TaggingMode)
{
    return TaggingInfo{"ExplicitIdentifier<UniversalTag::utc_time>", true};
}

std::string to_string(const DefinedType& type)
{
    if (!type.parameters.empty())
    {
        std::set<std::string> parameter_types;
        for (const Type& paramter : type.parameters)
        {
            parameter_types.insert(to_string(paramter));
        }
        return type.name + create_template_arguments(parameter_types);
    }
    return type.name;
}

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
    TaggingInfo operator()(const T& t)
    {
        return universal_tag(t, tagging_mode);
    }

    TaggingMode tagging_mode;
};

static ToStringHelper string_helper;

std::string to_string(const BuiltinType& type) { return absl::visit(string_helper, type); }
std::string to_string(const Type& type) { return absl::visit(string_helper, type); }
std::string fully_tagged_type(const Type& type, TaggingMode tagging_mode)
{
    const TaggingInfo& tagging_info = universal_tag(type, tagging_mode);
    if (tagging_info.is_default_tagged)
    {
        return to_string(type);
    }

    return "TaggedType<" + to_string(type) + ", " + tagging_info.tag + ">";
}

TaggingInfo universal_tag(const DefinedType&, TaggingMode)
{
    return TaggingInfo{"ExplicitIdentifier<UniversalTag::sequence_of>", true};
}
TaggingInfo universal_tag(const BuiltinType& type, TaggingMode tagging_mode)
{
    UniversalTagHelper tag_helper{tagging_mode};
    return absl::visit(tag_helper, type);
}
TaggingInfo universal_tag(const Type& type, TaggingMode tagging_mode)
{
    UniversalTagHelper tag_helper{tagging_mode};
    return absl::visit(tag_helper, type);
}

struct Context;
