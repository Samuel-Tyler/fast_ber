#pragma once

#include "fast_ber/compiler/CompilerTypes.hpp"

std::string type_as_string(const AnyType&, const Module&, const Asn1Tree&, const std::string& = {});
std::string type_as_string(const BitStringType&, const Module&, const Asn1Tree&, const std::string& = {});
std::string type_as_string(const BooleanType&, const Module&, const Asn1Tree&, const std::string& = {});
std::string type_as_string(const CharacterStringType&, const Module&, const Asn1Tree&, const std::string& = {});
std::string type_as_string(const ChoiceType&, const Module&, const Asn1Tree&, const std::string& = {});
std::string type_as_string(const DateType&, const Module&, const Asn1Tree&, const std::string& = {});
std::string type_as_string(const DateTimeType&, const Module&, const Asn1Tree&, const std::string& = {});
std::string type_as_string(const DurationType&, const Module&, const Asn1Tree&, const std::string& = {});
std::string type_as_string(const EmbeddedPDVType&, const Module&, const Asn1Tree&, const std::string& = {});
std::string type_as_string(const EnumeratedType&, const Module&, const Asn1Tree&, const std::string& = {});
std::string type_as_string(const ExternalType&, const Module&, const Asn1Tree&, const std::string& = {});
std::string type_as_string(const GeneralizedTimeType&, const Module&, const Asn1Tree&, const std::string& = {});
std::string type_as_string(const InstanceOfType&, const Module&, const Asn1Tree&, const std::string& = {});
std::string type_as_string(const IntegerType&, const Module&, const Asn1Tree&, const std::string& = {});
std::string type_as_string(const IRIType&, const Module&, const Asn1Tree&, const std::string& = {});
std::string type_as_string(const NullType&, const Module&, const Asn1Tree&, const std::string& = {});
std::string type_as_string(const ObjectClassFieldType&, const Module&, const Asn1Tree&, const std::string& = {});
std::string type_as_string(const ObjectDescriptorType&, const Module&, const Asn1Tree&, const std::string& = {});
std::string type_as_string(const ObjectIdentifierType&, const Module&, const Asn1Tree&, const std::string& = {});
std::string type_as_string(const OctetStringType&, const Module&, const Asn1Tree&, const std::string& = {});
std::string type_as_string(const RealType&, const Module&, const Asn1Tree&, const std::string& = {});
std::string type_as_string(const RelativeIRIType&, const Module&, const Asn1Tree&, const std::string& = {});
std::string type_as_string(const RelativeOIDType&, const Module&, const Asn1Tree&, const std::string& = {});
std::string type_as_string(const SequenceType&, const Module&, const Asn1Tree&, const std::string& = {});
std::string type_as_string(const SequenceOfType&, const Module&, const Asn1Tree&, const std::string& = {});
std::string type_as_string(const SetType&, const Module&, const Asn1Tree&, const std::string& = {});
std::string type_as_string(const SetOfType&, const Module&, const Asn1Tree&, const std::string& = {});
std::string type_as_string(const PrefixedType&, const Module&, const Asn1Tree&, const std::string& = {});
std::string type_as_string(const TimeType&, const Module&, const Asn1Tree&, const std::string& = {});
std::string type_as_string(const TimeOfDayType&, const Module&, const Asn1Tree&, const std::string& = {});
std::string type_as_string(const UTCTimeType&, const Module&, const Asn1Tree&, const std::string& = {});
std::string type_as_string(const DefinedType&, const Module&, const Asn1Tree&, const std::string& = {});
std::string type_as_string(const BuiltinType&, const Module&, const Asn1Tree&, const std::string& = {});
std::string type_as_string(const Type&, const Module&, const Asn1Tree&, const std::string& = {});

struct TypeNameHelper
{
    template <typename T>
    std::string operator()(const T& t)
    {
        return type_name(t);
    }
};

inline std::string type_name(const AnyType&) { return "AnyType"; }
inline std::string type_name(const BitStringType&) { return "BitStringType"; }
inline std::string type_name(const BooleanType&) { return "BooleanType"; }
inline std::string type_name(const CharacterStringType&) { return "CharacterStringType"; }
inline std::string type_name(const ChoiceType&) { return "ChoiceType"; }
inline std::string type_name(const DateType&) { return "DateType"; }
inline std::string type_name(const DateTimeType&) { return "DateTimeType"; }
inline std::string type_name(const DurationType&) { return "DurationType"; }
inline std::string type_name(const EmbeddedPDVType&) { return "EmbeddedPDVType"; }
inline std::string type_name(const EnumeratedType&) { return "EnumeratedType"; }
inline std::string type_name(const ExternalType&) { return "ExternalType"; }
inline std::string type_name(const GeneralizedTimeType&) { return "GeneralizedTimeType"; }
inline std::string type_name(const InstanceOfType&) { return "InstanceOfType"; }
inline std::string type_name(const IntegerType&) { return "IntegerType"; }
inline std::string type_name(const IRIType&) { return "IRIType"; }
inline std::string type_name(const NullType&) { return "NullType"; }
inline std::string type_name(const ObjectClassFieldType&) { return "ObjectClassFieldType"; }
inline std::string type_name(const ObjectDescriptorType&) { return "ObjectDescriptorType"; }
inline std::string type_name(const ObjectIdentifierType&) { return "ObjectIdentifierType"; }
inline std::string type_name(const OctetStringType&) { return "OctetStringType"; }
inline std::string type_name(const RealType&) { return "RealType"; }
inline std::string type_name(const RelativeIRIType&) { return "RelativeIRIType"; }
inline std::string type_name(const RelativeOIDType&) { return "RelativeOIDType"; }
inline std::string type_name(const SequenceType&) { return "SequenceType"; }
inline std::string type_name(const SequenceOfType&) { return "SequenceOfType"; }
inline std::string type_name(const SetType&) { return "SetType"; }
inline std::string type_name(const SetOfType&) { return "SetOfType"; }
inline std::string type_name(const PrefixedType&) { return "PrefixedType"; }
inline std::string type_name(const TimeType&) { return "TimeType"; }
inline std::string type_name(const TimeOfDayType&) { return "TimeOfDayType"; }
inline std::string type_name(const UTCTimeType&) { return "UTCTimeType"; }
inline std::string type_name(const DefinedType&) { return "DefinedType"; }
inline std::string type_name(const BuiltinType& t) { return absl::visit(TypeNameHelper{}, (t)); }
inline std::string type_name(const Type& t) { return absl::visit(TypeNameHelper{}, (t)); }
