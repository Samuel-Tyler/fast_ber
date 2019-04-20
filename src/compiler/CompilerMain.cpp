#include "autogen/asn_compiler.hpp"
#include "fast_ber/compiler/CompilerTypes.hpp"
#include "fast_ber/compiler/CppGeneration.hpp"
#include "fast_ber/compiler/Dependencies.hpp"
#include "fast_ber/compiler/Identifier.hpp"
#include "fast_ber/compiler/ReorderAssignments.hpp"

#include <string>
#include <unordered_map>

std::string strip_path(const std::string& path)
{
    std::size_t found = path.find_last_of("/\\");
    if (found == std::string::npos)
    {
        return path;
    }
    return path.substr(found + 1);
}

std::string create_type_assignment(const std::string& name, const Type& type, const TaggingMode& tagging_mode)
{
    if (is_set(type) || is_sequence(type))
    {
        return "struct " + name + to_string(type);
    }
    else if (is_enumerated(type))
    {
        return "enum class " + name + to_string(type);
    }
    else
    {
        return "using " + name + " = " + fully_tagged_type(type, tagging_mode) + ";\n";
    }
}

std::string create_type_assignment(const Assignment& assignment, const TaggingMode& tagging_mode)
{
    const std::string& template_definition = create_template_definition(assignment.parameters);
    return template_definition +
           create_type_assignment(assignment.name, absl::get<TypeAssignment>(assignment.specific).type, tagging_mode) +
           "\n";
}

std::string create_assignment(const Assignment& assignment, TaggingMode tagging_mode)
{
    if (absl::holds_alternative<ValueAssignment>(assignment.specific)) // Value assignment
    {
        const ValueAssignment& value_assign = absl::get<ValueAssignment>(assignment.specific);
        std::string result = fully_tagged_type(value_assign.type, tagging_mode) + " " + assignment.name + " = ";

        if (is_oid(value_assign.type) || (is_defined(value_assign.type) && absl::holds_alternative<std::vector<Value>>(
                                                                               value_assign.value.value_selection)))
        {
            result += "ObjectIdentifier{";
            try
            {
                const ObjectIdComponents& object_id = ObjectIdComponents(value_assign.value);

                for (size_t i = 0; i < object_id.components.size(); i++)
                {
                    if (object_id.components[i].value)
                    {
                        result += std::to_string(*object_id.components[i].value);
                    }
                    else
                    {
                        result += std::to_string(0);
                    }

                    if (i < object_id.components.size() - 1)
                    {
                        result += ", ";
                    }
                }
            }
            catch (const std::runtime_error& e)
            {
                std::cerr << "Warning: Not an object identifier : " + assignment.name + std::string(" : ") + e.what()
                          << std::endl;
                return "";
            }
            result += "}";
        }
        else if (absl::holds_alternative<std::string>(value_assign.value.value_selection))
        {
            const std::string& string = absl::get<std::string>(value_assign.value.value_selection);
            result += string;
        }
        else if (absl::holds_alternative<int64_t>(value_assign.value.value_selection))
        {
            const int64_t integer = absl::get<int64_t>(value_assign.value.value_selection);
            result += std::to_string(integer);
        }
        result += ";\n";
        return result;
    }
    else if (absl::holds_alternative<TypeAssignment>(assignment.specific))
    {
        return create_type_assignment(assignment, tagging_mode);
    }
    else if (absl::holds_alternative<ObjectClassAssignment>(assignment.specific))
    {
        std::cerr << "Warning: Parsed but ignoring class assignment: " << assignment.name << std::endl;
        return "";
    }
    else
    {
        return "";
    }
}

std::string create_identifier_functions(const Assignment& assignment, const std::string& module_name)
{
    std::string full_assignment_name = assignment.name + create_template_arguments(assignment.parameters);
    std::string res                  = create_template_definition(assignment.parameters);

    if (absl::holds_alternative<TypeAssignment>(assignment.specific))
    {
        const TypeAssignment& type_assign = absl::get<TypeAssignment>(assignment.specific);

        if (is_sequence(type_assign.type))
        {
            const std::string tags_class = assignment.name + "Tags";

            res += "constexpr inline ExplicitIdentifier<UniversalTag::sequence> identifier(const fast_ber::" +
                   module_name + "::" + full_assignment_name + "*) noexcept";
            res += "\n{\n";
            res += "    return {};\n";
            res += "}\n\n";
            return res;
        }
        else if (is_set(type_assign.type))
        {
            const std::string tags_class = full_assignment_name + "Tags";

            res += "constexpr inline ExplicitIdentifier<UniversalTag::set> identifier(const fast_ber::" + module_name +
                   "::" + assignment.name + "*) noexcept";
            res += "\n{\n";
            res += "    return {};\n";
            res += "}\n\n";
            return res;
        }
    }
    return "";
}

