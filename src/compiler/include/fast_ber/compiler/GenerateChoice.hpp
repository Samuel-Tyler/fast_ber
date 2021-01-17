#include "fast_ber/compiler/CompilerTypes.hpp"

std::string create_choice_definition(const ChoiceType& choice, const Module& module, const Asn1Tree& tree,
                                     const std::string& name, const std::string& identifier_override);

std::string create_choice_functions(const Asn1Tree& tree, const Module& module, const Assignment& assignment);

CodeBlock create_choice_helpers(const ChoiceType&, const std::string& name);
