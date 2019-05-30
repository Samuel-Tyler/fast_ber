#pragma once

#include "fast_ber/compiler/CompilerTypes.hpp"
#include "fast_ber/compiler/ResolveTypeFwd.hpp"

void object_class_to_concrete(Asn1Tree& tree, Module& module, Type& type);

void object_class_to_concrete(Asn1Tree& tree, Module& module, ChoiceType& choice)
{
    for (NamedType& named_type : choice.choices)
    {
        object_class_to_concrete(tree, module, named_type.type);
    }
}
void object_class_to_concrete(Asn1Tree& tree, Module& module, SequenceType& sequence)
{
    for (ComponentType& component : sequence.components)
    {
        object_class_to_concrete(tree, module, component.named_type.type);
    }
}
void object_class_to_concrete(Asn1Tree& tree, Module& module, SequenceOfType& sequence)
{
    Type& type = sequence.has_name ? sequence.named_type->type : *sequence.type;
    object_class_to_concrete(tree, module, type);
}
void object_class_to_concrete(Asn1Tree& tree, Module& module, SetType& set)
{
    for (ComponentType& component : set.components)
    {
        object_class_to_concrete(tree, module, component.named_type.type);
    }
}
void object_class_to_concrete(Asn1Tree& tree, Module& module, SetOfType& set)
{
    Type& type = set.has_name ? set.named_type->type : *set.type;
    object_class_to_concrete(tree, module, type);
}
void object_class_to_concrete(Asn1Tree& tree, Module& module, PrefixedType& prefixed_type)
{
    object_class_to_concrete(tree, module, prefixed_type.tagged_type->type);
}

void object_class_to_concrete(Asn1Tree& tree, Module& module, BuiltinType& type)
{
    if (absl::holds_alternative<ChoiceType>(type))
    {
        object_class_to_concrete(tree, module, absl::get<ChoiceType>(type));
    }
    else if (absl::holds_alternative<SequenceType>(type))
    {
        object_class_to_concrete(tree, module, absl::get<SequenceType>(type));
    }
    else if (absl::holds_alternative<SequenceOfType>(type))
    {
        object_class_to_concrete(tree, module, absl::get<SequenceOfType>(type));
    }
    else if (absl::holds_alternative<SetType>(type))
    {
        object_class_to_concrete(tree, module, absl::get<SetType>(type));
    }
    else if (absl::holds_alternative<SetOfType>(type))
    {
        object_class_to_concrete(tree, module, absl::get<SetOfType>(type));
    }
    else if (absl::holds_alternative<PrefixedType>(type))
    {
        object_class_to_concrete(tree, module, absl::get<PrefixedType>(type));
    }
}

Type create_concrete_type(Asn1Tree& tree, Module& module, ObjectClassFieldType& object_class_field)
{
    const Assignment& assigment = resolve(tree, module.module_reference, object_class_field.referenced_object_class);
    if (!is_object_class(assigment))
    {
        throw std::runtime_error("Referenced object is not an ObjectClass " +
                                 object_class_field.referenced_object_class.type_reference);
    }

    if (object_class_field.fieldnames.size() == 1)
    {
        for (const ClassField& field : object_class(assigment).fields)
        {
            if (field.name == object_class_field.fieldnames[0])
            {
                if (absl::holds_alternative<FixedTypeValueField>(field.field))
                {
                    return absl::get<FixedTypeValueField>(field.field).type;
                }
                throw std::runtime_error("Referenced class field does not have a type: " +
                                         object_class_field.referenced_object_class.type_reference + "." +
                                         object_class_field.fieldnames[0]);
            }
        }
    }

    throw std::runtime_error("Failed to parse object field reference " +
                             object_class_field.referenced_object_class.type_reference);
}

void object_class_to_concrete(Asn1Tree& tree, Module& module, Type& type)
{
    if (absl::holds_alternative<BuiltinType>(type))
    {
        if (absl::holds_alternative<ObjectClassFieldType>(absl::get<BuiltinType>(type)))
        {
            type = create_concrete_type(tree, module, absl::get<ObjectClassFieldType>(absl::get<BuiltinType>(type)));
        }
        else
        {
            object_class_to_concrete(tree, module, absl::get<BuiltinType>(type));
        }
    }
    else if (absl::holds_alternative<DefinedType>(type))
    {
        // Do nothing
    }
    else
    {
        throw std::runtime_error("Unhandled type!");
    }
}

bool is_defined_object_class(const std::string& module_reference, const std::string& type_reference,
                             const std::set<std::string>& object_class_names)
{
    return object_class_names.count(module_reference + "." + type_reference) > 0;
}

bool is_defined_object_class(const Asn1Tree&, Module& module, const Type& type,
                             const std::set<std::string>& object_class_names)
{
    if (is_defined(type))
    {
        const DefinedType& defined = absl::get<DefinedType>(type);
        if (defined.module_reference)
        {
            return is_defined_object_class(*defined.module_reference, defined.type_reference, object_class_names);
        }
        return is_defined_object_class(module.module_reference, defined.type_reference, object_class_names);
    }
    return false;
}

