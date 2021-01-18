#include "fast_ber/compiler/TypeAsString.hpp"

#include "fast_ber/compiler/GenerateChoice.hpp"
#include "fast_ber/compiler/Identifier.hpp"
#include "fast_ber/compiler/ResolveType.hpp"
#include "fast_ber/compiler/ValueAsString.hpp"
#include "fast_ber/compiler/ValueType.hpp"

#include "absl/container/flat_hash_set.h"

#include <iostream>

template <typename Type>
std::string identifier_template_params(const Type&, const Module&, const Asn1Tree&,
                                       const std::string& identifier_override)
{
    return "<" + identifier_override + ">";
}

template <typename Collection>
std::string collection_as_string(const Collection& collection, const Module& module, const Asn1Tree& tree,
                                 const std::string& type_name, const std::string& identifier_override,
                                 const std::string&)
{
    std::string res = "struct " + type_name + " {\n";

    bool has_defaults = false;
    for (const ComponentType& component : collection.components)
    {
        if (component.default_value)
        {
            std::string name = component.named_type.name;
            name[0]          = std::toupper(name[0]);
            res += "    struct DefaultValue" + name + " { constexpr static " +
                   value_type(component.named_type.type, module, tree, component.named_type.name, {}) +
                   " get_value() noexcept { return " +
                   value_as_string(resolve_type(tree, module.module_reference, component.named_type),
                                   *component.default_value) +
                   "; } };\n";
            has_defaults = true;
        }
    }
    if (has_defaults)
    {
        res += "\n\n";
    }

    int64_t tag_counter = 0;

    // Define child types

    std::string definitions;
    for (const ComponentType& component : collection.components)
    {
        if (module.tagging_default == TaggingMode::automatic)
        {
            std::string id = Identifier(Class::context_specific, tag_counter++).name();
            definitions += "    " + create_type_assignment(make_type_name(component.named_type.name, type_name),
                                                           component.named_type.type, module, tree, id, false);
        }
        else
        {
            definitions += "    " + create_type_assignment(make_type_name(component.named_type.name, type_name),
                                                           component.named_type.type, module, tree, {}, false);
        }
    }

    res += definitions;
    if (!collection.components.empty())
    {
        res += '\n';
    }

    std::vector<std::string> component_types;

    for (const ComponentType& component : collection.components)
    {
        std::string component_type = make_type_name(component.named_type.name, type_name);
        if (component.is_optional)
        {
            component_type = make_type_optional(component_type, component.optional_storage);
        }
        else if (component.default_value)
        {
            std::string name = component.named_type.name;
            name[0]          = std::toupper(name[0]);
            name             = "DefaultValue" + name;
            component_type   = "Default<" + component_type + ", " + name + ">";
        }
        res += "    " + component_type + " " + component.named_type.name + ";\n";
        component_types.push_back(component_type);
    }
    if (!collection.components.empty())
    {
        res += '\n';
    }

    auto id = identifier_override.empty() ? identifier(collection, module, tree).name() : identifier_override;
    res += "    " + create_template_definition({"Identifier"}) + '\n';
    res += "    size_t encoded_length_with_id() const noexcept;\n";
    res += "    " + create_template_definition({"Identifier"}) + '\n';
    res += "    EncodeResult encode_with_id(absl::Span<uint8_t>) const noexcept;\n";
    res += "    " + create_template_definition({"Identifier"}) + '\n';
    res += "    DecodeResult decode_with_id(BerView) noexcept;\n";

    res += "    size_t encoded_length() const noexcept\n";
    res += "    { return encoded_length_with_id<" + id + ">(); }\n";
    res += "    EncodeResult encode(absl::Span<uint8_t> output) const noexcept\n";
    res += "    { return encode_with_id<" + id + ">(output); }\n";
    res += "    DecodeResult decode(BerView input) noexcept\n";
    res += "    { return decode_with_id<" + id + ">(input); }\n";
    res += "    using AsnId = " + id + ";\n";

    res += "};\n";

    return res;
}

