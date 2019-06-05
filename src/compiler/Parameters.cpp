#include "fast_ber/compiler/Parameters.hpp"
#include "fast_ber/compiler/ResolveType.hpp"

void resolve_parameterized(const Asn1Tree& tree, const Module& module, ChoiceType& choice,
                           const ParameterTypes& parameters)
{
    for (NamedType& named_type : choice.choices)
    {
        resolve_parameterized(tree, module, named_type.type, parameters);
    }
}
void resolve_parameterized(const Asn1Tree& tree, const Module& module, SequenceType& sequence,
                           const ParameterTypes& parameters)
{
    for (ComponentType& component : sequence.components)
    {
        resolve_parameterized(tree, module, component.named_type.type, parameters);
    }
}
void resolve_parameterized(const Asn1Tree& tree, const Module& module, SequenceOfType& sequence,
                           const ParameterTypes& parameters)
{
    Type& type = sequence.has_name ? sequence.named_type->type : *sequence.type;
    resolve_parameterized(tree, module, type, parameters);
}
void resolve_parameterized(const Asn1Tree& tree, const Module& module, SetType& set, const ParameterTypes& parameters)
{
    for (ComponentType& component : set.components)
    {
        resolve_parameterized(tree, module, component.named_type.type, parameters);
    }
}
void resolve_parameterized(const Asn1Tree& tree, const Module& module, SetOfType& set, const ParameterTypes& parameters)
{
    Type& type = set.has_name ? set.named_type->type : *set.type;
    resolve_parameterized(tree, module, type, parameters);
}
void resolve_parameterized(const Asn1Tree& tree, const Module& module, PrefixedType& prefixed_type,
                           const ParameterTypes& parameters)
{
    resolve_parameterized(tree, module, prefixed_type.tagged_type->type, parameters);
}

void resolve_parameterized(const Asn1Tree& tree, const Module& module, BuiltinType& type,
                           const ParameterTypes& parameters)
{
    if (absl::holds_alternative<ChoiceType>(type))
    {
        resolve_parameterized(tree, module, absl::get<ChoiceType>(type), parameters);
    }
    else if (absl::holds_alternative<SequenceType>(type))
    {
        resolve_parameterized(tree, module, absl::get<SequenceType>(type), parameters);
    }
    else if (absl::holds_alternative<SequenceOfType>(type))
    {
        resolve_parameterized(tree, module, absl::get<SequenceOfType>(type), parameters);
    }
    else if (absl::holds_alternative<SetType>(type))
    {
        resolve_parameterized(tree, module, absl::get<SetType>(type), parameters);
    }
    else if (absl::holds_alternative<SetOfType>(type))
    {
        resolve_parameterized(tree, module, absl::get<SetOfType>(type), parameters);
    }
    else if (absl::holds_alternative<PrefixedType>(type))
    {
        resolve_parameterized(tree, module, absl::get<PrefixedType>(type), parameters);
    }
}

void resolve_parameterized(const Asn1Tree& tree, const Module& module, Type& param_type,
                           const ParameterTypes& parameters)
{
    if (absl::holds_alternative<DefinedType>(param_type))
    {
        const DefinedType& defined = absl::get<DefinedType>(param_type);
        if (defined.parameters.size() > 0)
        {
            throw std::runtime_error("Nested parameters!");
        }
        else if (!defined.module_reference)
        {
            if (parameters.count(defined.type_reference) > 0)
            {
                param_type = parameters.find(defined.type_reference)->second;
                return;
            }
        }

        // param_type = type(resolve(tree, module.module_reference, defined));
        // resolve_parameterized(tree, module, param_type, parameters);
    }
    else if (absl::holds_alternative<BuiltinType>(param_type))
    {
        resolve_parameterized(tree, module, absl::get<BuiltinType>(param_type), parameters);
    }
}

