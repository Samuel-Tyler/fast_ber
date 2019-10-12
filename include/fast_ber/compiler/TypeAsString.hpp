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
std::string type_as_string(const GeneralizedTimeType& type, const Module&, const Asn1Tree&, const std::string& = {});
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