std::string type_as_string(const AnyType& type, const Module& module, const Asn1Tree& tree, const std::string&,
                           const std::string& identifier_override)
{
    return "::fast_ber::Any" + identifier_template_params(type, module, tree, identifier_override);
}
std::string type_as_string(const BitStringType& type, const Module& module, const Asn1Tree& tree, const std::string&,
                           const std::string& identifier_override)
{
    return "::fast_ber::BitString" + identifier_template_params(type, module, tree, identifier_override);
}
std::string type_as_string(const BooleanType& type, const Module& module, const Asn1Tree& tree, const std::string&,
                           const std::string& identifier_override)
{
    return "::fast_ber::Boolean" + identifier_template_params(type, module, tree, identifier_override);
}
std::string type_as_string(const CharacterStringType& type, const Module& module, const Asn1Tree& tree,
                           const std::string&, const std::string& identifier_override)
{
    return to_string(type) + identifier_template_params(type, module, tree, identifier_override);
}
std::string type_as_string(const ChoiceType& choice, const Module& module, const Asn1Tree& tree,
                           const std::string& name, const std::string& identifier_override)
{
    return create_choice_definition(choice, module, tree, name, identifier_override);
}
std::string type_as_string(const DateType& type, const Module& module, const Asn1Tree& tree, const std::string&,
                           const std::string& identifier_override)
{
    return "::fast_ber::Date" + identifier_template_params(type, module, tree, identifier_override);
}
std::string type_as_string(const DateTimeType& type, const Module& module, const Asn1Tree& tree, const std::string&,
                           const std::string& identifier_override)
{
    return "::fast_ber::DateTime" + identifier_template_params(type, module, tree, identifier_override);
}
std::string type_as_string(const DurationType& type, const Module& module, const Asn1Tree& tree, const std::string&,
                           const std::string& identifier_override)
{
    return "::fast_ber::Duration" + identifier_template_params(type, module, tree, identifier_override);
}
std::string type_as_string(const EmbeddedPDVType& type, const Module& module, const Asn1Tree& tree, const std::string&,
                           const std::string& identifier_override)
{
    return "::fast_ber::EmbeddedPDV" + identifier_template_params(type, module, tree, identifier_override);
}
std::string type_as_string(const EnumeratedType& enumerated, const Module&, const Asn1Tree&, const std::string&,
                           const std::string&    identifier_override)
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

    if (!identifier_override.empty())
    {
        throw std::runtime_error("Enum must be default tagged");
    }
    return res;
}
std::string type_as_string(const ExternalType& type, const Module& module, const Asn1Tree& tree, const std::string&,
                           const std::string& identifier_override)
{
    return "::fast_ber::External" + identifier_template_params(type, module, tree, identifier_override);
}
std::string type_as_string(const GeneralizedTimeType& type, const Module& module, const Asn1Tree& tree,
                           const std::string&, const std::string& identifier_override)
{
    return "::fast_ber::GeneralizedTime" + identifier_template_params(type, module, tree, identifier_override);
}
std::string type_as_string(const InstanceOfType& type, const Module& module, const Asn1Tree& tree, const std::string&,
                           const std::string& identifier_override)
{
    return "::fast_ber::InstanceOf" + identifier_template_params(type, module, tree, identifier_override);
}
std::string type_as_string(const IntegerType& type, const Module& module, const Asn1Tree& tree, const std::string&,
                           const std::string& identifier_override)
{
    return "::fast_ber::Integer" + identifier_template_params(type, module, tree, identifier_override);
}
std::string type_as_string(const IRIType& type, const Module& module, const Asn1Tree& tree, const std::string&,
                           const std::string& identifier_override)
{
    return "::fast_ber::IRI" + identifier_template_params(type, module, tree, identifier_override);
}
std::string type_as_string(const NullType& type, const Module& module, const Asn1Tree& tree, const std::string&,
                           const std::string& identifier_override)
{
    return "::fast_ber::Null" + identifier_template_params(type, module, tree, identifier_override);
}
std::string type_as_string(const ObjectClassFieldType&, const Module&, const Asn1Tree&, const std::string&,
                           const std::string&)
{
    throw std::runtime_error("ObjectClassFieldType is not serializable!");
}
std::string type_as_string(const ObjectDescriptorType& type, const Module& module, const Asn1Tree& tree,
                           const std::string&, const std::string& identifier_override)
{
    return "::fast_ber::ObjectDescriptor" + identifier_template_params(type, module, tree, identifier_override);
}
std::string type_as_string(const ObjectIdentifierType& type, const Module& module, const Asn1Tree& tree,
                           const std::string&, const std::string& identifier_override)
{
    return "::fast_ber::ObjectIdentifier" + identifier_template_params(type, module, tree, identifier_override);
}
std::string type_as_string(const OctetStringType& type, const Module& module, const Asn1Tree& tree, const std::string&,
                           const std::string& identifier_override)
{
    return "::fast_ber::OctetString" + identifier_template_params(type, module, tree, identifier_override);
}
std::string type_as_string(const RealType& type, const Module& module, const Asn1Tree& tree, const std::string&,
                           const std::string& identifier_override)
{
    return "::fast_ber::Real" + identifier_template_params(type, module, tree, identifier_override);
}
std::string type_as_string(const RelativeIRIType& type, const Module& module, const Asn1Tree& tree, const std::string&,
                           const std::string& identifier_override)
{
    return "::fast_ber::RelativeIRI" + identifier_template_params(type, module, tree, identifier_override);
}
std::string type_as_string(const RelativeOIDType& type, const Module& module, const Asn1Tree& tree, const std::string&,
                           const std::string& identifier_override)
{
    return "::fast_ber::RelativeOID" + identifier_template_params(type, module, tree, identifier_override);
}
std::string type_as_string(const SequenceType& sequence, const Module& module, const Asn1Tree& tree,
                           const std::string& type_name, const std::string& identifier_override)
{
    if (module.tagging_default != TaggingMode::automatic)
    {
        std::vector<Identifier>     previous_optional_ids;
        absl::optional<std::string> previous_optional_type;
        for (const ComponentType& component : sequence.components)
        {
            if (component.is_optional || component.default_value)
            {
                auto outer_ids = identifier(component.named_type.type, module, tree).outer_tags();

                for (const Identifier& previous_optional_id : previous_optional_ids)
                {
                    if (previous_optional_type && previous_optional_id == outer_ids.front())
                    {
                        throw std::runtime_error("SEQUENCE " + type_name +
                                                 " is ambiguous, two optional values in a row with same ID [" +
                                                 component.named_type.name + "] [" + *previous_optional_type + "] [" +
                                                 outer_ids.front().name() + "]");
                    }
                }
                previous_optional_ids  = std::move(outer_ids);
                previous_optional_type = component.named_type.name;
            }
            else
            {
                previous_optional_type = absl::nullopt;
                previous_optional_ids.clear();
            }
        }
    }
    return collection_as_string(sequence, module, tree, type_name, identifier_override, "sequence");
}
std::string type_as_string(const SequenceOfType& sequence, const Module& module, const Asn1Tree& tree,
                           const std::string& type_name, const std::string& identifier_override)
{
    const Type& type = sequence.has_name ? sequence.named_type->type : *sequence.type;

    std::string contained_type_definition =
        create_type_assignment(type_name + "Contained", type, module, tree, {}, false) + '\n';

    std::string res = "SequenceOf<" + type_name + "Contained";
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

    return contained_type_definition + "FAST_BER_ALIAS(" + type_name + ", " + res + ");";
}
std::string type_as_string(const SetType& set, const Module& module, const Asn1Tree& tree, const std::string& type_name,
                           const std::string& identifier_override)
{
    if (module.tagging_default != TaggingMode::automatic)
    {
        absl::flat_hash_set<Identifier> ids;
        for (const ComponentType& component : set.components)
        {
            auto outer_ids = identifier(component.named_type.type, module, tree).outer_tags();
            for (const Identifier& id : outer_ids)
            {
                if (ids.count(id) > 0)
                {
                    throw std::runtime_error("Identifier " + id.name() + " occurs more than once in SET " + type_name);
                }
                ids.insert(id);
            }
        }
    }

    return collection_as_string(set, module, tree, type_name, identifier_override, "set");
}
std::string type_as_string(const SetOfType& set, const Module& module, const Asn1Tree& tree,
                           const std::string& type_name, const std::string& identifier_override)
{
    const Type& type = set.has_name ? set.named_type->type : *set.type;

    std::string contained_type_definition =
        create_type_assignment(type_name + "Contained", type, module, tree, {}, false) + '\n';

    std::string res = "SequenceOf<" + type_name + "Contained";
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

    return contained_type_definition + "FAST_BER_ALIAS(" + type_name + ", " + res + ");";
}
std::string type_as_string(const PrefixedType& prefixed_type, const Module& module, const Asn1Tree& tree,
                           const std::string&, const std::string& identifier_override)
{
    auto id = identifier(prefixed_type, module, tree).name();
    if (!identifier_override.empty())
    {
        id = identifier_override;
    }

    return type_as_string(prefixed_type.tagged_type->type, module, tree, "", id);
}
std::string type_as_string(const TimeType& type, const Module& module, const Asn1Tree& tree, const std::string&,
                           const std::string& identifier_override)
{
    return "::fast_ber::Time" + identifier_template_params(type, module, tree, identifier_override);
}
std::string type_as_string(const TimeOfDayType& type, const Module& module, const Asn1Tree& tree, const std::string&,
                           const std::string& identifier_override)
{
    return "::fast_ber::TimeOfDay" + identifier_template_params(type, module, tree, identifier_override);
}
std::string type_as_string(const UTCTimeType& type, const Module& module, const Asn1Tree& tree, const std::string&,
                           const std::string& identifier_override)
{
    return "::fast_ber::UTCTime" + identifier_template_params(type, module, tree, identifier_override);
}
std::string type_as_string(const DefinedType& defined_type, const Module& module, const Asn1Tree& tree,
                           const std::string& type_name, const std::string& identifier_override)
{
    if (!identifier_override.empty())
    {
        NamedType resolved = resolve_type(tree, module.module_reference, defined_type);
        if (!is_generated(resolved.type))
        {
            return type_as_string(resolved.type, module, tree, type_name, identifier_override);
        }
    }

    const std::string& assigned_type =
        "::fast_ber::" +
        (defined_type.module_reference ? *defined_type.module_reference + "::" : module.module_reference + "::") +
        defined_type.type_reference;

    return assigned_type;
}

