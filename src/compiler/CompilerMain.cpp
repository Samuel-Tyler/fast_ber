#include "autogen/asn_compiler.hpp"
#include "fast_ber/compiler/CompilerTypes.hpp"
#include "fast_ber/compiler/CppGeneration.hpp"
#include "fast_ber/compiler/Dependencies.hpp"
#include "fast_ber/compiler/Identifier.hpp"
#include "fast_ber/compiler/ObjectClass.hpp"
#include "fast_ber/compiler/Parameters.hpp"
#include "fast_ber/compiler/ReorderAssignments.hpp"
#include "fast_ber/compiler/ResolveType.hpp"
#include "fast_ber/compiler/TypeAsString.hpp"

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

std::string create_type_assignment(const std::string& name, const Type& type, const Module& module,
                                   const Asn1Tree& tree)
{
    if (is_set(type) || is_sequence(type))
    {
        return "struct " + name + type_as_string(type, module, tree);
    }
    else if (is_enumerated(type))
    {
        return "enum class " + name + type_as_string(type, module, tree);
    }
    else
    {
        return "using " + name + " = " + type_as_string(type, module, tree) + ";\n";
    }
}

std::string create_type_assignment(const Assignment& assignment, const Module& module, const Asn1Tree& tree)
{
    return create_type_assignment(assignment.name, absl::get<TypeAssignment>(assignment.specific).type, module, tree) +
           "\n";
}

std::string cpp_value(const HexStringValue& hex)
{
    std::string res = "\"";
    size_t      i   = 0;

    if (hex.value.length() % 2 == 1)
    {
        const std::string& byte = std::string(hex.value.begin(), hex.value.begin() + 1);
        res += "\\";
        res += std::to_string(std::stoi(byte, nullptr, 16));
        i++;
    }

    for (; i < hex.value.length(); i += 2)
    {
        const std::string& byte = std::string(hex.value.begin() + i, hex.value.begin() + i + 2);
        res += "\\";
        res += std::to_string(std::stoi(byte, nullptr, 16));
    }

    return res + "\"";
}

std::string create_assignment(const Asn1Tree& tree, const Module& module, const Assignment& assignment)
{
    try
    {
        if (absl::holds_alternative<ValueAssignment>(assignment.specific)) // Value assignment
        {
            const ValueAssignment& value_assign = absl::get<ValueAssignment>(assignment.specific);
            std::string result = type_as_string(value_assign.type, module, tree) + " " + assignment.name + " = ";

            const Type& assigned_to_type =
                (is_defined(value_assign.type))
                    ? type(resolve(tree, module.module_reference, absl::get<DefinedType>(value_assign.type)))
                    : value_assign.type;
            if (is_oid(assigned_to_type))
            {
                result += "ObjectIdentifier<>{";
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
                    std::cerr << "Warning: Not an object identifier : " + assignment.name + std::string(" : ") +
                                     e.what()
                              << std::endl;
                    return "";
                }
                result += "}";
            }
            else if (is_bit_string(assigned_to_type))
            {
                if (absl::holds_alternative<BitStringValue>(value_assign.value.value_selection))
                {
                    const BitStringValue& bstring = absl::get<BitStringValue>(value_assign.value.value_selection);
                    (void)bstring; // TODO: convert bstring to cstring
                    result += "\"\"";
                }
                else
                {
                    result += "\"\"";
                }
            }
            else if (absl::holds_alternative<std::string>(value_assign.value.value_selection))
            {
                const std::string& string = absl::get<std::string>(value_assign.value.value_selection);
                result += string;
            }
            else if (absl::holds_alternative<HexStringValue>(value_assign.value.value_selection))
            {
                const HexStringValue& hstring = absl::get<HexStringValue>(value_assign.value.value_selection);
                result += cpp_value(hstring);
            }
            else if (absl::holds_alternative<CharStringValue>(value_assign.value.value_selection))
            {
                const CharStringValue& cstring = absl::get<CharStringValue>(value_assign.value.value_selection);
                result += cstring.value;
            }
            else if (absl::holds_alternative<int64_t>(value_assign.value.value_selection))
            {
                const int64_t integer = absl::get<int64_t>(value_assign.value.value_selection);
                result += std::to_string(integer);
            }
            else if (absl::holds_alternative<DefinedValue>(value_assign.value.value_selection))
            {
                const DefinedValue& defined = absl::get<DefinedValue>(value_assign.value.value_selection);
                result += defined.reference;
            }
            else
            {
                throw std::runtime_error("Strange value assign");
            }

            result += ";\n";
            return result;
        }
        else if (absl::holds_alternative<TypeAssignment>(assignment.specific))
        {
            return create_type_assignment(assignment, module, tree);
        }
        else if (absl::holds_alternative<ObjectClassAssignment>(assignment.specific))
        {
            throw("Compiler Error: ObjectClassAssignment should be resolved: " + assignment.name);
        }
        else
        {
            return "";
        }
    }
    catch (const std::runtime_error& e)
    {
        throw(std::runtime_error("failed: " + assignment.name + e.what()));
    }
}

