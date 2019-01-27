#include "autogen/asn_compiler.hpp"
#include "fast_ber/compiler/CompilerTypes.hpp"
#include "fast_ber/compiler/Dependencies.hpp"
#include "fast_ber/compiler/ReorderAssignments.hpp"

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

std::string create_assignment(const Assignment& assignment, TaggingMode tagging_mode)
{
    if (absl::holds_alternative<BuiltinType>(assignment.type) &&
        absl::holds_alternative<SequenceType>(absl::get<BuiltinType>(assignment.type)))
    {
        const SequenceType& sequence = absl::get<SequenceType>(absl::get<BuiltinType>(assignment.type));
        return "struct " + assignment.name + to_string(sequence);
    }
    else if (absl::holds_alternative<BuiltinType>(assignment.type) &&
             absl::holds_alternative<SetType>(absl::get<BuiltinType>(assignment.type)))
    {
        const SetType& set = absl::get<SetType>(absl::get<BuiltinType>(assignment.type));
        return "struct " + assignment.name + to_string(set);
    }
    else if (absl::holds_alternative<BuiltinType>(assignment.type) &&
             absl::holds_alternative<EnumeratedType>(absl::get<BuiltinType>(assignment.type)))
    {
        const EnumeratedType& enumerated = absl::get<EnumeratedType>(absl::get<BuiltinType>(assignment.type));
        return "enum class " + assignment.name + to_string(enumerated);
    }
    else
    {
        return "using " + assignment.name + " = " + fully_tagged_type(assignment.type, tagging_mode) + ";\n\n";
    }
}

std::string create_encode_functions(const Assignment& assignment, const std::string& module_name,
                                    TaggingMode tagging_mode)
{
    if (absl::holds_alternative<BuiltinType>(assignment.type) &&
        absl::holds_alternative<SequenceType>(absl::get<BuiltinType>(assignment.type)))
    {
        std::string         res;
        const SequenceType& sequence   = absl::get<SequenceType>(absl::get<BuiltinType>(assignment.type));
        const std::string   tags_class = assignment.name + "Tags";

        res += "namespace " + module_name + " {\n";
        res += "inline ExplicitIdentifier<UniversalTag::sequence> identifier(const " + assignment.name + "&) noexcept";
        res += "\n    {\n";
        res += "        return {};\n";
        res += "    }\n}\n\n";

        res += "namespace " + tags_class + " {\n";
        int tag_counter = 0;
        for (const ComponentType& component : sequence.components)
        {
            res += "static const auto " + component.named_type.name + " = ";
            if (absl::holds_alternative<BuiltinType>(component.named_type.type) &&
                (absl::holds_alternative<PrefixedType>(absl::get<BuiltinType>(component.named_type.type))))
            {
                res += universal_tag(component.named_type.type, tagging_mode).tag;
            }
            else
            {
                res += "ImplicitIdentifier<Class::context_specific, " + std::to_string(tag_counter++) + ">";
            }
            res += "{};\n";
        }
        res += "}\n\n";

        res += "template <typename ID = ExplicitIdentifier<UniversalTag::sequence>>\n";
        res += "inline EncodeResult encode(absl::Span<uint8_t> output, const " + module_name + "::" + assignment.name +
               "& input, const ID& id = ID{}) noexcept\n{\n";
        res += "    return encode_sequence_combine(output, id";
        for (const ComponentType& component : sequence.components)
        {
            res += ",\n                          input." + component.named_type.name + ", " + tags_class +
                   "::" + component.named_type.name;
        }
        res += ");\n}\n\n";
        return res;
    }
    else if (absl::holds_alternative<BuiltinType>(assignment.type) &&
             absl::holds_alternative<SetType>(absl::get<BuiltinType>(assignment.type)))
    {
        std::string       res;
        const SetType&    set        = absl::get<SetType>(absl::get<BuiltinType>(assignment.type));
        const std::string tags_class = assignment.name + "Tags";

        res += "namespace " + module_name + " {\n";
        res += "inline ExplicitIdentifier<UniversalTag::sequence> identifier(const " + assignment.name + "&) noexcept";
        res += "\n    {\n";
        res += "        return {};\n";
        res += "    }\n}\n\n";

        res += "namespace " + tags_class + " {\n";
        int tag_counter = 0;
        for (const ComponentType& component : set.components)
        {
            res += "static const auto " + component.named_type.name + " = ";
            if (absl::holds_alternative<BuiltinType>(component.named_type.type) &&
                (absl::holds_alternative<PrefixedType>(absl::get<BuiltinType>(component.named_type.type))))
            {
                res += universal_tag(component.named_type.type, tagging_mode).tag;
            }
            else
            {
                res += "ImplicitIdentifier<Class::context_specific, " + std::to_string(tag_counter++) + ">";
            }
            res += "{};\n";
        }
        res += "}\n\n";

        res += "template <typename ID = ExplicitIdentifier<UniversalTag::set>>\n";
        res += "inline EncodeResult encode(absl::Span<uint8_t> output, const " + module_name + "::" + assignment.name +
               "& input, const ID& id = ID{}) noexcept\n{\n";
        res += "    return encode_set_combine(output, id";
        for (const ComponentType& component : set.components)
        {
            res += ",\n                          input." + component.named_type.name + ", " + tags_class +
                   "::" + component.named_type.name;
        }
        res += ");\n}\n\n";
        return res;
    }
    else
    {
        return "";
    }
}