void remove_object_classes(Asn1Tree& tree, const std::set<std::string>& object_class_names)
{
    for (Module& module : tree.modules)
    {
        module.assignments.erase(
            std::remove_if(
                module.assignments.begin(), module.assignments.end(),
                [&](const Assignment& assignment) {
                    if (is_object_class(assignment))
                    {
                        return true;
                    }
                    else if (is_type(assignment) && is_defined(type(assignment)))
                    {
                        return is_defined_object_class(tree, module, type(assignment), object_class_names);
                    }
                    else if (absl::holds_alternative<ValueAssignment>(assignment.specific) &&
                             absl::holds_alternative<DefinedType>(absl::get<ValueAssignment>(assignment.specific).type))
                    {
                        const ValueAssignment& value_assign = absl::get<ValueAssignment>(assignment.specific);
                        return is_defined_object_class(tree, module, value_assign.type, object_class_names);
                    }

                    for (const Parameter& parameter : assignment.parameters)
                    {
                        if (parameter.governor)
                        {
                            if (is_defined_object_class(tree, module, *parameter.governor, object_class_names))
                            {
                                return true;
                            }
                        }
                    }

                    return false;
                }),
            module.assignments.end());
    }

    for (Module& module : tree.modules)
    {
        for (Import& import : module.imports)
        {
            import.imports.erase(std::remove_if(import.imports.begin(), import.imports.end(),
                                                [&](const std::string& imported_name) {
                                                    return is_defined_object_class(import.module_reference,
                                                                                   imported_name, object_class_names);
                                                }),
                                 import.imports.end());
        }
    }
}

std::set<std::string> get_object_class_names(const Asn1Tree& tree)
{
    std::set<std::string> object_class_names;

    size_t old_number_of_names = 0;
    do
    {
        old_number_of_names = object_class_names.size();
        for (const Module& module : tree.modules)
        {
            for (const Assignment& assignment : module.assignments)
            {
                if (is_type(assignment) || is_value(assignment))
                {
                    for (const Parameter& parameter : assignment.parameters)
                    {
                        if (parameter.governor)
                        {
                            if (is_defined(*parameter.governor))
                            {
                                const DefinedType& defined          = absl::get<DefinedType>(*parameter.governor);
                                const Assignment&  inner_assignment = resolve(tree, module.module_reference, defined);
                                if (is_object_class(inner_assignment))
                                {
                                    object_class_names.insert(module.module_reference + "." + assignment.name);
                                }
                                else if (is_defined_object_class(*defined.module_reference, defined.type_reference,
                                                                 object_class_names))
                                {
                                    object_class_names.insert(module.module_reference + "." + assignment.name);
                                }
                            }
                        }
                    }
                }
                if (is_type(assignment) && is_defined(type(assignment)))
                {
                    const DefinedType& defined = absl::get<DefinedType>(type(assignment));
                    if (!is_a_parameter(defined.type_reference, assignment.parameters))
                    {
                        const Assignment& inner_assignment = resolve(tree, module.module_reference, defined);
                        if (is_object_class(inner_assignment))
                        {
                            object_class_names.insert(module.module_reference + "." + assignment.name);
                        }
                        else if (is_defined_object_class(module.module_reference, defined.type_reference,
                                                         object_class_names))
                        {
                            object_class_names.insert(module.module_reference + "." + assignment.name);
                        }
                    }
                }
                if (is_value(assignment) && is_defined(value(assignment).type))
                {
                    const DefinedType& defined = absl::get<DefinedType>(value(assignment).type);
                    if (!is_a_parameter(defined.type_reference, assignment.parameters))
                    {
                        const Assignment& inner_assignment = resolve(tree, module.module_reference, defined);
                        if (is_object_class(inner_assignment))
                        {
                            object_class_names.insert(module.module_reference + "." + assignment.name);
                        }
                        else if (is_defined_object_class(module.module_reference, defined.type_reference,
                                                         object_class_names))
                        {
                            object_class_names.insert(module.module_reference + "." + assignment.name);
                        }
                    }
                }
                if (is_object_class(assignment))
                {
                    object_class_names.insert(module.module_reference + "." + assignment.name);
                }
            }
        }

        for (const Module& module : tree.modules)
        {
            for (const Import& import : module.imports)
            {
                for (const std::string& imported_name : import.imports)
                {
                    if (is_defined_object_class(import.module_reference, imported_name, object_class_names))
                    {
                        object_class_names.insert(module.module_reference + "." + imported_name);
                    }
                }
            }
        }

        for (auto s : object_class_names)
        {
            std::cout << "Object class names " << s << std::endl;
        }
    } while (object_class_names.size() > old_number_of_names);
    return object_class_names;
}

// Convert usage of object classes to standard ASN.1 types
void resolve_object_classes(Asn1Tree& tree)
{
    std::set<std::string> object_class_names = get_object_class_names(tree);

    for (Module& module : tree.modules)
    {
        for (Assignment& assignment : module.assignments)
        {
            if (absl::holds_alternative<TypeAssignment>(assignment.specific))
            {
                bool skip = false;
                for (const Parameter& parameter : assignment.parameters)
                {
                    if (parameter.governor)
                    {
                        skip = true;
                    }
                }
                if (!skip)
                {
                    object_class_to_concrete(tree, module, absl::get<TypeAssignment>(assignment.specific).type);
                }
            }
        }
    }

    remove_object_classes(tree, object_class_names);
}
