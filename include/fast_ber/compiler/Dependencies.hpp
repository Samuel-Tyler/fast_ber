#pragma once

#include "fast_ber/compiler/CompilerTypes.hpp"

std::vector<std::string> depends_on(const AnyType&);
std::vector<std::string> depends_on(const BitStringType&);
std::vector<std::string> depends_on(const BooleanType&);
std::vector<std::string> depends_on(const CharacterStringType&);
std::vector<std::string> depends_on(const ChoiceType);
std::vector<std::string> depends_on(const DateType&);
std::vector<std::string> depends_on(const DateTimeType&);
std::vector<std::string> depends_on(const DurationType&);
std::vector<std::string> depends_on(const EmbeddedPDVType&);
std::vector<std::string> depends_on(const EnumeratedType&);
std::vector<std::string> depends_on(const ExternalType&);
std::vector<std::string> depends_on(const GeneralizedTimeType&);
std::vector<std::string> depends_on(const InstanceOfType&);
std::vector<std::string> depends_on(const IntegerType&);
std::vector<std::string> depends_on(const IRIType&);
std::vector<std::string> depends_on(const NullType&);
std::vector<std::string> depends_on(const ObjectClassFieldType&);
std::vector<std::string> depends_on(const ObjectDescriptorType&);
std::vector<std::string> depends_on(const ObjectIdentifierType&);
std::vector<std::string> depends_on(const OctetStringType&);
std::vector<std::string> depends_on(const RealType&);
std::vector<std::string> depends_on(const RelativeIRIType&);
std::vector<std::string> depends_on(const RelativeOIDType&);
std::vector<std::string> depends_on(const SequenceType& sequence);
std::vector<std::string> depends_on(const SequenceOfType& sequence);
std::vector<std::string> depends_on(const SetType& set);
std::vector<std::string> depends_on(const SetOfType& set);
std::vector<std::string> depends_on(const PrefixedType&);
std::vector<std::string> depends_on(const TimeType&);
std::vector<std::string> depends_on(const TimeOfDayType&);
std::vector<std::string> depends_on(const UTCTimeType&);
std::vector<std::string> depends_on(const DefinedType&);
std::vector<std::string> depends_on(const BuiltinType& type);
std::vector<std::string> depends_on(const Type& type);

std::vector<std::string> dependencies(const Type& type);
std::vector<std::string> dependencies(const Assignment& assignment);
