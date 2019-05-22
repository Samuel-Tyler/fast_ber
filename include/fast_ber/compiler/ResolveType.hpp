#pragma once

#include "fast_ber/compiler/CompilerTypes.hpp"

Type& resolve_type(std::vector<Assignment>& assignments, const std::string& type_reference)
{
    for (Assignment& assignemnt : assignments)
    {
        if (assignemnt.name == type_reference)
        {
            if (!absl::holds_alternative<TypeAssignment>(assignemnt.specific))
            {
                throw std::runtime_error("Object is not a type: " + type_reference);
            }
            return absl::get<TypeAssignment>(assignemnt.specific).type;
        }
    }

    throw std::runtime_error("Reference to undefined type: " + type_reference);
}

const Type& resolve_type(const std::vector<Assignment>& assignments, const std::string& type_reference)
{
    return resolve_type(const_cast<std::vector<Assignment>&>(assignments), type_reference);
}

Type& resolve_type(Asn1Tree& tree, const std::string& current_module_reference, const DefinedType& defined)
{
    const std::string& module_reference =
        (defined.module_reference) ? *defined.module_reference : current_module_reference;

    for (Module& module : tree.modules)
    {
        if (module.module_reference == module_reference)
        {
            return resolve_type(module.assignments, defined.type_reference);
        }
    }
    throw std::runtime_error("Reference to undefined type: " + module_reference + "." + defined.type_reference);
}

const Type& resolve_type(const Asn1Tree& tree, const std::string& current_module_reference, const DefinedType& defined)
{
    return resolve_type(const_cast<Asn1Tree&>(tree), current_module_reference, defined);
}
