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
    for (const ComponentType& component : collection.components)
    {
        std::string component_type;
        if (is_sequence(component.named_type.type) || is_set(component.named_type.type) ||
            is_choice(component.named_type.type))
        {
            res += type_as_string(component.named_type.type, module, tree, component.named_type.name + "_type");
        }
    }

    std::vector<std::string> component_types;

    for (const ComponentType& component : collection.components)
    {
        std::string component_type;

        if (is_set(component.named_type.type) || is_sequence(component.named_type.type) ||
            is_choice(component.named_type.type))
        {
            component_type = component.named_type.name + "_type ";
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
std::string type_as_string(const ChoiceType& choice, const Module& module, const Asn1Tree& tree,
                           const std::string& name, const std::string& identifier_override)
{
    std::string base     = "Choice<Choices<";
    bool        is_first = true;

    int64_t tag_counter = 0;
    for (const auto& named_type : choice.choices)
    {
        if (!is_first)
            base += ", ";

        if (is_sequence(named_type.type) || is_set(named_type.type) || is_enumerated(named_type.type) ||
            is_choice(named_type.type))
        {
            throw std::runtime_error("ChoiceType must not be a structure or enum [" + name + "] [" +
                                     type_as_string(named_type.type, module, tree) + "]");
        }

        if (module.tagging_default == TaggingMode::automatic)
        {
            base += type_as_string(named_type.type, module, tree, "",
                                   Identifier(Class::context_specific, tag_counter++).name());
        }
        else
        {
            base += type_as_string(named_type.type, module, tree);
        }
        is_first = false;
    }
    base += ">, ";

    const auto id = identifier_override.empty() ? identifier(choice, module, tree).name() : identifier_override;

    base += id;
    base += ", " + to_string(choice.storage);
    base += ">";

    std::string res = "struct " + name + " : " + base + "{\n";

    // Define sub types
    for (const NamedType& named_type : choice.choices)
    {
        std::string component_type;
        if (is_set(named_type.type) || is_sequence(named_type.type) || is_choice(named_type.type))
        {
            const std::string id_template_param = "Identifier" + std::to_string(id_counter++);
            res += create_template_definition({id_template_param + " = ExplicitId<UniversalTag::sequence>"}) + '\n';
            res += type_as_string(named_type.type, module, tree, named_type.name + "_type");
        }
    }

    res += "    using AliasedType = " + base + ";\n";
    res += "    using AliasedType::AliasedType;\n";
#ifndef _MSC_VER
    res += "    using AliasedType::operator=;\n";
    res += "    " + name + "() noexcept : AliasedType(){}\n";
    res += "    " + name + "(const " + name + "& rhs) noexcept : AliasedType(static_cast<const AliasedType&>(rhs)){}\n";
    res +=
        "    " + name + "(" + name + "&& rhs) noexcept : AliasedType(static_cast<AliasedType&&>(std::move(rhs))){}\n";
    res += "    " + name + "& operator=(const " + name +
           "& rhs) noexcept { static_cast<AliasedType>(*this) = static_cast<const AliasedType&>(rhs); "
           "return *this; }\n";
    res += "    " + name + "& operator=(" + name +
           "&& rhs) noexcept { static_cast<AliasedType>(*this) = static_cast<AliasedType&&>(std::move(rhs)); "
           "return *this; }\n";
#endif
    res += "    " + create_template_definition({"Identifier = " + id}) + '\n';
    res += "    std::size_t encoded_length_with_id() const noexcept;\n";
    res += "    " + create_template_definition({"Identifier = " + id}) + '\n';
    res += "    EncodeResult encode_with_id(absl::Span<uint8_t> output) const noexcept;\n";
    res += "    " + create_template_definition({"Identifier = " + id}) + '\n';
    res += "    DecodeResult decode_with_id(BerView output) noexcept;\n";

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
    if (!identifier_override.empty())
    {
        NamedType resolved = resolve_type(tree, module.module_reference, defined_type);
        if (!is_sequence(resolved.type) && !is_set(resolved.type) && !is_enumerated(resolved.type) &&
            !is_choice(resolved.type))
            return type_as_string(resolved.type, module, tree, resolved.name, identifier_override);
    }

    const std::string& assigned_type =
        (defined_type.module_reference ? *defined_type.module_reference + "::" : std::string("")) +
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