std::string collection_name(const SequenceType&) { return "sequence"; }
std::string collection_name(const SetType&) { return "set"; }

template <typename CollectionType>
std::string create_collection_encode_functions(const std::string            assignment_name,
                                               const std::set<std::string>& parameters,
                                               const CollectionType& collection, const Module& module)
{
    std::string res;
    std::string tags_class = module.module_reference + "_" + assignment_name + "Tags";
    std::replace(tags_class.begin(), tags_class.end(), ':', '_');

    res += "namespace " + tags_class + " {\n";
    int tag_counter = 0;
    for (const ComponentType& component : collection.components)
    {
        res += "static constexpr auto " + component.named_type.name + " = ";
        if (is_prefixed(component.named_type.type))
        {
            res += identifier(component.named_type.type, module.tagging_default).tag + "{}";
        }
        else if (module.tagging_default == TaggingMode::automatic)
        {
            res += "ImplicitIdentifier<Class::context_specific, " + std::to_string(tag_counter++) + ">{}";
        }
        else if (is_defined(component.named_type.type))
        {
            const DefinedType& defined = absl::get<DefinedType>(component.named_type.type);
            res += "identifier(static_cast<" + module.module_reference + "::" + defined.name + "*>(nullptr))";
        }
        else
        {
            res += identifier(component.named_type.type, module.tagging_default).tag + "{}";
        }
        res += ";\n";
    }
    res += "}\n\n";

    // Make child encode functions
    for (const ComponentType& component : collection.components)
    {
        if (is_sequence(component.named_type.type))
        {
            const SequenceType& sequence = absl::get<SequenceType>(absl::get<BuiltinType>(component.named_type.type));

            res += create_collection_encode_functions(assignment_name + "::" + component.named_type.name + "_type",
                                                      parameters, sequence, module);
        }
        else if (is_set(component.named_type.type))
        {
            const SetType& set = absl::get<SetType>(absl::get<BuiltinType>(component.named_type.type));

            res += create_collection_encode_functions(assignment_name + "::" + component.named_type.name + "_type",
                                                      parameters, set, module);
        }
    }

    std::set<std::string> template_args = parameters;
    template_args.insert("ID = ExplicitIdentifier<UniversalTag::" + collection_name(collection) + ">");

    res += create_template_definition(template_args);
    res += "inline EncodeResult encode(absl::Span<uint8_t> output, const " + module.module_reference +
           "::" + assignment_name + create_template_arguments(parameters) +
           "& input, const ID& id = ID{}) noexcept\n{\n";
    res += "    return encode_sequence_combine(output, id";
    for (const ComponentType& component : collection.components)
    {
        res += ",\n                          input." + component.named_type.name + ", " + tags_class +
               "::" + component.named_type.name;
    }
    res += ");\n}\n\n";
    return res;
}

template <typename CollectionType>
std::string create_collection_decode_functions(const std::string            assignment_name,
                                               const std::set<std::string>& parameters,
                                               const CollectionType& collection, const Module& module)
{
    std::string res;
    std::string tags_class = module.module_reference + "_" + assignment_name + "Tags";
    std::replace(tags_class.begin(), tags_class.end(), ':', '_');

    std::set<std::string> template_args = parameters;
    template_args.insert("ID = ExplicitIdentifier<UniversalTag::" + collection_name(collection) + ">");

    res += create_template_definition(template_args);
    res += "inline DecodeResult decode(const BerView& input, " + module.module_reference + "::" + assignment_name +
           create_template_arguments(parameters) + "& output, const ID& id = ID{}) noexcept\n{\n";
    res += "    return decode_" + collection_name(collection) + "_combine(input, \"" + module.module_reference +
           "::" + assignment_name + "\", id";
    for (const ComponentType& component : collection.components)
    {
        res += ",\n                          output." + component.named_type.name + ", " + tags_class +
               "::" + component.named_type.name;
    }
    res += ");\n}\n\n";

    // Make child decode functions
    for (const ComponentType& component : collection.components)
    {
        if (is_sequence(component.named_type.type))
        {
            const SequenceType& sequence = absl::get<SequenceType>(absl::get<BuiltinType>(component.named_type.type));

            res += create_collection_decode_functions(assignment_name + "::" + component.named_type.name + "_type",
                                                      parameters, sequence, module);
        }
        else if (is_set(component.named_type.type))
        {
            const SetType& set = absl::get<SetType>(absl::get<BuiltinType>(component.named_type.type));

            res += create_collection_decode_functions(assignment_name + "::" + component.named_type.name + "_type",
                                                      parameters, set, module);
        }
    }

    res += create_template_definition(template_args);
    res += "inline DecodeResult decode(BerViewIterator& input, " + module.module_reference + "::" + assignment_name +
           create_template_arguments(parameters) + "& output, const ID& id = ID{}) noexcept\n{\n";
    res += "    DecodeResult result = decode(*input, output, id);\n";
    res += "    ++input;\n";
    res += "    return result;\n";
    res += "}\n\n";
    return res;
}