struct ToStringHelper
{
    template <typename T>
    std::string operator()(const T& t)
    {
        return type_as_string(t, module, tree, type_name, identifier_override);
    }

    const Module&      module;
    const Asn1Tree&    tree;
    const std::string& type_name;
    const std::string& identifier_override;
};

std::string type_as_string(const BuiltinType& type, const Module& module, const Asn1Tree& tree,
                           const std::string& type_name, const std::string& identifier_override)
{
    ToStringHelper string_helper{module, tree, type_name, identifier_override};
    return absl::visit(string_helper, type);
}

std::string type_as_string(const Type& type, const Module& module, const Asn1Tree& tree, const std::string& type_name,
                           const std::string& identifier_override)
{
    ToStringHelper string_helper{module, tree, type_name, identifier_override};
    return absl::visit(string_helper, type);
}

std::string create_type_assignment(const std::string& name, const Type& assignment_type, const Module& module,
                                   const Asn1Tree& tree, const std::string& identifier_override, bool introduce_type)
{
    std::string res;

    if (is_set(assignment_type) || is_sequence(assignment_type) || is_choice(assignment_type) ||
        is_set_of(assignment_type) || is_sequence_of(assignment_type))
    {
        res += type_as_string(assignment_type, module, tree, name, identifier_override);
    }
    else if (is_enumerated(assignment_type))
    {
        auto id =
            (!identifier_override.empty()) ? identifier_override : identifier(assignment_type, module, tree).name();

        res += "FAST_BER_ALIAS(" + name + ", " + type_as_string(enumerated, module, tree, name, id) + ");";
    }
    else if (is_prefixed(assignment_type))
    {
        PrefixedType prefixed = absl::get<PrefixedType>(absl::get<BuiltinType>(assignment_type));
        res += create_type_assignment(name, prefixed.tagged_type->type, module, tree,
                                      Identifier(prefixed.tagged_type->tag).name(), introduce_type);
    }
    else
    {
        if (introduce_type)
        {
            res += "FAST_BER_ALIAS(" + name + ", " + type_as_string(assignment_type, module, tree, name, id) + ");\n";
        }
        else
        {
            res += "using " + name + " = " + type_as_string(assignment_type, module, tree, name, identifier_override) +
                   ";\n";
        }
    }
    return res;
}

std::string create_type_assignment(const Assignment& assignment, const Module& module, const Asn1Tree& tree)
{
    return create_type_assignment(assignment.name, absl::get<TypeAssignment>(assignment.specific).type, module, tree,
                                  {}, true) +
           "\n";
}