std::string create_identifier_functions(const Assignment& assignment, const std::string& module_name)
{
    std::string res;

    if (absl::holds_alternative<TypeAssignment>(assignment.specific))
    {
        const TypeAssignment& type_assign = absl::get<TypeAssignment>(assignment.specific);

        if (is_sequence(type_assign.type))
        {
            const std::string tags_class = assignment.name + "Tags";

            res += "constexpr inline ExplicitIdentifier<UniversalTag::sequence> identifier(const fast_ber::" +
                   module_name + "::" + assignment.name + "*) noexcept";
            res += "\n{\n";
            res += "    return {};\n";
            res += "}\n\n";
            return res;
        }
        else if (is_set(type_assign.type))
        {
            const std::string tags_class = assignment.name + "Tags";

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
std::string
create_collection_encode_functions(const std::string assignment_name, const std::vector<Parameter>& parameters,
                                   const CollectionType& collection, const Module& module, const Asn1Tree tree)
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
            res += identifier(component.named_type.type, module, tree).tag + "{}";
        }
        else if (module.tagging_default == TaggingMode::automatic)
        {
            res += "ImplicitIdentifier<Class::context_specific, " + std::to_string(tag_counter++) + ">{}";
        }
        else
        {
            res += identifier(component.named_type.type, module, tree).tag + "{}";
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
                                                      parameters, sequence, module, tree);
        }
        else if (is_set(component.named_type.type))
        {
            const SetType& set = absl::get<SetType>(absl::get<BuiltinType>(component.named_type.type));

            res += create_collection_encode_functions(assignment_name + "::" + component.named_type.name + "_type",
                                                      parameters, set, module, tree);
        }
    }

    std::vector<std::string> template_args = {"ID = ExplicitIdentifier<UniversalTag::" + collection_name(collection) +
                                              ">"};
    res += create_template_definition(template_args);
    res += "inline EncodeResult encode(absl::Span<uint8_t> output, const " + module.module_reference +
           "::" + assignment_name + "& input, const ID& id = ID{}) noexcept\n{\n";

    if (collection.components.size() == 0)
    {
        res += "    (void)input;\n";
    }

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
std::string create_collection_decode_functions(const std::string             assignment_name,
                                               const std::vector<Parameter>& parameters,
                                               const CollectionType& collection, const Module& module)
{
    std::string res;
    std::string tags_class = module.module_reference + "_" + assignment_name + "Tags";
    std::replace(tags_class.begin(), tags_class.end(), ':', '_');

    std::vector<std::string> template_args = {"ID = ExplicitIdentifier<UniversalTag::" + collection_name(collection) +
                                              ">"};

    res += create_template_definition(template_args);
    res += "inline DecodeResult decode(const BerView& input, " + module.module_reference + "::" + assignment_name +
           "& output, const ID& id = ID{}) noexcept\n{\n";

    if (collection.components.size() == 0)
    {
        res += "    (void)output;\n";
    }

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
           "& output, const ID& id = ID{}) noexcept\n{\n";
    res += "    DecodeResult result = decode(*input, output, id);\n";
    res += "    ++input;\n";
    res += "    return result;\n";
    res += "}\n\n";
    return res;
}

