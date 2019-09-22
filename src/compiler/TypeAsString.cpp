#include "fast_ber/compiler/TypeAsString.hpp"
#include "fast_ber/compiler/Identifier.hpp"
#include "fast_ber/compiler/ResolveType.hpp"

static int unnamed_definition_reference_num = 0;

template <typename Type>
std::string wrap_with_tagged_type(const Type& type, const Module& module, const Asn1Tree& tree,
                                  const std::string& type_string)
{
    const auto tagging_info = identifier(type, module, tree);
    if (tagging_info.is_default_tagged)
    {
        return type_string;
    }

    return "TaggedType<" + type_string + ", " + tagging_info.tag + ">";
}

template <typename Type>
std::string identifier_template_params(const Type& type, const Module& module, const Asn1Tree& tree)
{
    const auto tagging_info = identifier(type, module, tree);
    if (tagging_info.is_default_tagged)
    {
        return "<>";
    }
    return "<" + tagging_info.tag + ">";
}

std::string type_as_string(const AnyType& type, const Module& module, const Asn1Tree& tree)
{
    return "Any" + identifier_template_params(type, module, tree);
}
std::string type_as_string(const BitStringType& type, const Module& module, const Asn1Tree& tree)
{
    return "BitString" + identifier_template_params(type, module, tree);
}
std::string type_as_string(const BooleanType& type, const Module& module, const Asn1Tree& tree)
{
    return "Boolean" + identifier_template_params(type, module, tree);
}
std::string type_as_string(const CharacterStringType& type, const Module& module, const Asn1Tree& tree)
{
    return to_string(type) + identifier_template_params(type, module, tree);
}
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
            res += type_as_string(named_type.type, module, tree);
        }

        is_first = false;
    }

    res += ">";

    return wrap_with_tagged_type(choice, module, tree, res);
}
std::string type_as_string(const DateType& type, const Module& module, const Asn1Tree& tree)
{
    return "Date" + identifier_template_params(type, module, tree);
}
std::string type_as_string(const DateTimeType& type, const Module& module, const Asn1Tree& tree)
{
    return "DateTime" + identifier_template_params(type, module, tree);
}
std::string type_as_string(const DurationType& type, const Module& module, const Asn1Tree& tree)
{
    return "Duration" + identifier_template_params(type, module, tree);
}
std::string type_as_string(const EmbeddedPDVType& type, const Module& module, const Asn1Tree& tree)
{
    return "EmbeddedPDV" + identifier_template_params(type, module, tree);
}
std::string type_as_string(const EnumeratedType& enumerated, const Module& module, const Asn1Tree& tree)
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

    if (!identifier(enumerated, module, tree).is_default_tagged)
    {
        throw std::runtime_error("Enum must be default tagged");
    }
    return res;
}
std::string type_as_string(const ExternalType& type, const Module& module, const Asn1Tree& tree)
{
    return "External" + identifier_template_params(type, module, tree);
}
std::string type_as_string(const GeneralizedTimeType& type, const Module& module, const Asn1Tree& tree)
{
    return "GeneralizedTime" + identifier_template_params(type, module, tree);
}
std::string type_as_string(const InstanceOfType& type, const Module& module, const Asn1Tree& tree)
{
    return "InstanceOf" + identifier_template_params(type, module, tree);
}
std::string type_as_string(const IntegerType& type, const Module& module, const Asn1Tree& tree)
{
    return "Integer" + identifier_template_params(type, module, tree);
}
std::string type_as_string(const IRIType& type, const Module& module, const Asn1Tree& tree)
{
    return "IRI" + identifier_template_params(type, module, tree);
}
std::string type_as_string(const NullType& type, const Module& module, const Asn1Tree& tree)
{
    return "Null" + identifier_template_params(type, module, tree);
}
std::string type_as_string(const ObjectClassFieldType&, const Module&, const Asn1Tree&)
{
    throw std::runtime_error("ObjectClassFieldType is not serializable!");
}
std::string type_as_string(const ObjectDescriptorType& type, const Module& module, const Asn1Tree& tree)
{
    return "ObjectDescriptor" + identifier_template_params(type, module, tree);
}
std::string type_as_string(const ObjectIdentifierType& type, const Module& module, const Asn1Tree& tree)
{
    return "ObjectIdentifier" + identifier_template_params(type, module, tree);
}
std::string type_as_string(const OctetStringType& type, const Module& module, const Asn1Tree& tree)
{
    return "OctetString" + identifier_template_params(type, module, tree);
}
std::string type_as_string(const RealType& type, const Module& module, const Asn1Tree& tree)
{
    return "Real" + identifier_template_params(type, module, tree);
}
std::string type_as_string(const RelativeIRIType& type, const Module& module, const Asn1Tree& tree)
{
    return "RelativeIRI" + identifier_template_params(type, module, tree);
}
std::string type_as_string(const RelativeOIDType& type, const Module& module, const Asn1Tree& tree)
{
    return "RelativeOID" + identifier_template_params(type, module, tree);
}
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

    res += "    using Id = ExplicitIdentifier<UniversalTag::sequence>;";
    res += "    using ExplicitId = ExplicitIdentifier<UniversalTag::sequence>;";
    res += "};\n";

    if (!identifier(sequence, module, tree).is_default_tagged)
    {
        throw std::runtime_error("Sequence must be default tagged");
    }

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
    res += "    using Id = ExplicitIdentifier<UniversalTag::set>;";
    res += "    using ExplicitId = ExplicitIdentifier<UniversalTag::set>;";
    res += "};\n";

    if (!identifier(set, module, tree).is_default_tagged)
    {
        throw std::runtime_error("Set must be default tagged");
    }

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
    auto child_type = type_as_string(prefixed_type.tagged_type->type, module, tree);
    if (is_sequence(prefixed_type.tagged_type->type))
    {
        child_type = "UnnamedSequence" + std::to_string(unnamed_definition_reference_num++);
    }
    else if (is_set(prefixed_type.tagged_type->type))
    {
        child_type = "UnnamedSet" + std::to_string(unnamed_definition_reference_num++);
    }
    else if (is_enumerated(prefixed_type.tagged_type->type))
    {
        child_type = "UnnamedEnum" + std::to_string(unnamed_definition_reference_num++);
    }

    return wrap_with_tagged_type(prefixed_type, module, tree, child_type);
}
std::string type_as_string(const TimeType& type, const Module& module, const Asn1Tree& tree)
{
    return "Time" + identifier_template_params(type, module, tree);
}
std::string type_as_string(const TimeOfDayType& type, const Module& module, const Asn1Tree& tree)
{
    return "TimeOfDay" + identifier_template_params(type, module, tree);
}
std::string type_as_string(const UTCTimeType& type, const Module& module, const Asn1Tree& tree)
{
    return "UTCTime" + identifier_template_params(type, module, tree);
}
std::string type_as_string(const DefinedType& type, const Module&, const Asn1Tree&) { return type.type_reference; }

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
