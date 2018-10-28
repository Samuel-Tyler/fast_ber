#pragma once

#include "fast_ber/ber_types/All.hpp"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <list>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

struct ComponentType;
using ComponentTypeList = std::vector<ComponentType>;
using SequenceType      = ComponentTypeList;

class BitStringType
{
};
class BooleanType
{
};
class CharacterStringType
{
};
class ChoiceType
{
};
class DateType
{
};
class DateTimeType
{
};
class DurationType
{
};
class EmbeddedPDVType
{
};
class EnumeratedType
{
};
class ExternalType
{
};
class InstanceOfType
{
};
class IntegerType
{
};
class IRIType
{
};
class NullType
{
};
class ObjectClassFieldType
{
};
class ObjectIdentifierType
{
};
class OctetStringType
{
};
class RealType
{
};
class RelativeIRIType
{
};
class RelativeOIDType
{
};
class SequenceOfType
{
};
class SetType
{
};
class SetOfType
{
};
class PrefixedType
{
};
class TimeType
{
};
class TimeOfDayType
{
};

using BuiltinType = std::variant<BitStringType, BooleanType, CharacterStringType, ChoiceType, DateType, DateTimeType,
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
std::string to_string(const SequenceOfType&) { return "SequenceOf"; }
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

using Type = std::variant<BuiltinType, DefinedType>;

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
    NamedType            named_type;
    bool                 is_optional = false;
    std::optional<Value> value;
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

std::string to_string(const Type& type);

std::string to_string(const BuiltinType& type)
{
    return std::visit([](const auto& t) { return to_string(t); }, type);
}

std::string to_string(const Type& type)
{
    return std::visit([](const auto& t) { return to_string(t); }, type);
}

struct Context;
