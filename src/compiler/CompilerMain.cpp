#include "autogen/asn_compiler.hpp"
#include "fast_ber/compiler/CompilerTypes.hpp"

std::string strip_path(const std::string& path)
{
    std::size_t found = path.find_last_of("/\\");
    if (found == std::string::npos)
    {
        return path;
    }
    return path.substr(found + 1);
}

std::string make_type_optional(const std::string& type) { return "Optional<" + type + ">"; }

std::string create_assignment(const Assignment& assignment, TaggingMode tagging_mode)
{
    if (absl::holds_alternative<BuiltinType>(assignment.type) &&
        absl::holds_alternative<SequenceType>(absl::get<BuiltinType>(assignment.type)))
    {
        const SequenceType& sequence = absl::get<SequenceType>(absl::get<BuiltinType>(assignment.type));

        std::string res = "struct " + assignment.name + " {\n";

        for (const ComponentType& component : sequence)
        {
            std::string component_type = to_string(component.named_type.type);
            if (component.is_optional)
            {
                component_type = make_type_optional(component_type);
            }
            res += "    " + component_type + " " + component.named_type.name + ";\n";
        }
        res += "};\n\n";
        return res;
    }
    else
    {
        return "using " + assignment.name + " = " + "TaggedType<" + to_string(assignment.type) + ", " +
               universal_tag(assignment.type, tagging_mode) + ">;\n\n";
    }
}

std::string create_encode_functions(const Assignment& assignment, TaggingMode tagging_mode)
{
    if (absl::holds_alternative<BuiltinType>(assignment.type) &&
        absl::holds_alternative<SequenceType>(absl::get<BuiltinType>(assignment.type)))
    {
        const SequenceType& sequence = absl::get<SequenceType>(absl::get<BuiltinType>(assignment.type));
        std::string         res;
        const std::string   tags_class = assignment.name + "Tags";

        res += "namespace " + tags_class + " {\n";
        for (const ComponentType& component : sequence)
        {
            res += "static const auto " + component.named_type.name + " = " +
                   universal_tag(component.named_type.type, tagging_mode);
            res += "{};\n";
        }
        res += "}\n\n";

        res += "template <fast_ber::UniversalTag T>\n";
        res += "inline EncodeResult encode_with_specific_id(absl::Span<uint8_t> output, const " + assignment.name +
               "& input, const ExplicitIdentifier<T>& id) noexcept\n{\n";
        res += "    return encode_combine(output, id";
        for (const ComponentType& component : sequence)
        {
            res += ",\n                          input." + component.named_type.name + ", " + tags_class +
                   "::" + component.named_type.name;
        }
        res += ");\n}\n\n";

        res += "inline EncodeResult encode(absl::Span<uint8_t> output, const " + assignment.name +
               "& input) noexcept\n{\n";
        res += "    return encode_with_specific_id(output, input, ExplicitIdentifier<UniversalTag::sequence_of>{});\n";
        res += "}\n\n";

        return res;
    }
    else
    {
        return "";
    }
}

std::string create_decode_functions(const Assignment& assignment)
{
    if (absl::holds_alternative<BuiltinType>(assignment.type) &&
        absl::holds_alternative<SequenceType>(absl::get<BuiltinType>(assignment.type)))
    {
        const SequenceType& sequence = absl::get<SequenceType>(absl::get<BuiltinType>(assignment.type));
        std::string         res;
        const std::string   tags_class = assignment.name + "Tags";

        res += "constexpr const char " + assignment.name + "_name[] = \"" + assignment.name + "\";\n";
        res += "template <fast_ber::UniversalTag T>\n";
        res += "inline bool decode_with_specific_id(const BerView& input, " + assignment.name +
               "& output, const ExplicitIdentifier<T>& id) noexcept\n{\n";
        res += "    return decode_combine<" + assignment.name + "_name>(input, id";
        for (const ComponentType& component : sequence)
        {
            res += ",\n                          output." + component.named_type.name + ", " + tags_class +
                   "::" + component.named_type.name;
        }
        res += ");\n}\n\n";

        res += "template <fast_ber::UniversalTag T>\n";
        res += "inline bool decode_with_specific_id(BerViewIterator& input, " + assignment.name +
               "& output, const ExplicitIdentifier<T>& id) noexcept\n{\n";
        res += "    bool success = decode_with_specific_id(*input, output, id) > 0;\n";
        res += "    ++input;\n";
        res += "    return success;\n";
        res += "}\n\n";

        res += "inline bool decode(absl::Span<const uint8_t> input, " + assignment.name + "& output) noexcept\n{\n";
        res += "    return decode_with_specific_id(BerView(input), output, "
               "ExplicitIdentifier<UniversalTag::sequence_of>{});\n";
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

std::string create_body(const Asn1Tree& tree, const std::string detail_filename)
{
    std::string output;
    output += "\n";

    for (auto iter = tree.assignments.crbegin(); iter != tree.assignments.crend(); ++iter)
    {
        output += create_assignment(*iter, tree.tagging_default);
    }

    output += create_include(strip_path(detail_filename)) + '\n';
    return output;
}

std::string create_detail_body(const Asn1Tree& tree)
{
    std::string output;
    output += "\n";

    output += "/* Functionality provided for Encoding and Decoding BER */\n\n";
    for (auto iter = tree.assignments.crbegin(); iter != tree.assignments.crend(); ++iter)
    {
        output += create_encode_functions(*iter, tree.tagging_default);
        output += create_decode_functions(*iter) + "\n";
    }
    return output;
}

std::string create_output_file(const Asn1Tree& tree, const std::string detail_filename)
{
    std::string output;
    output += "#pragma once\n\n";
    output += create_include("fast_ber/ber_types/All.hpp");
    output += create_include("fast_ber/util/Encode.hpp");
    output += create_include("fast_ber/util/Decode.hpp");
    output += "\n";

    output += add_namespace("fast_ber", add_namespace(tree.module_reference, create_body(tree, detail_filename)));

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
        output_file << create_output_file(context.asn1_tree, detail_filame);
        detail_output_file << create_detail_body(context.asn1_tree);
    }
}
