#pragma once

#include "fast_ber/compiler/CompilerTypes.hpp"
#include "fast_ber/compiler/ResolveTypeFwd.hpp"

std::string type_as_string(const AnyType&, const Module&, const Asn1Tree&);
std::string type_as_string(const BitStringType&, const Module&, const Asn1Tree&);
std::string type_as_string(const BooleanType&, const Module&, const Asn1Tree&);
std::string type_as_string(const CharacterStringType&, const Module&, const Asn1Tree&);
std::string type_as_string(const ChoiceType&, const Module&, const Asn1Tree&);
std::string type_as_string(const DateType&, const Module&, const Asn1Tree&);
std::string type_as_string(const DateTimeType&, const Module&, const Asn1Tree&);
std::string type_as_string(const DurationType&, const Module&, const Asn1Tree&);
std::string type_as_string(const EmbeddedPDVType&, const Module&, const Asn1Tree&);
std::string type_as_string(const EnumeratedType&, const Module&, const Asn1Tree&);
std::string type_as_string(const ExternalType&, const Module&, const Asn1Tree&);
std::string type_as_string(const GeneralizedTimeType& type, const Module&, const Asn1Tree&);
std::string type_as_string(const InstanceOfType&, const Module&, const Asn1Tree&);
std::string type_as_string(const IntegerType&, const Module&, const Asn1Tree&);
std::string type_as_string(const IRIType&, const Module&, const Asn1Tree& tree);
std::string type_as_string(const NullType&, const Module&, const Asn1Tree& tree);
std::string type_as_string(const ObjectClassFieldType&, const Module&, const Asn1Tree&);
std::string type_as_string(const ObjectDescriptorType&, const Module&, const Asn1Tree&);
std::string type_as_string(const ObjectIdentifierType&, const Module&, const Asn1Tree&);
std::string type_as_string(const OctetStringType&, const Module&, const Asn1Tree&);
std::string type_as_string(const RealType&, const Module&, const Asn1Tree&);
std::string type_as_string(const RelativeIRIType&, const Module&, const Asn1Tree&);
std::string type_as_string(const RelativeOIDType&, const Module&, const Asn1Tree&);
std::string type_as_string(const SequenceType&, const Module&, const Asn1Tree&);
std::string type_as_string(const SequenceOfType&, const Module&, const Asn1Tree&);
std::string type_as_string(const SetType&, const Module&, const Asn1Tree&);
std::string type_as_string(const SetOfType&, const Module&, const Asn1Tree&);
std::string type_as_string(const PrefixedType&, const Module&, const Asn1Tree&);
std::string type_as_string(const TimeType&, const Module&, const Asn1Tree&);
std::string type_as_string(const TimeOfDayType&, const Module&, const Asn1Tree&);
std::string type_as_string(const UTCTimeType&, const Module&, const Asn1Tree&);
std::string type_as_string(const DefinedType&, const Module&, const Asn1Tree&);
std::string type_as_string(const BuiltinType&, const Module&, const Asn1Tree&);
std::string type_as_string(const Type&, const Module&, const Asn1Tree&);
std::string fully_tagged_type(const Type& type, const Module&, const Asn1Tree&);

TaggingInfo identifier(const DefinedType&, const Module&, const Module&, const Asn1Tree&);
TaggingInfo identifier(const BuiltinType& type, const Module&, const Module&, const Asn1Tree&);
TaggingInfo identifier(const Type& type, const Module&, const Module&, const Asn1Tree&);

