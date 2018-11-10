#pragma once

//#include "fast_ber/ber_types/All.hpp"

#include "absl/memory/memory.h"
#include "absl/types/optional.h"
#include "absl/types/variant.h"

#include <algorithm>
#include <fstream>
#include <list>
#include <string>
#include <unordered_map>
#include <vector>

struct ComponentType;
using ComponentTypeList = std::vector<ComponentType>;
using SequenceType      = ComponentTypeList;

struct NamedType;

struct BitStringType
{
};
struct BooleanType
{
};
struct CharacterStringType
{
};
struct ChoiceType
{
};
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
struct PrefixedType
{
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

struct Assignment
{
    std::string name;
    Type        type;
};

struct Asn1Tree
{
    std::string             module_reference;
    std::vector<Assignment> assignments;
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
std::string to_string(const ChoiceType&) { return "Choice"; }
std::string to_string(const DateType&) { return "Date"; }
std::string to_string(const DateTimeType&) { return "DateTime"; }
std::string to_string(const DurationType&) { return "Duration"; }
std::string to_string(const EmbeddedPDVType&) { return "EmbeddedPDV"; }
std::string to_string(const EnumeratedType&) { return "Enumerated"; }
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
std::string to_string(const SequenceType&) { return "Sequence"; }
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
std::string to_string(const PrefixedType&) { return "Prefixed"; }
std::string to_string(const TimeType&) { return "Time"; }
std::string to_string(const TimeOfDayType&) { return "TimeOfDay"; }

std::string universal_tag(const BitStringType&);
std::string universal_tag(const BooleanType&);
std::string universal_tag(const CharacterStringType&);
std::string universal_tag(const ChoiceType&);
std::string universal_tag(const DateType&);
std::string universal_tag(const DateTimeType&);
std::string universal_tag(const DurationType&);
std::string universal_tag(const EmbeddedPDVType&);
std::string universal_tag(const EnumeratedType&);
std::string universal_tag(const ExternalType&);
std::string universal_tag(const InstanceOfType&);
std::string universal_tag(const IntegerType&);
std::string universal_tag(const IRIType&);
std::string universal_tag(const NullType&);
std::string universal_tag(const ObjectClassFieldType&);
std::string universal_tag(const ObjectIdentifierType&);
std::string universal_tag(const OctetStringType&);
std::string universal_tag(const RealType&);
std::string universal_tag(const RelativeIRIType&);
std::string universal_tag(const RelativeOIDType&);
std::string universal_tag(const SequenceType&);
std::string universal_tag(const SequenceOfType&);
std::string universal_tag(const SetType&);
std::string universal_tag(const SetOfType&);
std::string universal_tag(const PrefixedType&);
std::string universal_tag(const TimeType&);
std::string universal_tag(const TimeOfDayType&);
std::string universal_tag(const DefinedType&);
std::string universal_tag(const BuiltinType& type);
std::string universal_tag(const Type& type);
std::string universal_tag(const BitStringType&) { return "ExplicitIdentifier{UniversalTag::bit_string"; }
std::string universal_tag(const BooleanType&) { return "ExplicitIdentifier{UniversalTag::boolean}"; }
std::string universal_tag(const CharacterStringType&) { return "ExplicitIdentifier{UniversalTag::bit_string}"; }
std::string universal_tag(const ChoiceType&) { return "ExplicitIdentifier{UniversalTag::choice_type}"; }
std::string universal_tag(const DateType&) { return "ExplicitIdentifier{UniversalTag::date_type}"; }
std::string universal_tag(const DateTimeType&) { return "ExplicitIdentifier{UniversalTag::date_time}"; }
std::string universal_tag(const DurationType&) { return "ExplicitIdentifier{UniversalTag::duration}"; }
std::string universal_tag(const EmbeddedPDVType&) { return "ExplicitIdentifier{UniversalTag::embedded_pdv}"; }
std::string universal_tag(const EnumeratedType&) { return "ExplicitIdentifier{UniversalTag::enumerated}"; }
std::string universal_tag(const ExternalType&) { return "ExplicitIdentifier{UniversalTag::external}"; }
std::string universal_tag(const InstanceOfType&) { return "ExplicitIdentifier{UniversalTag::instance_of}"; }
std::string universal_tag(const IntegerType&) { return "ExplicitIdentifier{UniversalTag::integer}"; }
std::string universal_tag(const IRIType&) { return "ExplicitIdentifier{UniversalTag::iri}"; }
std::string universal_tag(const NullType&) { return "ExplicitIdentifier{UniversalTag::null}"; }
std::string universal_tag(const ObjectClassFieldType&)
{
    return "ExplicitIdentifier{UniversalTag::object_class_field}";
}
std::string universal_tag(const ObjectIdentifierType&) { return "ExplicitIdentifier{UniversalTag::object_identifier}"; }
std::string universal_tag(const OctetStringType&) { return "ExplicitIdentifier{UniversalTag::octet_string}"; }
std::string universal_tag(const RealType&) { return "ExplicitIdentifier{UniversalTag::real}"; }
std::string universal_tag(const RelativeIRIType&) { return "ExplicitIdentifier{UniversalTag::relative_iri}"; }
std::string universal_tag(const RelativeOIDType&) { return "ExplicitIdentifier{UniversalTag::relative_oid}"; }
std::string universal_tag(const SequenceType&) { return "ExplicitIdentifier{UniversalTag::sequence}"; }
std::string universal_tag(const SequenceOfType& sequence)
{
    return "make_sequence_id(ExplicitIdentifier{UniversalTag::sequence_of}, " + universal_tag(*sequence.type) + ")";
}
std::string universal_tag(const SetType&) { return "ExplicitIdentifier{UniversalTag::set}"; }
std::string universal_tag(const SetOfType&) { return "ExplicitIdentifier{UniversalTag::set_of}"; }
std::string universal_tag(const PrefixedType&) { return "ExplicitIdentifier{UniversalTag::prefixed}"; }
std::string universal_tag(const TimeType&) { return "ExplicitIdentifier{UniversalTag::time"; }
std::string universal_tag(const TimeOfDayType&) { return "ExplicitIdentifier{UniversalTag::time_of_day}"; }

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
        return universal_tag(t);
    }
};

ToStringHelper     string_helper;
UniversalTagHelper tag_helper;
std::string        to_string(const Type& type);
std::string        to_string(const BuiltinType& type) { return absl::visit(string_helper, type); }
std::string        to_string(const Type& type) { return absl::visit(string_helper, type); }
std::string        universal_tag(const DefinedType&) { return "ExplicitIdentifier{UniversalTag::sequence_of}"; }
std::string        universal_tag(const BuiltinType& type) { return absl::visit(tag_helper, type); }
std::string        universal_tag(const Type& type) { return absl::visit(tag_helper, type); }

struct Context;
