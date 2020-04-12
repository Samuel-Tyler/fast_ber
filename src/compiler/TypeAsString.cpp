#include "fast_ber/compiler/TypeAsString.hpp"

#include "fast_ber/compiler/Identifier.hpp"
#include "fast_ber/compiler/ResolveType.hpp"
#include "fast_ber/compiler/ValueAsString.hpp"
#include "fast_ber/compiler/ValueType.hpp"

#include "absl/container/flat_hash_set.h"

#include <iostream>

thread_local static size_t id_counter = 0;

template <typename Type>
std::string identifier_template_params(const Type&, const Module&, const Asn1Tree&,
                                       const std::string& identifier_override)
{
    return "<" + identifier_override + ">";
}

template <typename Collection>
std::string collection_as_string(const Collection& collection, const Module& module, const Asn1Tree& tree,
                                 const std::string& type_name, const std::string& identifier_override,
                                 const std::string& collection_name)
{
    std::string res = " {\n";

    bool has_defaults;
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

    for (const ComponentType& component : collection.components)
    {
        std::string component_type;
        if (is_set(component.named_type.type) || is_sequence(component.named_type.type))
        {
            const std::string id_template_param = "Identifier" + std::to_string(id_counter++);
            component_type =
                type_as_string(component.named_type.type, module, tree, component.named_type.name + "_type");

            res += create_template_definition(
                {id_template_param + " = ExplicitId<UniversalTag::" + collection_name + ">"});
            res += "struct " + component.named_type.name + "_type " + component_type;
        }
    }

    std::vector<std::string> component_types;

    for (const ComponentType& component : collection.components)
    {
        std::string component_type;

        if (is_set(component.named_type.type) || is_sequence(component.named_type.type))
        {
            component_type = component.named_type.name + "_type<> ";
        }
        else if (!is_prefixed(component.named_type.type) && module.tagging_default == TaggingMode::automatic)
        {
            component_type = type_as_string(component.named_type.type, module, tree, "",
                                            Identifier(Class::context_specific, tag_counter++).name());
        }
        else
        {
            component_type = type_as_string(component.named_type.type, module, tree);
        }

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

    res += "\n\n";
    res += "    " + type_name + "() noexcept {}\n";
    if (collection.components.size() > 0)
    {
        bool is_first = true;
        if (collection.components.size() == 1)
        {
            res += "    explicit " + type_name + "(";
        }
        else
        {
            res += "    " + type_name + "(";
        }
        for (size_t i = 0; i < collection.components.size(); i++)
        {
            if (!is_first)
            {
                res += ", ";
            }
            res += "const " + component_types[i] + "& t" + std::to_string(i);
            is_first = false;
        }
        size_t counter = 0;
        res += ")\n";
        for (const ComponentType& component : collection.components)
        {
            res += "        ";
            if (counter == 0)
            {
                res += ": ";
            }
            else
            {
                res += ", ";
            }

            res += component.named_type.name + "(t" + std::to_string(counter) + ")\n";
            counter++;
        }
        res += "    {}\n";
    }
    bool is_first = true;
    res += "    template <typename Identifier2>\n";
    res += "    " + type_name + "(const " + type_name + "<Identifier2>& rhs)\n";
    for (const ComponentType& component : collection.components)
    {
        res += "        ";
        if (is_first)
        {
            res += ": ";
        }
        else
        {
            res += ", ";
        }

        res += component.named_type.name + "(rhs." + component.named_type.name + ")\n";
        is_first = false;
    }
    if (collection.components.size() == 0)
    {
        res += "        {(void)rhs;}\n";
    }
    else
    {
        res += "        {}\n";
    }

    is_first = true;
    res += "    template <typename Identifier2>\n";
    res += "    " + type_name + "(" + type_name + "<Identifier2>&& rhs) noexcept\n";
    for (const ComponentType& component : collection.components)
    {
        res += "        ";
        if (is_first)
        {
            res += ": ";
        }
        else
        {
            res += ", ";
        }

        res += component.named_type.name + "(std::move(rhs." + component.named_type.name + "))\n";
        is_first = false;
    }
    if (collection.components.size() == 0)
    {
        res += "        {(void)rhs;}\n";
    }
    else
    {
        res += "        {}\n";
    }

    res += "\n    template <typename Identifier2>\n";
    res += "    " + type_name + "& operator=(const " + type_name + "<Identifier2>& rhs)\n";
    res += "    {\n";
    if (collection.components.size() == 0)
    {
        res += "        (void)rhs;\n";
    }
    for (const ComponentType& component : collection.components)
    {
        res += "        " + component.named_type.name + " = rhs." + component.named_type.name + ";\n";
    }
    res += "        return *this;\n";
    res += "    }\n";

    res += "    template <typename Identifier2>\n";
    res += "    " + type_name + "& operator=(" + type_name + "<Identifier2>&& rhs) noexcept\n";
    res += "    {\n";
    if (collection.components.size() == 0)
    {
        res += "        (void)rhs;\n";
    }
    for (const ComponentType& component : collection.components)
    {
        res += "        " + component.named_type.name + " = std::move(rhs." + component.named_type.name + ");\n";
    }
    res += "        return *this;\n";
    res += "    }\n";
    res += "    size_t encoded_length() const noexcept;\n";
    res += "    EncodeResult encode(absl::Span<uint8_t>) const noexcept;\n";
    res += "    DecodeResult decode(BerView) noexcept;\n";
    res += "    using AsnId = Identifier;\n";
    res += "};\n";

    if (!identifier(collection, module, tree).is_default_tagged || !identifier_override.empty())
    {
        throw std::runtime_error(collection_name + " must be default tagged");
    }
    return res;
}

std::string type_as_string(const AnyType& type, const Module& module, const Asn1Tree& tree, const std::string&,
                           const std::string& identifier_override)
{
    return "Any" + identifier_template_params(type, module, tree, identifier_override);
}
std::string type_as_string(const BitStringType& type, const Module& module, const Asn1Tree& tree, const std::string&,
                           const std::string& identifier_override)
{
    return "BitString" + identifier_template_params(type, module, tree, identifier_override);
}
std::string type_as_string(const BooleanType& type, const Module& module, const Asn1Tree& tree, const std::string&,
                           const std::string& identifier_override)
{
    return "Boolean" + identifier_template_params(type, module, tree, identifier_override);
}
std::string type_as_string(const CharacterStringType& type, const Module& module, const Asn1Tree& tree,
                           const std::string&, const std::string& identifier_override)
{
    return to_string(type) + identifier_template_params(type, module, tree, identifier_override);
}
std::string type_as_string(const ChoiceType& choice, const Module& module, const Asn1Tree& tree, const std::string&,
                           const std::string& identifier_override)
{
    std::string res      = "Choice<Choices<";
    bool        is_first = true;

    int64_t tag_counter = 0;
    for (const auto& named_type : choice.choices)
    {
        if (!is_first)
            res += ", ";

        if (is_sequence(named_type.type) || is_set(named_type.type) || is_enumerated(named_type.type))
        {
            throw std::runtime_error("ChoiceType must not be a structure or enum" +
                                     type_as_string(named_type.type, module, tree));
        }

        if (module.tagging_default == TaggingMode::automatic)
        {
            res += type_as_string(named_type.type, module, tree, "",
                                  Identifier(Class::context_specific, tag_counter++).name());
        }
        else
        {
            res += type_as_string(named_type.type, module, tree);
        }
        is_first = false;
    }
    res += ">, ";

    if (identifier_override.empty())
    {
        res += identifier(choice, module, tree).name();
    }
    else
    {
        res += identifier_override;
    }

    res += ", " + to_string(choice.storage);
    res += ">";
    return res;
}
std::string type_as_string(const DateType& type, const Module& module, const Asn1Tree& tree, const std::string&,
                           const std::string& identifier_override)
{
    return "Date" + identifier_template_params(type, module, tree, identifier_override);
}
std::string type_as_string(const DateTimeType& type, const Module& module, const Asn1Tree& tree, const std::string&,
                           const std::string& identifier_override)
{
    return "DateTime" + identifier_template_params(type, module, tree, identifier_override);
}
std::string type_as_string(const DurationType& type, const Module& module, const Asn1Tree& tree, const std::string&,
                           const std::string& identifier_override)
{
    return "Duration" + identifier_template_params(type, module, tree, identifier_override);
}
std::string type_as_string(const EmbeddedPDVType& type, const Module& module, const Asn1Tree& tree, const std::string&,
                           const std::string& identifier_override)
{
    return "EmbeddedPDV" + identifier_template_params(type, module, tree, identifier_override);
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
    return "External" + identifier_template_params(type, module, tree, identifier_override);
}
std::string type_as_string(const GeneralizedTimeType& type, const Module& module, const Asn1Tree& tree,
                           const std::string&, const std::string& identifier_override)
{
    return "GeneralizedTime" + identifier_template_params(type, module, tree, identifier_override);
}
std::string type_as_string(const InstanceOfType& type, const Module& module, const Asn1Tree& tree, const std::string&,
                           const std::string& identifier_override)
{
    return "InstanceOf" + identifier_template_params(type, module, tree, identifier_override);
}
std::string type_as_string(const IntegerType& type, const Module& module, const Asn1Tree& tree, const std::string&,
                           const std::string& identifier_override)
{
    return "Integer" + identifier_template_params(type, module, tree, identifier_override);
}
std::string type_as_string(const IRIType& type, const Module& module, const Asn1Tree& tree, const std::string&,
                           const std::string& identifier_override)
{
    return "IRI" + identifier_template_params(type, module, tree, identifier_override);
}
std::string type_as_string(const NullType& type, const Module& module, const Asn1Tree& tree, const std::string&,
                           const std::string& identifier_override)
{
    return "Null" + identifier_template_params(type, module, tree, identifier_override);
}
std::string type_as_string(const ObjectClassFieldType&, const Module&, const Asn1Tree&, const std::string&,
                           const std::string&)
{
    throw std::runtime_error("ObjectClassFieldType is not serializable!");
}
std::string type_as_string(const ObjectDescriptorType& type, const Module& module, const Asn1Tree& tree,
                           const std::string&, const std::string& identifier_override)
{
    return "ObjectDescriptor" + identifier_template_params(type, module, tree, identifier_override);
}
std::string type_as_string(const ObjectIdentifierType& type, const Module& module, const Asn1Tree& tree,
                           const std::string&, const std::string& identifier_override)
{
    return "ObjectIdentifier" + identifier_template_params(type, module, tree, identifier_override);
}
std::string type_as_string(const OctetStringType& type, const Module& module, const Asn1Tree& tree, const std::string&,
                           const std::string& identifier_override)
{
    return "OctetString" + identifier_template_params(type, module, tree, identifier_override);
}
std::string type_as_string(const RealType& type, const Module& module, const Asn1Tree& tree, const std::string&,
                           const std::string& identifier_override)
{
    return "Real" + identifier_template_params(type, module, tree, identifier_override);
}
std::string type_as_string(const RelativeIRIType& type, const Module& module, const Asn1Tree& tree, const std::string&,
                           const std::string& identifier_override)
{
    return "RelativeIRI" + identifier_template_params(type, module, tree, identifier_override);
}
std::string type_as_string(const RelativeOIDType& type, const Module& module, const Asn1Tree& tree, const std::string&,
                           const std::string& identifier_override)
{
    return "RelativeOID" + identifier_template_params(type, module, tree, identifier_override);
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
                           const std::string&, const std::string& identifier_override)
{
    const Type& type = sequence.has_name ? sequence.named_type->type : *sequence.type;
    if (is_sequence(type) || is_set(type) || is_enumerated(type))
    {
        throw std::runtime_error("SequenceOfType must not be a structure or enum");
    }

    std::string res = "SequenceOf<" + type_as_string(type, module, tree);
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
std::string type_as_string(const SetOfType& set, const Module& module, const Asn1Tree& tree, const std::string&,
                           const std::string& identifier_override)
{
    const Type& type = set.has_name ? set.named_type->type : *set.type;
    if (is_sequence(type) || is_set(type) || is_enumerated(type))
    {
        throw std::runtime_error("SetOfType must not be a structure or enum");
    }

    std::string res = "SetOf<" + type_as_string(type, module, tree);
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
std::string type_as_string(const PrefixedType& prefixed_type, const Module& module, const Asn1Tree& tree,
                           const std::string&, const std::string& identifier_override)
{
    auto id = identifier(prefixed_type, module, tree).name();
    if (!identifier_override.empty())
    {
        id = identifier_override;
    }

    if (is_sequence(prefixed_type.tagged_type->type) || is_set(prefixed_type.tagged_type->type) ||
        is_enumerated(prefixed_type.tagged_type->type))
    {
        throw std::runtime_error("PrefixedType must not be a structure or enum");
    }

    return type_as_string(prefixed_type.tagged_type->type, module, tree, "", id);
}
std::string type_as_string(const TimeType& type, const Module& module, const Asn1Tree& tree, const std::string&,
                           const std::string& identifier_override)
{
    return "Time" + identifier_template_params(type, module, tree, identifier_override);
}
std::string type_as_string(const TimeOfDayType& type, const Module& module, const Asn1Tree& tree, const std::string&,
                           const std::string& identifier_override)
{
    return "TimeOfDay" + identifier_template_params(type, module, tree, identifier_override);
}
std::string type_as_string(const UTCTimeType& type, const Module& module, const Asn1Tree& tree, const std::string&,
                           const std::string& identifier_override)
{
    return "UTCTime" + identifier_template_params(type, module, tree, identifier_override);
}
std::string type_as_string(const DefinedType& defined_type, const Module& module, const Asn1Tree& tree,
                           const std::string&, const std::string& identifier_override)
{
    const std::string& assigned_type =
        (defined_type.module_reference ? *defined_type.module_reference + "::" : std::string("")) +
        defined_type.type_reference;

    if (!identifier_override.empty())
    {
        return assigned_type + "<" + identifier_override + ">";
    }
    else
    {
        // const Type& referenced = resolve_type(tree, module.module_reference, defined_type);
        const Type& referenced = type(resolve(tree, module.module_reference, defined_type));
        return assigned_type + "<" + identifier(referenced, module, tree).name() + ">";
    }
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
