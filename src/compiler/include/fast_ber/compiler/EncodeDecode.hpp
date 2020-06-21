#pragma once

#include "fast_ber/compiler/CompilerTypes.hpp"

inline std::string collection_name(const SequenceType&) { return "sequence"; }
inline std::string collection_name(const SetType&) { return "set"; }

std::string create_encode_functions(const Assignment& assignment, const Module& module, const Asn1Tree& tree);
std::string create_decode_functions(const Assignment& assignment, const Module& module, const Asn1Tree& tree);
