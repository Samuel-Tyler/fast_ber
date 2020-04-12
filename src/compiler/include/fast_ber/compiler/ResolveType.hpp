#pragma once

#include "fast_ber/compiler/CompilerTypes.hpp"

#include <string>

Module&       find_module(Asn1Tree& tree, const std::string& module_reference);
const Module& find_module(const Asn1Tree& tree, const std::string& module_reference);
Module&       find_module(Asn1Tree& tree, Module& module, const DefinedType& defined);
const Module& find_module(const Asn1Tree& tree, const Module& module, const DefinedType& defined);

Assignment&       resolve(Asn1Tree& tree, Module& module, const std::string& reference);
Assignment&       resolve(Asn1Tree& tree, const std::string& module_reference, const std::string& reference);
Assignment&       resolve(Asn1Tree& tree, const std::string& current_module_reference, const DefinedType& defined);
const Assignment& resolve(const Asn1Tree& tree, const Module& module, const std::string& reference);
const Assignment& resolve(const Asn1Tree& tree, const std::string& module_reference, const std::string& reference);
const Assignment& resolve(const Asn1Tree& tree, const std::string& current_module_reference,
                          const DefinedType& defined);

NamedType resolve_type(const Asn1Tree& tree, const std::string& current_module_reference,
                       const DefinedType& original_defined);
NamedType resolve_type(const Asn1Tree& tree, const std::string& current_module_reference, const NamedType& type);

bool exists(const Asn1Tree& tree, const Module& module, const std::string& reference);
bool exists(const Asn1Tree& tree, const std::string& module_reference, const std::string& reference);
bool exists(const Asn1Tree& tree, const std::string& current_module_reference, const DefinedType& defined);

bool is_type(const Assignment& assignment);
bool is_value(const Assignment& assignment);
bool is_object_class(const Assignment& assignment);
bool is_a_parameter(const std::string& reference, const std::vector<Parameter>& parameters);

Type&                        type(Assignment& assignemnt);
const Type&                  type(const Assignment& assignemnt);
ValueAssignment&             value(Assignment& assignemnt);
const ValueAssignment&       value(const Assignment& assignemnt);
ObjectClassAssignment&       object_class(Assignment& assignemnt);
const ObjectClassAssignment& object_class(const Assignment& assignemnt);
