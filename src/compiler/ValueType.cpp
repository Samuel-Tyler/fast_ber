#include "fast_ber/compiler/ValueType.hpp"

#include "fast_ber/compiler/Identifier.hpp"
#include "fast_ber/compiler/ResolveType.hpp"

#include "absl/container/flat_hash_set.h"

#include <iostream>

std::string identifier_template_params(const std::string& identifier_override)
{
    return "<" + identifier_override + ">";
}

std::string value_type(const AnyType&, const Module&, const Asn1Tree&, const std::string&,
                       const std::string& identifier_override)
{
    return "Any" + identifier_template_params(identifier_override);
}
std::string value_type(const BitStringType&, const Module&, const Asn1Tree&, const std::string&, const std::string&)
{
    return "absl::string_view";
}
std::string value_type(const BooleanType&, const Module&, const Asn1Tree&, const std::string&, const std::string&)
{
    return "bool";
}
std::string value_type(const CharacterStringType&, const Module&, const Asn1Tree&, const std::string&,
                       const std::string&)
{
    return "absl::string_view";
}
std::string value_type(const ChoiceType&, const Module&, const Asn1Tree&, const std::string& type_name,
                       const std::string& identifier_override)
{
    return type_name + identifier_template_params(identifier_override);
}
std::string value_type(const DateType&, const Module&, const Asn1Tree&, const std::string&,
                       const std::string& identifier_override)
{
    return "Date" + identifier_template_params(identifier_override);
}
std::string value_type(const DateTimeType&, const Module&, const Asn1Tree&, const std::string&,
                       const std::string& identifier_override)
{
    return "DateTime" + identifier_template_params(identifier_override);
}
std::string value_type(const DurationType&, const Module&, const Asn1Tree&, const std::string&,
                       const std::string& identifier_override)
{
    return "Duration" + identifier_template_params(identifier_override);
}
std::string value_type(const EmbeddedPDVType&, const Module&, const Asn1Tree&, const std::string&,
                       const std::string& identifier_override)
{
    return "EmbeddedPDV" + identifier_template_params(identifier_override);
}
std::string value_type(const EnumeratedType& , const Module&, const Asn1Tree&, const std::string& assigned_type,
                       const std::string&)
{
    return assigned_type + "Values";
}
std::string value_type(const ExternalType&, const Module&, const Asn1Tree&, const std::string&,
                       const std::string& identifier_override)
{
    return "External" + identifier_template_params(identifier_override);
}
std::string value_type(const GeneralizedTimeType&, const Module&, const Asn1Tree&, const std::string&,
                       const std::string& identifier_override)
{
    return "GeneralizedTime" + identifier_template_params(identifier_override);
}
std::string value_type(const InstanceOfType&, const Module&, const Asn1Tree&, const std::string&,
                       const std::string& identifier_override)
{
    return "InstanceOf" + identifier_template_params(identifier_override);
}
std::string value_type(const IntegerType&, const Module&, const Asn1Tree&, const std::string&, const std::string&)
{
    return "int64_t";
}
std::string value_type(const IRIType&, const Module&, const Asn1Tree&, const std::string&,
                       const std::string& identifier_override)
{
    return "IRI" + identifier_template_params(identifier_override);
}
std::string value_type(const NullType&, const Module&, const Asn1Tree&, const std::string&, const std::string&)
{
    return "nullptr_t";
}
std::string value_type(const ObjectClassFieldType&, const Module&, const Asn1Tree&, const std::string&,
                       const std::string&)
{
    throw std::runtime_error("ObjectClassFieldType is not serializable!");
}
std::string value_type(const ObjectDescriptorType&, const Module&, const Asn1Tree&, const std::string&,
                       const std::string& identifier_override)
{
    return "ObjectDescriptor" + identifier_template_params(identifier_override);
}
std::string value_type(const ObjectIdentifierType&, const Module&, const Asn1Tree&, const std::string&,
                       const std::string& identifier_override)
{
    return "ObjectIdentifier" + identifier_template_params(identifier_override);
}
std::string value_type(const OctetStringType&, const Module&, const Asn1Tree&, const std::string&, const std::string&)
{
    return "absl::string_view";
}
std::string value_type(const RealType&, const Module&, const Asn1Tree&, const std::string&,
                       const std::string& identifier_override)
{
    return "Real" + identifier_template_params(identifier_override);
}
std::string value_type(const RelativeIRIType&, const Module&, const Asn1Tree&, const std::string&,
                       const std::string& identifier_override)
{
    return "RelativeIRI" + identifier_template_params(identifier_override);
}
std::string value_type(const RelativeOIDType&, const Module&, const Asn1Tree&, const std::string&,
                       const std::string& identifier_override)
{
    return "RelativeOID" + identifier_template_params(identifier_override);
}
std::string value_type(const SequenceType&, const Module&, const Asn1Tree&, const std::string& type_name,
                       const std::string& identifier_override)
{
    return type_name + identifier_template_params(identifier_override);
}
std::string value_type(const SequenceOfType& sequence, const Module& module, const Asn1Tree& tree, const std::string&,
                       const std::string& identifier_override)
{
    std::string name = sequence.has_name ? sequence.named_type->name : "";
    const Type& type = sequence.has_name ? sequence.named_type->type : *sequence.type;
    std::string res  = "SequenceOf<" + value_type(type, module, tree, name, {});
    if (identifier_override.empty())
    {
        res += ", ExplicitId<UniversalTag::sequence>";
    }
    else
    {
        res += ", " + identifier_override;
    }

    res += ", " + to_string(sequence.storage);
    res += ">";

    return res;
}
std::string value_type(const SetType&, const Module&, const Asn1Tree&, const std::string& type_name,
                       const std::string& identifier_override)
{
    return type_name + identifier_template_params(identifier_override);
}
std::string value_type(const SetOfType& set, const Module& module, const Asn1Tree& tree, const std::string&,
                       const std::string& identifier_override)
{
    std::string name = set.has_name ? set.named_type->name : "";
    const Type& type = set.has_name ? set.named_type->type : *set.type;
    std::string res  = "SetOf<" + value_type(type, module, tree, set.named_type->name, {});
    if (identifier_override.empty())
    {
        res += ", ExplicitId<UniversalTag::sequence>";
    }
    else
    {
        res += ", " + identifier_override;
    }

    res += ", " + to_string(set.storage);
    res += ">";

    return res;
}
std::string value_type(const PrefixedType& prefixed_type, const Module& module, const Asn1Tree& tree,
                       const std::string&, const std::string& identifier_override)
{
    auto id = identifier(prefixed_type, module, tree).name();
    if (!identifier_override.empty())
    {
        id = identifier_override;
    }

    return value_type(prefixed_type.tagged_type->type, module, tree, "", id);
}
std::string value_type(const TimeType&, const Module&, const Asn1Tree&, const std::string&,
                       const std::string& identifier_override)
{
    return "Time" + identifier_template_params(identifier_override);
}
std::string value_type(const TimeOfDayType&, const Module&, const Asn1Tree&, const std::string&,
                       const std::string& identifier_override)
{
    return "TimeOfDay" + identifier_template_params(identifier_override);
}
std::string value_type(const UTCTimeType&, const Module&, const Asn1Tree&, const std::string&,
                       const std::string& identifier_override)
{
    return "UTCTime" + identifier_template_params(identifier_override);
}
std::string value_type(const DefinedType& defined_type, const Module& module, const Asn1Tree& tree, const std::string&,
                       const std::string& identifier_override)
{
    const NamedType& referenced = resolve_type(tree, module.module_reference, defined_type);
    return value_type(referenced.type, module,tree, referenced.name, identifier_override);
}

struct ValueTypeHelper
{
    template <typename T>
    std::string operator()(const T& t)
    {
        return value_type(t, module, tree, type_name, identifier_override);
    }

    const Module&      module;
    const Asn1Tree&    tree;
    const std::string& type_name;
    const std::string& identifier_override;
};

std::string value_type(const BuiltinType& type, const Module& module, const Asn1Tree& tree,
                       const std::string& type_name, const std::string& identifier_override)
{
    ValueTypeHelper string_helper{module, tree, type_name, identifier_override};
    return absl::visit(string_helper, type);
}
std::string value_type(const Type& type, const Module& module, const Asn1Tree& tree, const std::string& type_name,
                       const std::string& identifier_override)
{
    ValueTypeHelper string_helper{module, tree, type_name, identifier_override};
    return absl::visit(string_helper, type);
}
