#include "fast_ber/compiler/GenerateHelpers.hpp"

template <typename CollectionType>
std::string create_ostream_operators(const CollectionType& collection, const std::string& full_name)
{
    std::string res;
    res += "std::ostream& operator<<(std::ostream& os, const " + full_name + "& object)\n";
    res += "{\n";
    res += "    bool first = true;\n";
    res += "    os << '{';\n";
    if (collection.components.size() == 0)
    {
        res += "    (void)object;\n";
        res += "    (void)first;\n";
    }

    for (const ComponentType& component : collection.components)
    {
        res += "    if (!first) os << \", \";\n";
        res +=
            "    os << \"\\\"" + component.named_type.name + "\\\" : \" << object." + component.named_type.name + ";\n";
        res += "    first = false;\n";
    }
    res += "    os << '}';\n";
    res += "    return os;\n";
    res += "}\n\n";
    return res;
}

template <typename CollectionType>
std::string create_collection_equality_operators(const CollectionType& collection, std::vector<std::string> parents,
                                                 const std::string& name)
{
    std::string full_namespace;
    for (const std::string& parent : parents)
    {
        full_namespace += parent + "::";
    }

    std::string full_name = full_namespace + name;

    std::string res;

    res += "bool operator==(";
    res += "const " + full_name + "& lhs, ";
    res += "const " + full_name + "& rhs)\n";
    res += "{\n";

    if (collection.components.size() == 0)
    {
        res += "    (void)lhs;\n";
        res += "    (void)rhs;\n";
    }

    res += "    return true";
    for (const ComponentType& component : collection.components)
    {
        res += " &&\n      lhs." + component.named_type.name + " == ";
        res += "rhs." + component.named_type.name;
    }
    res += ";\n}\n\n";

    res += "bool operator!=(";
    res += "const " + full_name + "& lhs, ";
    res += "const " + full_name + "& rhs)\n";
    res += "{\n";
    res += "    return !(lhs == rhs);\n}\n\n";

    res += create_ostream_operators(collection, full_name);

    parents.push_back(name);

    return res;
}

std::string create_helper_functions(const std::string& name, const Type& type)
{
    if (is_sequence(type))
    {
        std::string         res;
        const SequenceType& sequence = absl::get<SequenceType>(absl::get<BuiltinType>(type));
        for (const ComponentType& component : sequence.components)
        {
            res += create_helper_functions(name + "::" + make_type_name(component.named_type.name),
                                           component.named_type.type);
        }
        res += create_collection_equality_operators(sequence, {}, name);
        return res;
    }
    else if (is_set(type))
    {
        std::string    res;
        const SetType& set = absl::get<SetType>(absl::get<BuiltinType>(type));
        for (const ComponentType& component : set.components)
        {
            res += create_helper_functions(name + "::" + make_type_name(component.named_type.name),
                                           component.named_type.type);
        }

        res += create_collection_equality_operators(set, {}, name);
        return res;
    }
    else if (is_enumerated(type))
    {
        const EnumeratedType& e           = absl::get<EnumeratedType>(absl::get<BuiltinType>(type));
        const std::string     values_name = name + "Values";

        std::string res;
        res += "const char* to_string(" + values_name + " e)\n";
        res += "{\n";
        res += "    switch (e)\n";
        res += "    {\n";
        for (const EnumerationValue& enum_value : e.enum_values)
        {
            res += "    case " + values_name + "::" + enum_value.name + ": return \"" + enum_value.name + "\";\n";
        }

        res += "    default: return \"Invalid state!\";\n";
        res += "    }\n";
        res += "}\n\n";

        res += "std::ostream& operator<<(std::ostream& os, " + values_name + " object)\n";
        res += "{\n";
        res += "    os << to_string(object);\n";
        res += "    return os;\n";
        res += "}\n\n";
        return res;
    }
    else if (is_choice(type))
    {
        std::string       res;
        const ChoiceType& choice = absl::get<ChoiceType>(absl::get<BuiltinType>(type));
        for (const NamedType& named_type : choice.choices)
        {
            res += create_helper_functions(name + make_type_name(named_type.name), named_type.type);
        }
        return res;
    }
    else if (is_prefixed(type))
    {
        const PrefixedType& prefixed = absl::get<PrefixedType>(absl::get<BuiltinType>(type));
        return create_helper_functions(name, prefixed.tagged_type->type);
    }

    return "";
}

std::string create_helper_functions(const Assignment& assignment)
{

    if (absl::holds_alternative<TypeAssignment>(assignment.specific))
    {
        const TypeAssignment& type_assignment = absl::get<TypeAssignment>(assignment.specific);
        return create_helper_functions(assignment.name, type_assignment.type);
    }
    return "";
}
