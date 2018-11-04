#pragma once

#include "fast_ber/ber_types/All.hpp"

#include "absl/memory/memory.h"
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

using BuiltinType = absl::variant<BitStringType, BooleanType, CharacterStringType, ChoiceType, DateType, DateTimeType,
                                  DurationType, EmbeddedPDVType, EnumeratedType, ExternalType, InstanceOfType,
                                  IntegerType, IRIType, NullType, ObjectClassFieldType, ObjectIdentifierType,
                                  OctetStringType, RealType, RelativeIRIType, RelativeOIDType, SequenceType,
                                  SequenceOfType, SetType, SetOfType, PrefixedType, TimeType, TimeOfDayType>;

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
std::string to_string(const SetType&) { return "Set"; }
std::string to_string(const SetOfType&) { return "SetOf"; }
std::string to_string(const PrefixedType&) { return "Prefixed"; }
std::string to_string(const TimeType&) { return "Time"; }
std::string to_string(const TimeOfDayType&) { return "TimeOfDay"; }

struct DefinedType
{
    std::string name;
};

std::string to_string(const DefinedType& type) { return type.name; }

using Type = absl::variant<BuiltinType, DefinedType>;

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

struct to_string_helper
{
    template <typename T>
    std::string operator()(const T& t)
    {
        return to_string(t);
    }
};

to_string_helper string_helper;
std::string      to_string(const Type& type);
std::string      to_string(const BuiltinType& type) { return absl::visit(string_helper, type); }
std::string      to_string(const Type& type) { return absl::visit(string_helper, type); }

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

struct Context;