std::string create_encode_functions(const Assignment& assignment, const Module& module, const Asn1Tree& tree)
{
    if (absl::holds_alternative<TypeAssignment>(assignment.specific))
    {
        const TypeAssignment& type_assignment = absl::get<TypeAssignment>(assignment.specific);

        if (is_sequence(type_assignment.type))
        {
            const SequenceType& sequence = absl::get<SequenceType>(absl::get<BuiltinType>(type_assignment.type));
            return create_collection_encode_functions(assignment.name, assignment.parameters, sequence, module, tree);
        }
        else if (is_set(type_assignment.type))
        {
            std::string    res;
            const SetType& set = absl::get<SetType>(absl::get<BuiltinType>(type_assignment.type));
            return create_collection_encode_functions(assignment.name, assignment.parameters, set, module, tree);
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
std::string create_collection_equality_operators(const CollectionType& collection, const std::string& name)
{
    const std::string tags_class = name + "Tags";

    std::string res;

    res += "bool operator==(";
    res += "const " + name + "& lhs, ";
    res += "const " + name + "& rhs)\n";
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
    res += "const " + name + "& lhs, ";
    res += "const " + name + "& rhs)\n";
    res += "{\n";
    res += "    return !(lhs == rhs);\n}\n\n";

    std::string child_equality;
    // Create assignments for any children too
    for (const ComponentType& component : collection.components)
    {
        if (is_sequence(component.named_type.type))
        {
            const SequenceType& sequence = absl::get<SequenceType>(absl::get<BuiltinType>(component.named_type.type));
            child_equality +=
                create_collection_equality_operators(sequence, name + "::" + component.named_type.name + "_type");
        }
        else if (is_set(component.named_type.type))
        {
            const SetType& set = absl::get<SetType>(absl::get<BuiltinType>(component.named_type.type));
            child_equality +=
                create_collection_equality_operators(set, name + "::" + component.named_type.name + "_type");
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
            return create_collection_equality_operators(sequence, assignment.name);
        }
        else if (is_set(type_assignment.type))
        {
            const SetType& set = absl::get<SetType>(absl::get<BuiltinType>(type_assignment.type));
            return create_collection_equality_operators(set, assignment.name);
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

std::string create_body(const Asn1Tree& tree, const Module& module)
{
    std::string output;
    output += "\n";

    for (const Import& import : module.imports)
    {
        for (const auto& import_name : import.imports)
        {
            if (is_type(resolve(tree, import.module_reference, import_name)) ||
                is_value(resolve(tree, import.module_reference, import_name)))
            {
                output += "using " + import_name + " = " + import.module_reference + "::" + import_name + ";\n";
            }
        }
        if (import.imports.size() > 0)
        {
            output += "\n";
        }
    }

    for (const Assignment& assignment : module.assignments)
    {
        output += create_assignment(tree, module, assignment);
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
            body += create_encode_functions(assignment, module, tree);
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
        definitions += add_namespace(module.module_reference, create_body(tree, module));
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

        resolve_parameters(context.asn1_tree);
        context.asn1_tree.modules = reorder_modules(context.asn1_tree.modules);

        for (auto& module : context.asn1_tree.modules)
        {
            module.assignments = split_definitions(module.assignments);
            check_duplicated_names(module.assignments, module.module_reference);
            module.assignments = reorder_assignments(module.assignments, module.imports);
            module.assignments = split_nested_structures(module.assignments);
        }

        resolve_components_of(context.asn1_tree);
        resolve_object_classes(context.asn1_tree);

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
