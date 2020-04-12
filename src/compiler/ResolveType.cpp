#include "fast_ber/compiler/ResolveType.hpp"

Module& find_module(Asn1Tree& tree, const std::string& module_reference)
{
    for (Module& module : tree.modules)
    {
        if (module.module_reference == module_reference)
        {
            return module;
        }
    }
    throw std::runtime_error("Module does not exist " + module_reference);
}

const Module& find_module(const Asn1Tree& tree, const std::string& module_reference)
{
    for (const Module& module : tree.modules)
    {
        if (module.module_reference == module_reference)
        {
            return module;
        }
    }
    throw std::runtime_error("Module does not exist " + module_reference);
}

Module& find_module(Asn1Tree& tree, Module& current_module, const DefinedType& defined)
{
    const std::string& module_reference =
        (defined.module_reference) ? *defined.module_reference : current_module.module_reference;

    for (Module& module : tree.modules)
    {
        if (module.module_reference == module_reference)
        {
            for (const Assignment& assignment : module.assignments)
            {
                if (assignment.name == defined.type_reference)
                {
                    return module;
                }
            }
        }
    }

    throw std::runtime_error("Reference to undefined object: " + current_module.module_reference + "." +
                             defined.type_reference);
}

const Module& find_module(const Asn1Tree& tree, const Module& current_module, const DefinedType& defined)
{
    const std::string& module_reference =
        (defined.module_reference) ? *defined.module_reference : current_module.module_reference;

    for (const Module& module : tree.modules)
    {
        if (module.module_reference == module_reference)
        {
            for (const Assignment& assignment : module.assignments)
            {
                if (assignment.name == defined.type_reference)
                {
                    return module;
                }
            }
        }
    }

    throw std::runtime_error("Reference to undefined object: " + current_module.module_reference + "." +
                             defined.type_reference);
}

Assignment& resolve(Asn1Tree&, Module& module, const std::string& reference)
{
    for (Assignment& assignemnt : module.assignments)
    {
        if (assignemnt.name == reference)
        {
            return assignemnt;
        }
    }

    throw std::runtime_error("Reference to undefined object: " + module.module_reference + "." + reference);
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

const Assignment& resolve(const Asn1Tree&, const Module& module, const std::string& reference)
{
    for (const Assignment& assignemnt : module.assignments)
    {
        if (assignemnt.name == reference)
        {
            return assignemnt;
        }
    }

    throw std::runtime_error("Reference to undefined object: " + module.module_reference + "." + reference);
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

NamedType resolve_type(const Asn1Tree& tree, const std::string& current_module_reference,
                       const DefinedType& original_defined)
{
    DefinedType defined = original_defined;
    std::string module  = current_module_reference;
    while (true)
    {
        assert(std::isupper(defined.type_reference[0]));
        const Assignment& assignment = resolve(tree, module, defined);
        if (!is_type(assignment))
        {
            throw std::runtime_error("Defined must be a type in this context! " + original_defined.type_reference);
        }
        if (is_defined(type(assignment)))
        {
            if (defined.module_reference)
            {
                module = *defined.module_reference;
            }
            defined = absl::get<DefinedType>(type(assignment));
        }
        else
        {
            return NamedType{assignment.name, type(assignment)};
        }
    }
}

NamedType resolve_type(const Asn1Tree& tree, const std::string& current_module_reference, const NamedType& type_info)
{
    if (is_defined(type_info.type))
    {
        return resolve_type(tree, current_module_reference, absl::get<DefinedType>(type_info.type));
    }
    return type_info;
}

bool exists(const Asn1Tree&, const Module& module, const std::string& reference)
{
    for (const Assignment& assignemnt : module.assignments)
    {
        if (assignemnt.name == reference)
        {
            return true;
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
