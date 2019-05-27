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

void remove_object_classes(Asn1Tree& tree, const std::set<std::string>& object_class_names)
{
    for (Module& module : tree.modules)
    {
        module.assignments.erase(
            std::remove_if(
                module.assignments.begin(), module.assignments.end(),
                [&](const Assignment& assignment) {
                    if (absl::holds_alternative<ObjectClassAssignment>(assignment.specific))
                    {
                        return true;
                    }

                    for (const std::string& parameter : assignment.parameters)
                    {
                        if (object_class_names.count(module.module_reference + "." + parameter) > 0)
                        {
                            return true;
                        }
                    }

                    if (absl::holds_alternative<TypeAssignment>(assignment.specific) &&
                        absl::holds_alternative<DefinedType>(absl::get<TypeAssignment>(assignment.specific).type))
                    {
                        if (is_object_class(
                                resolve(tree, module.module_reference,
                                        absl::get<DefinedType>(absl::get<TypeAssignment>(assignment.specific).type))))
                            return true;
                    }
                    return false;
                }),
            module.assignments.end());
    }

    for (Module& module : tree.modules)
    {
        module.imports.erase(
            std::remove_if(module.imports.begin(), module.imports.end(),
                           [&](const Import& import) {
                               for (const std::string& imported_name : import.imports)
                               {
                                   if (object_class_names.count(module.module_reference + "." + imported_name) > 0)
                                   {
                                       return true;
                                   }
                               }
                               return false;
                           }),
            module.imports.end());
    }
}

// Convert usage of object classes to standard ASN.1 types
void resolve_object_classes(Asn1Tree& tree)
{
    std::set<std::string> object_class_names;

    for (Module& module : tree.modules)
    {
        for (Assignment& assignment : module.assignments)
        {
            if (absl::holds_alternative<TypeAssignment>(assignment.specific))
            {
                object_class_to_concrete(tree, module, absl::get<TypeAssignment>(assignment.specific).type);

                if (absl::holds_alternative<TypeAssignment>(assignment.specific) &&
                    absl::holds_alternative<DefinedType>(absl::get<TypeAssignment>(assignment.specific).type))
                {
                    Assignment& inner_assignment =
                        resolve(tree, module.module_reference,
                                absl::get<DefinedType>(absl::get<TypeAssignment>(assignment.specific).type));
                    if (is_object_class(inner_assignment))
                    {
                        object_class_names.insert(module.module_reference + "." + inner_assignment.name);
                    }
                }
            }
            else if (absl::holds_alternative<ObjectClassAssignment>(assignment.specific))
            {
                object_class_names.insert(module.module_reference + "." + assignment.name);
            }
        }
    }

    remove_object_classes(tree, object_class_names);
}