std::string create_encode_functions(const Assignment& assignment, const Module& module)
{
    if (absl::holds_alternative<TypeAssignment>(assignment.specific))
    {
        const TypeAssignment& type_assignment = absl::get<TypeAssignment>(assignment.specific);

        if (is_sequence(type_assignment.type))
        {
            const SequenceType& sequence = absl::get<SequenceType>(absl::get<BuiltinType>(type_assignment.type));
            return create_collection_encode_functions(assignment.name, assignment.parameters, sequence, module);
        }
        else if (is_set(type_assignment.type))
        {
            std::string    res;
            const SetType& set = absl::get<SetType>(absl::get<BuiltinType>(type_assignment.type));
            return create_collection_encode_functions(assignment.name, assignment.parameters, set, module);
        }
    }

    return "";
}

std::string create_decode_functions(const Assignment& assignment, const Module& module)
{
    if (absl::holds_alternative<TypeAssignment>(assignment.specific))
    {
        const TypeAssignment& type_assignment = absl::get<TypeAssignment>(assignment.specific);

        if (is_sequence(type_assignment.type))
        {
            const SequenceType& sequence = absl::get<SequenceType>(absl::get<BuiltinType>(type_assignment.type));
            return create_collection_decode_functions(assignment.name, assignment.parameters, sequence, module);
        }
        else if (is_set(type_assignment.type))
        {
            const SetType& set = absl::get<SetType>(absl::get<BuiltinType>(type_assignment.type));
            return create_collection_decode_functions(assignment.name, assignment.parameters, set, module);
        }
    }
    return "";
}

template <typename CollectionType>
std::string create_collection_equality_operators(const CollectionType& collection, const std::string& name,
                                                 const std::set<std::string>& parameters)
{
    const std::string tags_class = name + "Tags";

    const std::string parameterized_name = name + create_template_arguments(parameters);
    std::string       res;
    res += create_template_definition(parameters);
    res += "bool operator==(";
    res += "const " + parameterized_name + "& lhs, ";
    res += "const " + parameterized_name + "& rhs)\n";
    res += "{\n";
    res += "    return true";
    for (const ComponentType& component : collection.components)
    {
        res += " &&\n      lhs." + component.named_type.name + " == ";
        res += "rhs." + component.named_type.name;
    }
    res += ";\n}\n\n";

    res += create_template_definition(parameters);
    res += "bool operator!=(";
    res += "const " + parameterized_name + "& lhs, ";
    res += "const " + parameterized_name + "& rhs)\n";
    res += "{\n";
    res += "    return !(lhs == rhs);\n}\n\n";

    std::string child_equality;
    // Create assignments for any children too
    for (const ComponentType& component : collection.components)
    {
        if (is_sequence(component.named_type.type))
        {
            const SequenceType& sequence = absl::get<SequenceType>(absl::get<BuiltinType>(component.named_type.type));
            child_equality += create_collection_equality_operators(
                sequence, name + "::" + component.named_type.name + "_type", parameters);
        }
        else if (is_set(component.named_type.type))
        {
            const SetType& set = absl::get<SetType>(absl::get<BuiltinType>(component.named_type.type));
            child_equality += create_collection_equality_operators(
                set, name + "::" + component.named_type.name + "_type", parameters);
        }
    }
    return child_equality + res;
}

