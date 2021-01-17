#include "fast_ber/compiler/GenerateHelpers.hpp"

#include "fast_ber/compiler/GenerateChoice.hpp"
#include "fast_ber/compiler/Visit.hpp"

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
CodeBlock create_collection_equality_operators(const CollectionType& collection, const std::string& name)
{
    CodeBlock   block;
    block.add_line("bool operator==(const " + name + "& lhs, const " + name + "& rhs) noexcept");
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

    block.add_line("bool operator!=(const " + name + "& lhs, const " + name + "& rhs) noexcept");
    {
        CodeScope scope1(block);
        block.add_line("return !(lhs == rhs);");
    }
    block.add_line();
    block.add_block(create_ostream_operators(collection, name));

    return block;
}

CodeBlock create_alias_helpers(const std::string& name)
{
    CodeBlock   block;
    block.add_line("bool operator==(const " + name + "& lhs, const " + name + "& rhs) noexcept");
    {
        CodeScope scope1(block);
        block.add_line("return lhs.get_base() == rhs.get_base();");
    }
    block.add_line();

    block.add_line("bool operator!=(const " + name + "& lhs, const " + name + "& rhs) noexcept");
    {
        CodeScope scope1(block);
        block.add_line("return !(lhs == rhs);");
    }
    block.add_line();

    block.add_line("std::ostream& operator<<(std::ostream& os, const " + name + "& object)");
    {
        CodeScope scope1(block);
        block.add_line("return os << object.get_base();");
    }
    block.add_line();

    return block;
}

CodeBlock create_helper_functions_impl(const Asn1Tree&, const Module&, const Type& type, const std::string& name)
{
    if (is_sequence(type))
    {
        const SequenceType& sequence = absl::get<SequenceType>(absl::get<BuiltinType>(type));
        return create_collection_equality_operators(sequence, name);
    }
    else if (is_set(type))
    {
        const SetType& set = absl::get<SetType>(absl::get<BuiltinType>(type));
        return create_collection_equality_operators(set, name);
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
        const ChoiceType& choice = absl::get<ChoiceType>(absl::get<BuiltinType>(type));
        return create_choice_helpers(choice, name);
    }

    return {};
}

std::string create_helper_functions(const Asn1Tree& tree, const Module& module, const Assignment& assignment)
{
     if (!absl::holds_alternative<TypeAssignment>(assignment.specific))
     {
         return {};
     }

        const TypeAssignment& type_assignment = absl::get<TypeAssignment>(assignment.specific);
    CodeBlock block;

    if (!is_generated(type_assignment.type))
    {
        // Top level types are introduced using FAST_BER_ALIAS, so helper functions must be define
     // block.add_block( create_alias_helpers(assignment.name));
    }

    block.add_block(visit_all_types(tree, module, assignment, create_helper_functions_impl));
return block.to_string();
}
