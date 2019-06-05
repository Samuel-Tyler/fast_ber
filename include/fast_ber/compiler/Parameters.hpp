#pragma once

#include "fast_ber/compiler/CompilerTypes.hpp"

using ParameterTypes = std::unordered_map<std::string, Type>;

void resolve_parameterized(const Asn1Tree&, const Module&, Type&, const ParameterTypes&);
void resolve_parameterized(const Asn1Tree&, const Module&, ChoiceType&, const ParameterTypes&);
void resolve_parameterized(const Asn1Tree&, const Module&, SequenceType&, const ParameterTypes&);
void resolve_parameterized(const Asn1Tree&, const Module&, SequenceOfType&, const ParameterTypes&);
void resolve_parameterized(const Asn1Tree&, const Module&, SetType&, const ParameterTypes&);
void resolve_parameterized(const Asn1Tree&, const Module&, SetOfType&, const ParameterTypes&);
void resolve_parameterized(const Asn1Tree&, const Module&, PrefixedType&, const ParameterTypes&);
void resolve_parameterized(const Asn1Tree&, const Module&, BuiltinType&, const ParameterTypes&);
void resolve_parameterized(const Asn1Tree&, const Module&, Type&, const ParameterTypes&);

void parameterized_to_concrete(const Asn1Tree&, const Module&, Type&);
void parameterized_to_concrete(const Asn1Tree&, const Module&, ChoiceType&);
void parameterized_to_concrete(const Asn1Tree&, const Module&, SequenceType&);
void parameterized_to_concrete(const Asn1Tree&, const Module&, SequenceOfType&);
void parameterized_to_concrete(const Asn1Tree&, const Module&, SetType&);
void parameterized_to_concrete(const Asn1Tree&, const Module&, SetOfType&);
void parameterized_to_concrete(const Asn1Tree&, const Module&, PrefixedType&);
void parameterized_to_concrete(const Asn1Tree&, const Module&, BuiltinType&);
void parameterized_to_concrete(const Asn1Tree&, const Module&, Type&);

void remove_parameter_templates(Asn1Tree&);

// Convert usage of object classes to standard ASN.1 types
void resolve_parameter(Asn1Tree&, Module&, DefinedType&, const Assignment&);
void resolve_parameters(Asn1Tree&);