std::string type_as_string(const AnyType&, const Module&, const Asn1Tree&) { return "Any"; }
std::string type_as_string(const BitStringType&, const Module&, const Asn1Tree&) { return "BitString"; }
std::string type_as_string(const BooleanType&, const Module&, const Asn1Tree&) { return "Boolean"; }
std::string type_as_string(const CharacterStringType&, const Module&, const Asn1Tree&) { return "CharacterString"; }
std::string type_as_string(const ChoiceType& choice, const Module& module, const Asn1Tree& tree)
{
    bool        is_first = true;
    std::string res      = "Choice<";
    for (const auto& named_type : choice.choices)
    {
        if (!is_first)
            res += ", ";

        if (is_sequence(named_type.type))
        {
            res += "UnnamedSequence" + std::to_string(unnamed_definition_reference_num++);
        }
        else if (is_set(named_type.type))
        {
            res += "UnnamedSet" + std::to_string(unnamed_definition_reference_num++);
        }
        else if (is_enumerated(named_type.type))
        {
            return "UnnamedEnum" + std::to_string(unnamed_definition_reference_num++);
        }
        else
        {
            res += fully_tagged_type(named_type.type, module, tree);
        }

        is_first = false;
    }

    res += ">";
    return res;
}
std::string type_as_string(const DateType&, const Module&, const Asn1Tree&) { return "Date"; }
std::string type_as_string(const DateTimeType&, const Module&, const Asn1Tree&) { return "DateTime"; }
std::string type_as_string(const DurationType&, const Module&, const Asn1Tree&) { return "Duration"; }
std::string type_as_string(const EmbeddedPDVType&, const Module&, const Asn1Tree&) { return "EmbeddedPDV"; }
std::string type_as_string(const EnumeratedType& enumerated, const Module&, const Asn1Tree&)
{
    std::string res = " {\n";
    for (const EnumerationValue& enum_value : enumerated.enum_values)
    {
        res += "    " + enum_value.name;
        if (enum_value.value)
        {
            res += " = " + std::to_string(*enum_value.value);
        }
        res += ",\n";
    }
    res += "};\n\n";

    return res;
}
std::string type_as_string(const ExternalType&, const Module&, const Asn1Tree&) { return "External"; }
std::string type_as_string(const GeneralizedTimeType&, const Module&, const Asn1Tree&) { return "GeneralizedTime"; }
std::string type_as_string(const InstanceOfType&, const Module&, const Asn1Tree&) { return "InstanceOf"; }
std::string type_as_string(const IntegerType&, const Module&, const Asn1Tree&) { return "Integer"; }
std::string type_as_string(const IRIType&, const Module&, const Asn1Tree&) { return "IRI"; }
std::string type_as_string(const NullType&, const Module&, const Asn1Tree&) { return "Null"; }
std::string type_as_string(const ObjectClassFieldType& object_class_field, const Module& module, const Asn1Tree& tree)
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
                    return type_as_string(absl::get<FixedTypeValueField>(field.field).type, module, tree);
                }
                throw std::runtime_error("Referenced class filed does not have a type " +
                                         object_class_field.referenced_object_class.type_reference);
            }
        }
    }

    throw std::runtime_error("Failed to parse object field reference " +
                             object_class_field.referenced_object_class.type_reference);
}
std::string type_as_string(const ObjectDescriptorType&, const Module&, const Asn1Tree&) { return "ObjectDescriptor"; }
std::string type_as_string(const ObjectIdentifierType&, const Module&, const Asn1Tree&) { return "ObjectIdentifier"; }
std::string type_as_string(const OctetStringType&, const Module&, const Asn1Tree&) { return "OctetString"; }
std::string type_as_string(const RealType&, const Module&, const Asn1Tree&) { return "Real"; }
std::string type_as_string(const RelativeIRIType&, const Module&, const Asn1Tree&) { return "RelativeIRI"; }
std::string type_as_string(const RelativeOIDType&, const Module&, const Asn1Tree&) { return "RelativeOID"; }
std::string type_as_string(const SequenceType& sequence, const Module& module, const Asn1Tree& tree)
{
    std::string res = " {\n";

    for (const ComponentType& component : sequence.components)
    {
        std::string component_type = type_as_string(component.named_type.type, module, tree);
        if (is_enumerated(component.named_type.type))
        {
            component_type = "UnnamedEnum" + std::to_string(unnamed_definition_reference_num++);
        }

        if (is_set(component.named_type.type) || is_sequence(component.named_type.type))
        {
            res += "struct " + component.named_type.name + "_type " + component_type;
            res += "    " + component.named_type.name + "_type " + component.named_type.name + ";\n";
        }
        else
        {
            if (component.is_optional)
            {
                component_type = make_type_optional(component_type);
            }
            res += "    " + component_type + " " + component.named_type.name + ";\n";
        }
    }
    res += "};\n";

    return res;
}
std::string type_as_string(const SequenceOfType& sequence, const Module& module, const Asn1Tree& tree)
{
    const Type& type = sequence.has_name ? sequence.named_type->type : *sequence.type;

    if (is_sequence(type))
    {
        if (sequence.has_name)
        {
            return "SequenceOf<" + sequence.named_type->name + ">";
        }
        return "SequenceOf<UnnamedSequence" + std::to_string(unnamed_definition_reference_num++) + ">";
    }
    else if (is_set(type))
    {
        if (sequence.has_name)
        {
            return "SequenceOf<" + sequence.named_type->name + ">";
        }
        return "SequenceOf<UnnamedSet" + std::to_string(unnamed_definition_reference_num++) + ">";
    }
    else if (is_enumerated(type))
    {
        if (sequence.has_name)
        {
            return "SequenceOf<" + sequence.named_type->name + ">";
        }
        return "SequenceOf<UnnamedEnum" + std::to_string(unnamed_definition_reference_num++) + ">";
    }
    else
    {
        return "SequenceOf<" + type_as_string(type, module, tree) + ">";
    }
}
std::string type_as_string(const SetType& set, const Module& module, const Asn1Tree& tree)
{
    std::string res = " {\n";

    for (const ComponentType& component : set.components)
    {
        std::string component_type = type_as_string(component.named_type.type, module, tree);
        if (is_enumerated(component.named_type.type))
        {
            component_type = "UnnamedEnum" + std::to_string(unnamed_definition_reference_num++);
        }

        if (is_set(component.named_type.type) || is_sequence(component.named_type.type))
        {
            res += "    struct " + component.named_type.name + "_type " + component_type;
            res += component.named_type.name + "_type " + component.named_type.name + ";\n";
        }
        else
        {
            if (component.is_optional)
            {
                component_type = make_type_optional(component_type);
            }
            res += "    " + component_type + " " + component.named_type.name + ";\n";
        }
    }
    res += "};\n";

    return res;
}
std::string type_as_string(const SetOfType& set, const Module& module, const Asn1Tree& tree)
{
    const Type& type = set.has_name ? set.named_type->type : *set.type;

    if (is_sequence(type))
    {
        if (set.has_name)
        {
            return "SequenceOf<" + set.named_type->name + ">";
        }
        return "SequenceOf<UnnamedSequence" + std::to_string(unnamed_definition_reference_num++) + ">";
    }
    else if (is_set(type))
    {
        if (set.has_name)
        {
            return "SequenceOf<" + set.named_type->name + ">";
        }
        return "SequenceOf<UnnamedSet" + std::to_string(unnamed_definition_reference_num++) + ">";
    }
    else if (is_enumerated(type))
    {
        if (set.has_name)
        {
            return "SequenceOf<" + set.named_type->name + ">";
        }
        return "SequenceOf<UnnamedEnum" + std::to_string(unnamed_definition_reference_num++) + ">";
    }
    else
    {
        return "SequenceOf<" + type_as_string(type, module, tree) + ">";
    }
}
std::string type_as_string(const PrefixedType& prefixed_type, const Module& module, const Asn1Tree& tree)
{
    if (is_sequence(prefixed_type.tagged_type->type))
    {
        return "UnnamedSequence" + std::to_string(unnamed_definition_reference_num++);
    }
    else if (is_set(prefixed_type.tagged_type->type))
    {
        return "UnnamedSet" + std::to_string(unnamed_definition_reference_num++);
    }
    else if (is_enumerated(prefixed_type.tagged_type->type))
    {
        return "UnnamedEnum" + std::to_string(unnamed_definition_reference_num++);
    }
    return type_as_string(prefixed_type.tagged_type->type, module, tree);
}
std::string type_as_string(const TimeType&, const Module&, const Asn1Tree&) { return "Time"; }
std::string type_as_string(const TimeOfDayType&, const Module&, const Asn1Tree&) { return "TimeOfDay"; }
std::string type_as_string(const UTCTimeType&, const Module&, const Asn1Tree&) { return "UTCTime"; }

std::string type_as_string(const DefinedType& type, const Module& module, const Asn1Tree& tree)
{
    if (!type.parameters.empty())
    {
        std::set<std::string> parameter_types;
        for (const Type& paramter : type.parameters)
        {
            parameter_types.insert(type_as_string(paramter, module, tree));
        }
        return type.type_reference + create_template_arguments(parameter_types);
    }
    return type.type_reference;
}

struct ToStringHelper
{
    template <typename T>
    std::string operator()(const T& t)
    {
        return type_as_string(t, module, tree);
    }

    const Module&   module;
    const Asn1Tree& tree;
};

std::string type_as_string(const BuiltinType& type, const Module& module, const Asn1Tree& tree)
{
    ToStringHelper string_helper{module, tree};
    return absl::visit(string_helper, type);
}
std::string type_as_string(const Type& type, const Module& module, const Asn1Tree& tree)
{
    ToStringHelper string_helper{module, tree};
    return absl::visit(string_helper, type);
}