void parameterized_to_concrete(const Asn1Tree& tree, const Module& module, ChoiceType& choice)
{
    for (NamedType& named_type : choice.choices)
    {
        parameterized_to_concrete(tree, module, named_type.type);
    }
}
void parameterized_to_concrete(const Asn1Tree& tree, const Module& module, SequenceType& sequence)
{
    for (ComponentType& component : sequence.components)
    {
        parameterized_to_concrete(tree, module, component.named_type.type);
    }
}
void parameterized_to_concrete(const Asn1Tree& tree, const Module& module, SequenceOfType& sequence)
{
    Type& type = sequence.has_name ? sequence.named_type->type : *sequence.type;
    parameterized_to_concrete(tree, module, type);
}
void parameterized_to_concrete(const Asn1Tree& tree, const Module& module, SetType& set)
{
    for (ComponentType& component : set.components)
    {
        parameterized_to_concrete(tree, module, component.named_type.type);
    }
}
void parameterized_to_concrete(const Asn1Tree& tree, const Module& module, SetOfType& set)
{
    Type& type = set.has_name ? set.named_type->type : *set.type;
    parameterized_to_concrete(tree, module, type);
}
void parameterized_to_concrete(const Asn1Tree& tree, const Module& module, PrefixedType& prefixed_type)
{
    parameterized_to_concrete(tree, module, prefixed_type.tagged_type->type);
}

void parameterized_to_concrete(const Asn1Tree& tree, const Module& module, BuiltinType& type)
{
    if (absl::holds_alternative<ChoiceType>(type))
    {
        parameterized_to_concrete(tree, module, absl::get<ChoiceType>(type));
    }
    else if (absl::holds_alternative<SequenceType>(type))
    {
        parameterized_to_concrete(tree, module, absl::get<SequenceType>(type));
    }
    else if (absl::holds_alternative<SequenceOfType>(type))
    {
        parameterized_to_concrete(tree, module, absl::get<SequenceOfType>(type));
    }
    else if (absl::holds_alternative<SetType>(type))
    {
        parameterized_to_concrete(tree, module, absl::get<SetType>(type));
    }
    else if (absl::holds_alternative<SetOfType>(type))
    {
        parameterized_to_concrete(tree, module, absl::get<SetOfType>(type));
    }
    else if (absl::holds_alternative<PrefixedType>(type))
    {
        parameterized_to_concrete(tree, module, absl::get<PrefixedType>(type));
    }
}

void parameterized_to_concrete(const Asn1Tree& tree, const Module& module, Type& param_type)
{
    if (absl::holds_alternative<DefinedType>(param_type))
    {
        const DefinedType& defined = absl::get<DefinedType>(param_type);
        if (defined.parameters.size() > 0)
        {
            const Assignment& parameter_template = resolve(tree, module.module_reference, defined);
            if (parameter_template.parameters.size() != defined.parameters.size())
            {
                throw std::runtime_error("Invalid number of parameters for parameterized assignment, expected " +
                                         std::to_string(defined.parameters.size()) + " got " +
                                         std::to_string(defined.parameters.size()));
            }

            std::unordered_map<std::string, Type> new_parameters;
            for (size_t i = 0; i < parameter_template.parameters.size(); i++)
            {
                new_parameters[parameter_template.parameters[i].reference] = defined.parameters[i];
            }

            param_type = type(parameter_template);
            resolve_parameterized(tree, module, param_type, new_parameters);
            return;
        }
    }
    else if (absl::holds_alternative<BuiltinType>(param_type))
    {
        parameterized_to_concrete(tree, module, absl::get<BuiltinType>(param_type));
    }
}

void remove_parameter_templates(Asn1Tree& tree)
{
    for (Module& module : tree.modules)
    {
        module.assignments.erase(
            std::remove_if(module.assignments.begin(), module.assignments.end(),
                           [&](const Assignment& assignment) { return assignment.parameters.size() > 0; }),
            module.assignments.end());
    }
}

// Convert usage of parameterized types into standard ASN.1 types
void resolve_parameters(Asn1Tree& tree)
{
    for (Module& module : tree.modules)
    {
        for (Assignment& assignment : module.assignments)
        {
            if (absl::holds_alternative<TypeAssignment>(assignment.specific))
            {
                if (assignment.parameters.size() == 0)
                {
                    parameterized_to_concrete(tree, module, type(assignment));
                }
            }
        }
    }

    remove_parameter_templates(tree);
}
