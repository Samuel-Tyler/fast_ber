#pragma once

#include "fast_ber/compiler/CompilerTypes.hpp"
#include "fast_ber/compiler/ResolveType.hpp"
#include "fast_ber/compiler/TypeAsString.hpp"

TaggingInfo identifier(const AnyType&, const Module&, const Asn1Tree&);
TaggingInfo identifier(const BitStringType&, const Module&, const Asn1Tree&);
TaggingInfo identifier(const BooleanType&, const Module&, const Asn1Tree&);
TaggingInfo identifier(const CharacterStringType&, const Module&, const Asn1Tree&);
TaggingInfo identifier(const ChoiceType&, const Module&, const Asn1Tree&);
TaggingInfo identifier(const DateType&, const Module&, const Asn1Tree&);
TaggingInfo identifier(const DateTimeType&, const Module&, const Asn1Tree&);
TaggingInfo identifier(const DurationType&, const Module&, const Asn1Tree&);
TaggingInfo identifier(const EmbeddedPDVType&, const Module&, const Asn1Tree&);
TaggingInfo identifier(const EnumeratedType&, const Module&, const Asn1Tree&);
TaggingInfo identifier(const ExternalType&, const Module&, const Asn1Tree&);
TaggingInfo identifier(const GeneralizedTimeType&, const Module&, const Asn1Tree&);
TaggingInfo identifier(const InstanceOfType&, const Module&, const Asn1Tree&);
TaggingInfo identifier(const IntegerType&, const Module&, const Asn1Tree&);
TaggingInfo identifier(const IRIType&, const Module&, const Asn1Tree&);
TaggingInfo identifier(const NullType&, const Module&, const Asn1Tree&);
TaggingInfo identifier(const ObjectClassFieldType&, const Module&, const Asn1Tree&);
TaggingInfo identifier(const ObjectDescriptorType&, const Module&, const Asn1Tree&);
TaggingInfo identifier(const ObjectIdentifierType&, const Module&, const Asn1Tree&);
TaggingInfo identifier(const OctetStringType&, const Module&, const Asn1Tree&);
TaggingInfo identifier(const RealType&, const Module&, const Asn1Tree&);
TaggingInfo identifier(const RelativeIRIType&, const Module&, const Asn1Tree&);
TaggingInfo identifier(const RelativeOIDType&, const Module&, const Asn1Tree&);
TaggingInfo identifier(const SequenceType&, const Module&, const Asn1Tree&);
TaggingInfo identifier(const SequenceOfType&, const Module&, const Asn1Tree&);
TaggingInfo identifier(const SetType&, const Module&, const Asn1Tree&);
TaggingInfo identifier(const SetOfType&, const Module&, const Asn1Tree&);
TaggingInfo identifier(const PrefixedType&, const Module&, const Asn1Tree&);
TaggingInfo identifier(const TaggedType& tagged_type, const Module&, const Asn1Tree&);
TaggingInfo identifier(const TimeType&, const Module&, const Asn1Tree&);
TaggingInfo identifier(const TimeOfDayType&, const Module&, const Asn1Tree&);
TaggingInfo identifier(const UTCTimeType&, const Module&, const Asn1Tree&);
TaggingInfo identifier(const DefinedType&, const Module&, const Asn1Tree&);
TaggingInfo identifier(const BuiltinType& type, const Module&, const Asn1Tree&);
TaggingInfo identifier(const Type& type, const Module&, const Asn1Tree&);

std::vector<Identifier> outer_identifiers(const AnyType&, const Module&, const Asn1Tree&);
std::vector<Identifier> outer_identifiers(const BitStringType&, const Module&, const Asn1Tree&);
std::vector<Identifier> outer_identifiers(const BooleanType&, const Module&, const Asn1Tree&);
std::vector<Identifier> outer_identifiers(const CharacterStringType&, const Module&, const Asn1Tree&);
std::vector<Identifier> outer_identifiers(const ChoiceType&, const Module&, const Asn1Tree&);
std::vector<Identifier> outer_identifiers(const DateType&, const Module&, const Asn1Tree&);
std::vector<Identifier> outer_identifiers(const DateTimeType&, const Module&, const Asn1Tree&);
std::vector<Identifier> outer_identifiers(const DurationType&, const Module&, const Asn1Tree&);
std::vector<Identifier> outer_identifiers(const EmbeddedPDVType&, const Module&, const Asn1Tree&);
std::vector<Identifier> outer_identifiers(const EnumeratedType&, const Module&, const Asn1Tree&);
std::vector<Identifier> outer_identifiers(const ExternalType&, const Module&, const Asn1Tree&);
std::vector<Identifier> outer_identifiers(const GeneralizedTimeType&, const Module&, const Asn1Tree&);
std::vector<Identifier> outer_identifiers(const InstanceOfType&, const Module&, const Asn1Tree&);
std::vector<Identifier> outer_identifiers(const IntegerType&, const Module&, const Asn1Tree&);
std::vector<Identifier> outer_identifiers(const IRIType&, const Module&, const Asn1Tree&);
std::vector<Identifier> outer_identifiers(const NullType&, const Module&, const Asn1Tree&);
std::vector<Identifier> outer_identifiers(const ObjectClassFieldType&, const Module&, const Asn1Tree&);
std::vector<Identifier> outer_identifiers(const ObjectDescriptorType&, const Module&, const Asn1Tree&);
std::vector<Identifier> outer_identifiers(const ObjectIdentifierType&, const Module&, const Asn1Tree&);
std::vector<Identifier> outer_identifiers(const OctetStringType&, const Module&, const Asn1Tree&);
std::vector<Identifier> outer_identifiers(const RealType&, const Module&, const Asn1Tree&);
std::vector<Identifier> outer_identifiers(const RelativeIRIType&, const Module&, const Asn1Tree&);
std::vector<Identifier> outer_identifiers(const RelativeOIDType&, const Module&, const Asn1Tree&);
std::vector<Identifier> outer_identifiers(const SequenceType&, const Module&, const Asn1Tree&);
std::vector<Identifier> outer_identifiers(const SequenceOfType&, const Module&, const Asn1Tree&);
std::vector<Identifier> outer_identifiers(const SetType&, const Module&, const Asn1Tree&);
std::vector<Identifier> outer_identifiers(const SetOfType&, const Module&, const Asn1Tree&);
std::vector<Identifier> outer_identifiers(const PrefixedType&, const Module&, const Asn1Tree&);
std::vector<Identifier> outer_identifiers(const TaggedType& tagged_type, const Module&, const Asn1Tree&);
std::vector<Identifier> outer_identifiers(const TimeType&, const Module&, const Asn1Tree&);
std::vector<Identifier> outer_identifiers(const TimeOfDayType&, const Module&, const Asn1Tree&);
std::vector<Identifier> outer_identifiers(const UTCTimeType&, const Module&, const Asn1Tree&);
std::vector<Identifier> outer_identifiers(const DefinedType&, const Module&, const Asn1Tree&);
std::vector<Identifier> outer_identifiers(const BuiltinType& type, const Module&, const Asn1Tree&);
std::vector<Identifier> outer_identifiers(const Type& type, const Module&, const Asn1Tree&);
