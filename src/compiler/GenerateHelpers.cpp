#include "fast_ber/compiler/GenerateHelpers.hpp"

template <typename CollectionType>
CodeBlock create_ostream_operators(const CollectionType& collection, const std::string& full_name)
{
    CodeBlock block;
    block.add_line("std::ostream& operator<<(std::ostream& os, const " + full_name + "& object)");
    {
        CodeScope scope1(block);
        {
            block.add_line("bool first = true;");
            block.add_line("os << '{';");
            if (collection.components.size() == 0)
            {
                block.add_line("(void)object;");
                block.add_line("(void)first;");
            }

            for (const ComponentType& component : collection.components)
            {
                block.add_line("if (!first) os << \", \";");
                block.add_line("os << \"\\\"" + component.named_type.name + "\\\" : \" << object." +
                               component.named_type.name + ";");
                block.add_line("first = false;");
            }
            block.add_line("os << '}';");
            block.add_line("return os;");
        }
    }
    block.add_line();
    return block;
}

template <typename CollectionType>
CodeBlock create_collection_equality_operators(const CollectionType& collection, std::vector<std::string> parents,
                                               const std::string& name)
{
    CodeBlock   block;
    std::string full_namespace;
    for (const std::string& parent : parents)
    {
        full_namespace += parent + "::";
    }
    std::string full_name = full_namespace + name;

    block.add_line("bool operator==(const " + full_name + "& lhs, const " + full_name + "& rhs) noexcept");
    {
        CodeScope scope1(block);

        if (collection.components.size() == 0)
        {
            block.add_line("(void)lhs;");
            block.add_line("(void)rhs;");
        }

        block.add_line("return true");
        for (const ComponentType& component : collection.components)
        {
            block.add_line(" && lhs." + component.named_type.name + " == rhs." + component.named_type.name);
        }
        block.add_line(";");
    }
    block.add_line();

    block.add_line("bool operator!=(const " + full_name + "& lhs, const " + full_name + "& rhs) noexcept");
    {
        CodeScope scope1(block);
        block.add_line("return !(lhs == rhs);");
    }
    block.add_line();
    block.add_block(create_ostream_operators(collection, full_name));
    parents.push_back(name);

    return block;
}

CodeBlock create_helper_functions_impl(const std::string& name, const Type& type)
{
    if (is_sequence(type))
    {
        const SequenceType& sequence = absl::get<SequenceType>(absl::get<BuiltinType>(type));
        return create_collection_equality_operators(sequence, {}, name);
    }
    else if (is_set(type))
    {
        const SetType& set = absl::get<SetType>(absl::get<BuiltinType>(type));
        return create_collection_equality_operators(set, {}, name);
    }
    else if (is_enumerated(type))
    {
        const EnumeratedType& e           = absl::get<EnumeratedType>(absl::get<BuiltinType>(type));
        const std::string     values_name = name + "Values";

        CodeBlock block;
        block.add_line("const char* to_string(" + values_name + " e) noexcept");
        {
            CodeScope scope(block);
            block.add_line("switch (e)");
            {
                CodeScope scope2(block);
                for (const EnumerationValue& enum_value : e.enum_values)
                {
                    block.add_line("    case " + values_name + "::" + enum_value.name + ": return \"" +
                                   enum_value.name + "\";");
                }
                block.add_line("default: return \"Invalid state!\";");
            }
        }

        block.add_line("std::ostream& operator<<(std::ostream& os, " + values_name + " object)");
        {
            CodeScope scope(block);
            block.add_line("os << to_string(object);");
            block.add_line("return os;");
        }
        return block;
    }
    else if (is_choice(type))
    {
        /*TODO std::string       res;
        const ChoiceType& choice = absl::get<ChoiceType>(absl::get<BuiltinType>(type));
        for (const NamedType& named_type : choice.choices)
        {
            res += create_helper_functions(name + make_type_name(named_type.name), named_type.type);
        }
        return res;*/
        return {};
    }

    return {};
}

template <typename Function>
CodeBlock visit_all_types(const std::string& context_namespace, const Type& type, Function&& func)
{
    CodeBlock block;

    if (is_sequence(type))
    {
        const SequenceType& sequence = absl::get<SequenceType>(absl::get<BuiltinType>(type));
        for (const ComponentType& component : sequence.components)
        {
            const std::string name = context_namespace + "::" + make_type_name(component.named_type.name);
            block.add_block(visit_all_types(name, component.named_type.type, std::forward<Function>(func)));
        }
    }
    else if (is_set(type))
    {
        const SetType& set = absl::get<SetType>(absl::get<BuiltinType>(type));
        for (const ComponentType& component : set.components)
        {
            const std::string name = context_namespace + "::" + make_type_name(component.named_type.name);
            block.add_block(visit_all_types(name, component.named_type.type, std::forward<Function>(func)));
        }
    }
    else if (is_sequence_of(type))
    {
        const SequenceOfType& sequence = absl::get<SequenceOfType>(absl::get<BuiltinType>(type));
        if (sequence.has_name)
        {
            const std::string name = context_namespace + "::" + make_type_name(sequence.named_type->name);
            block.add_block(visit_all_types(name, sequence.named_type->type, std::forward<Function>(func)));
        }
    }
    else if (is_set_of(type))
    {
        const SetOfType& set = absl::get<SetOfType>(absl::get<BuiltinType>(type));
        if (set.has_name)
        {
            const std::string name = context_namespace + "::" + make_type_name(set.named_type->name);
            block.add_block(visit_all_types(name, set.named_type->type, std::forward<Function>(func)));
        }
    }
    else if (is_choice(type))
    {
        const ChoiceType& choice = absl::get<ChoiceType>(absl::get<BuiltinType>(type));
        for (const NamedType& named_type : choice.choices)
        {
            const std::string name = context_namespace + "::" + make_type_name(named_type.name);
            block.add_block(visit_all_types(name, named_type.type, std::forward<Function>(func)));
        }
    }
    else if (is_prefixed(type))
    {
        const PrefixedType& prefixed = absl::get<PrefixedType>(absl::get<BuiltinType>(type));
        block.add_block(visit_all_types(context_namespace, prefixed.tagged_type->type, std::forward<Function>(func)));
    }

    block.add_block(func(context_namespace, type));
    return block;
}

std::string create_helper_functions(const Assignment& assignment)
{
    if (absl::holds_alternative<TypeAssignment>(assignment.specific))
    {
        const TypeAssignment& type_assignment = absl::get<TypeAssignment>(assignment.specific);
        return visit_all_types(assignment.name, type_assignment.type, create_helper_functions_impl).to_string();
    }
    return "";
}
