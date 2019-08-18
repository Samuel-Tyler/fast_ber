#include "fast_ber/compiler/ResolveType.hpp"

Assignment& resolve(Asn1Tree& tree, Module& module, const std::string& reference)
{
    for (Assignment& assignemnt : module.assignments)
    {
        if (assignemnt.name == reference)
        {
            return assignemnt;
        }
    }

    for (const Import& import : module.imports)
    {
        for (const std::string& imported_reference : import.imports)
        {
            if (imported_reference == reference)
            {
                return resolve(tree, import.module_reference, reference);
            }
        }
    }

    throw std::runtime_error("Reference to undefined object: " + reference);
}

Assignment& resolve(Asn1Tree& tree, const std::string& module_reference, const std::string& reference)
{
    for (Module& module : tree.modules)
    {
        if (module.module_reference == module_reference)
        {
            return resolve(tree, module, reference);
        }
    }
    throw std::runtime_error("Reference to undefined object: " + module_reference + "." + reference);
}

Assignment& resolve(Asn1Tree& tree, const std::string& current_module_reference, const DefinedType& defined)
{
    const std::string& module_reference =
        (defined.module_reference) ? *defined.module_reference : current_module_reference;

    for (Module& module : tree.modules)
    {
        if (module.module_reference == module_reference)
        {
            return resolve(tree, module, defined.type_reference);
        }
    }
    throw std::runtime_error("Reference to undefined object: " + module_reference + "." + defined.type_reference);
}

const Assignment& resolve(const Asn1Tree& tree, const Module& module, const std::string& reference)
{
    for (const Assignment& assignemnt : module.assignments)
    {
        if (assignemnt.name == reference)
        {
            return assignemnt;
        }
    }

    for (const Import& import : module.imports)
    {
        for (const std::string& imported_reference : import.imports)
        {
            if (imported_reference == reference)
            {
                return resolve(tree, import.module_reference, reference);
            }
        }
    }

    throw std::runtime_error("Reference to undefined object: " + reference);
}

const Assignment& resolve(const Asn1Tree& tree, const std::string& module_reference, const std::string& reference)
{
    for (const Module& module : tree.modules)
    {
        if (module.module_reference == module_reference)
        {
            return resolve(tree, module, reference);
        }
    }
    throw std::runtime_error("Reference to undefined object: " + module_reference + "." + reference);
}

const Assignment& resolve(const Asn1Tree& tree, const std::string& current_module_reference, const DefinedType& defined)
{
    const std::string& module_reference =
        (defined.module_reference) ? *defined.module_reference : current_module_reference;

    for (const Module& module : tree.modules)
    {
        if (module.module_reference == module_reference)
        {
            return resolve(tree, module, defined.type_reference);
        }
    }
    throw std::runtime_error("Reference to undefined object: " + module_reference + "." + defined.type_reference);
}

bool exists(const Asn1Tree& tree, const Module& module, const std::string& reference)
{
    for (const Assignment& assignemnt : module.assignments)
    {
        if (assignemnt.name == reference)
        {
            return true;
        }
    }

    for (const Import& import : module.imports)
    {
        for (const std::string& imported_reference : import.imports)
        {
            if (imported_reference == reference)
            {
                return exists(tree, import.module_reference, reference);
            }
        }
    }
    return false;
}

bool exists(const Asn1Tree& tree, const std::string& module_reference, const std::string& reference)
{
    for (const Module& module : tree.modules)
    {
        if (module.module_reference == module_reference)
        {
            return exists(tree, module, reference);
        }
    }
    return false;
}

bool exists(const Asn1Tree& tree, const std::string& current_module_reference, const DefinedType& defined)
{
    const std::string& module_reference =
        (defined.module_reference) ? *defined.module_reference : current_module_reference;

    return exists(tree, module_reference, defined.type_reference);
}

bool is_type(const Assignment& assignment) { return absl::holds_alternative<TypeAssignment>(assignment.specific); }
bool is_value(const Assignment& assignment) { return absl::holds_alternative<ValueAssignment>(assignment.specific); }
bool is_object_class(const Assignment& assignment)
{
    return absl::holds_alternative<ObjectClassAssignment>(assignment.specific) ||
           absl::holds_alternative<ObjectSetAssignment>(assignment.specific);
}

Type&       type(Assignment& assignemnt) { return absl::get<TypeAssignment>(assignemnt.specific).type; }
const Type& type(const Assignment& assignemnt) { return absl::get<TypeAssignment>(assignemnt.specific).type; }

ValueAssignment&       value(Assignment& assignemnt) { return absl::get<ValueAssignment>(assignemnt.specific); }
const ValueAssignment& value(const Assignment& assignemnt) { return absl::get<ValueAssignment>(assignemnt.specific); }

ObjectClassAssignment& object_class(Assignment& assignemnt)
{
    return absl::get<ObjectClassAssignment>(assignemnt.specific);
}
const ObjectClassAssignment& object_class(const Assignment& assignemnt)
{
    return absl::get<ObjectClassAssignment>(assignemnt.specific);
}

bool is_a_parameter(const std::string& reference, const std::vector<Parameter>& parameters)
{
    auto parameter_match = [&](const Parameter& param) { return param.reference == reference; };

    return std::find_if(parameters.begin(), parameters.end(), parameter_match) != parameters.end();
}
