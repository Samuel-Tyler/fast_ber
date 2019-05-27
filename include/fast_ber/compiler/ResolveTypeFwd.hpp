#pragma once

#include "fast_ber/compiler/CompilerTypes.hpp"

#include <string>

Assignment&       resolve(Asn1Tree& tree, Module& module, const std::string& reference);
Assignment&       resolve(Asn1Tree& tree, const std::string& module_reference, const std::string& reference);
Assignment&       resolve(Asn1Tree& tree, const std::string& current_module_reference, const DefinedType& defined);
const Assignment& resolve(const Asn1Tree& tree, const Module& module, const std::string& reference);
const Assignment& resolve(const Asn1Tree& tree, const std::string& module_reference, const std::string& reference);
const Assignment& resolve(const Asn1Tree& tree, const std::string& current_module_reference,
                          const DefinedType& defined);

bool exists(const Asn1Tree& tree, const Module& module, const std::string& reference);
bool exists(const Asn1Tree& tree, const std::string& module_reference, const std::string& reference);
bool exists(const Asn1Tree& tree, const std::string& current_module_reference, const DefinedType& defined);

bool is_type(const Assignment& assignment);
bool is_value(const Assignment& assignment);
bool is_object_class(const Assignment& assignment);

Type&                        type(Assignment& assignemnt);
const Type&                  type(const Assignment& assignemnt);
ValueAssignment&             value(Assignment& assignemnt);
const ValueAssignment&       value(const Assignment& assignemnt);
ObjectClassAssignment&       object_class(Assignment& assignemnt);
const ObjectClassAssignment& object_class(const Assignment& assignemnt);