std::string create_helper_functions(const Assignment& assignment)
{
    if (absl::holds_alternative<TypeAssignment>(assignment.specific))
    {
        const TypeAssignment& type_assignment = absl::get<TypeAssignment>(assignment.specific);

        if (is_sequence(type_assignment.type))
        {
            const SequenceType& sequence = absl::get<SequenceType>(absl::get<BuiltinType>(type_assignment.type));
            return create_collection_equality_operators(sequence, assignment.name, assignment.parameters);
        }
        else if (is_set(type_assignment.type))
        {
            const SetType& set = absl::get<SetType>(absl::get<BuiltinType>(type_assignment.type));
            return create_collection_equality_operators(set, assignment.name, assignment.parameters);
        }
        else if (is_enumerated(type_assignment.type))
        {
            const EnumeratedType& e = absl::get<EnumeratedType>(absl::get<BuiltinType>(type_assignment.type));

            std::string res = "const char* to_string(const " + assignment.name + "& e)\n";
            res += "{\n";
            res += "    switch (e)\n";
            res += "    {\n";
            for (const EnumerationValue& enum_value : e.enum_values)
            {
                res +=
                    "    case " + assignment.name + "::" + enum_value.name + ": return \"" + enum_value.name + "\";\n";
            }
            res += "    default: return \"Invalid state!\";\n";
            res += "    }\n";
            res += "}\n";
            return res;
        }
    }

    return "";
}

std::string create_body(const Module& module)
{
    std::string output;
    output += "\n";

    for (const Import& import : module.imports)
    {
        for (const auto& import_name : import.imports)
        {
            output += "using " + import_name + " = " + import.module_reference + "::" + import_name + ";\n";
        }
        if (import.imports.size() > 0)
        {
            output += "\n";
        }
    }

    for (const Assignment& assignment : module.assignments)
    {
        output += create_assignment(assignment, module.tagging_default);
    }

    return output;
}

std::string create_detail_body(const Asn1Tree& tree)
{
    std::string output;
    output += "/* Functionality provided for Encoding and Decoding BER */\n\n";

    std::string body;

    for (const Module& module : tree.modules)
    {
        // Inside namespace due to argument dependant lookup rules
        std::string ids;
        for (const Assignment& assignment : module.assignments)
        {
            ids += create_identifier_functions(assignment, module.module_reference);
        }
        body += add_namespace(module.module_reference, ids);

        std::string helpers;
        for (const Assignment& assignment : module.assignments)
        {
            body += create_encode_functions(assignment, module);
            body += create_decode_functions(assignment, module) + "\n";
            helpers += create_helper_functions(assignment);
        }

        body += add_namespace(module.module_reference, helpers);
    }
    output += add_namespace("fast_ber", body) + "\n\n";
    return output;
}

std::string create_output_file(const Asn1Tree& tree, const std::string detail_filename)
{
    std::string output;
    output += "#pragma once\n\n";
    output += create_include("fast_ber/ber_types/All.hpp");
    output += "\n";

    std::string definitions;
    for (const auto& module : tree.modules)
    {
        definitions += add_namespace(module.module_reference, create_body(module));
    }

    output += add_namespace("fast_ber", definitions) + '\n';
    output += create_include(strip_path(detail_filename)) + '\n';
    return output;
}

int main(int argc, char** argv)
{
    if (argc < 3)
    {
        std::cout << "Usage: INPUT.asn... OUTPUT_NAME\n";
        return -1;
    }

    try
    {
        Context context;
        for (int i = 1; i < argc - 1; i++)
        {
            std::string   input_filename = argv[i];
            std::ifstream input_file(input_filename);
            if (!input_file.good())
            {
                std::cerr << "Could not open input file: " << input_filename << "\n";
                return -1;
            }

            std::string buffer(std::istreambuf_iterator<char>(input_file), {});
            context.cursor = buffer.c_str();
            context.location.initialize(&input_filename);

            yy::asn1_parser parser(context);

            const auto res = parser.parse();
            if (res)
            {
                return -1;
            }
        }

        const std::string& output_filename = std::string(argv[argc - 1]) + ".hpp";
        const std::string& detail_filame   = std::string(argv[argc - 1]) + ".detail.hpp";

        std::ofstream output_file(output_filename);
        std::ofstream detail_output_file(detail_filame);
        if (!output_file.good())
        {
            std::cerr << "Could not create output file: " + output_filename + "\n";
            return -1;
        }
        if (!detail_output_file.good())
        {
            std::cerr << "Could not create output file: " + detail_filame + "\n";
            return -1;
        }

        context.asn1_tree.modules = reorder_modules(context.asn1_tree.modules);

        for (auto& module : context.asn1_tree.modules)
        {
            module.assignments = split_definitions(module.assignments);
            check_duplicated_names(module.assignments, module.module_reference);
            module.assignments = reorder_assignments(module.assignments, module.imports);
            module.assignments = split_nested_structures(module.assignments);
        }

        output_file << create_output_file(context.asn1_tree, detail_filame);
        detail_output_file << create_detail_body(context.asn1_tree);

        return 0;
    }
    catch (const std::exception& e)
    {
        std::cerr << "Compilation error: " << e.what() << "\n";
        return -1;
    }
}
