#pragma once

#include "fast_ber/compiler/CompilerTypes.hpp"

void object_class_to_concrete(Asn1Tree& tree, Module& module, Type& type);
void object_class_to_concrete(Asn1Tree& tree, Module& module, ChoiceType& choice);
void object_class_to_concrete(Asn1Tree& tree, Module& module, SequenceType& sequence);
void object_class_to_concrete(Asn1Tree& tree, Module& module, SequenceOfType& sequence);
void object_class_to_concrete(Asn1Tree& tree, Module& module, SetType& set);
void object_class_to_concrete(Asn1Tree& tree, Module& module, SetOfType& set);
void object_class_to_concrete(Asn1Tree& tree, Module& module, PrefixedType& prefixed_type);
void object_class_to_concrete(Asn1Tree& tree, Module& module, BuiltinType& type);
void object_class_to_concrete(Asn1Tree& tree, Module& module, Type& type);

Type create_concrete_type(Asn1Tree& tree, Module& module, ObjectClassFieldType& object_class_field);

void                  remove_object_classes(Asn1Tree& tree, const std::set<std::string>& object_class_names);
std::set<std::string> get_object_class_names(const Asn1Tree& tree);

// Convert usage of object classes to standard ASN.1 types
void resolve_object_classes(Asn1Tree& tree);