std::string create_decode_functions(const Assignment& assignment, const std::string& module_name)
{
    if (absl::holds_alternative<BuiltinType>(assignment.type) &&
        absl::holds_alternative<SequenceType>(absl::get<BuiltinType>(assignment.type)))
    {
        const SequenceType& sequence = absl::get<SequenceType>(absl::get<BuiltinType>(assignment.type));
        std::string         res;
        const std::string   tags_class = assignment.name + "Tags";

        res +=
            "constexpr const char " + assignment.name + "_name[] = \"" + module_name + "::" + assignment.name + "\";\n";
        res += "template <typename ID = ExplicitIdentifier<UniversalTag::sequence>>\n";
        res += "inline bool decode(const BerView& input, " + module_name + "::" + assignment.name +
               "& output, const ID& id = ID{}) noexcept\n{\n";
        res += "    return decode_sequence_combine<" + assignment.name + "_name>(input, id";
        for (const ComponentType& component : sequence.components)
        {
            res += ",\n                          output." + component.named_type.name + ", " + tags_class +
                   "::" + component.named_type.name;
        }
        res += ");\n}\n\n";

        res += "template <typename ID = ExplicitIdentifier<UniversalTag::sequence>>\n";
        res += "inline bool decode(BerViewIterator& input, " + module_name + "::" + assignment.name +
               "& output, const ID& id = ID{}) noexcept\n{\n";
        res += "    bool success = decode(*input, output, id) > 0;\n";
        res += "    ++input;\n";
        res += "    return success;\n";
        res += "}\n\n";
        return res;
    }
    else if (absl::holds_alternative<BuiltinType>(assignment.type) &&
             absl::holds_alternative<SetType>(absl::get<BuiltinType>(assignment.type)))
    {
        const SetType&    set = absl::get<SetType>(absl::get<BuiltinType>(assignment.type));
        std::string       res;
        const std::string tags_class = assignment.name + "Tags";

        res +=
            "constexpr const char " + assignment.name + "_name[] = \"" + module_name + "::" + assignment.name + "\";\n";
        res += "template <typename ID = ExplicitIdentifier<UniversalTag::set>>\n";
        res += "inline bool decode(const BerView& input, " + module_name + "::" + assignment.name +
               "& output, const ID& id = ID{}) noexcept\n{\n";
        res += "    return decode_set_combine<" + assignment.name + "_name>(input, id";
        for (const ComponentType& component : set.components)
        {
            res += ",\n                          output." + component.named_type.name + ", " + tags_class +
                   "::" + component.named_type.name;
        }
        res += ");\n}\n\n";

        res += "template <typename ID = ExplicitIdentifier<UniversalTag::set>>\n";
        res += "inline bool decode(BerViewIterator& input, " + module_name + "::" + assignment.name +
               "& output, const ID& id = ID{}) noexcept\n{\n";
        res += "    bool success = decode(*input, output, id) > 0;\n";
        res += "    ++input;\n";
        res += "    return success;\n";
        res += "}\n\n";
        return res;
    }
    else
    {
        return "";
    }
}

std::string create_include(const std::string& path) { return "#include \"" + path + "\"\n"; }

std::string add_namespace(const std::string& name, const std::string& enclosed)
{
    std::string output;

    output += "namespace " + name + " {\n";
    output += enclosed;
    output += "} // End namespace " + name + "\n";

    return output;
}

std::string create_body(const Asn1Tree& tree)
{
    std::string output;
    output += "\n";

    for (const Assignment& assignment : tree.assignments)
    {
        output += create_assignment(assignment, tree.tagging_default);
    }

    return output;
}

std::string create_detail_body(const Asn1Tree& tree)
{
    std::string output;
    output += "\n";

    output += "/* Functionality provided for Encoding and Decoding BER */\n\n";
    for (const Assignment& assignment : tree.assignments)
    {
        output += create_encode_functions(assignment, tree.module_reference, tree.tagging_default);
        output += create_decode_functions(assignment, tree.module_reference) + "\n";
    }
    return output;
}

std::string create_output_file(const Asn1Tree& tree, const std::string detail_filename)
{
    std::string output;
    output += "#pragma once\n\n";
    output += create_include("fast_ber/ber_types/All.hpp");
    output += "\n";

    output += add_namespace("fast_ber", add_namespace(tree.module_reference, create_body(tree)));
    output += '\n' + create_include(strip_path(detail_filename)) + '\n';

    return output;
}

int main(int argc, char** argv)
{
    if (argc != 3)
    {
        std::cout << "Please provide the following arguments:\n";
        std::cout << "    Arg 1: asn.1 input file\n";
        std::cout << "    Arg 2: output filename\n";
        return -1;
    }

    std::string input_filename = argv[1];

    std::ifstream input_file(input_filename);
    if (!input_file.good())
    {
        std::cout << "Could not open input file\n";
        return -1;
    }

    const std::string output_filename = std::string(argv[2]) + ".hpp";
    const std::string detail_filame   = std::string(argv[2]) + ".detail.hpp";

    std::ofstream output_file(output_filename);
    if (!output_file.good())
    {
        std::cout << "Could not create output file: " + output_filename + "\n";
        return -1;
    }
    std::ofstream detail_output_file(detail_filame);
    if (!detail_output_file.good())
    {
        std::cout << "Could not create output file: " + detail_filame + "\n";
        return -1;
    }

    std::string buffer(std::istreambuf_iterator<char>(input_file), {});

    Context context;
    context.cursor                  = buffer.c_str();
    context.location.begin.filename = &input_filename;
    context.location.end.filename   = &input_filename;

    yy::asn1_parser parser(context);

    const auto res = parser.parse();
    if (!res)
    {
        context.asn1_tree.assignments = reorder_assignments(context.asn1_tree.assignments);

        output_file << create_output_file(context.asn1_tree, detail_filame);
        detail_output_file << add_namespace("fast_ber", create_detail_body(context.asn1_tree));
    }
}
