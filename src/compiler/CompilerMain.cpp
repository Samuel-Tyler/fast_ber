#include "autogen/asn_compiler.hpp"
#include "fast_ber/compiler/CompilerTypes.hpp"

std::string make_type_optional(const std::string& type) { return "Optional<" + type + ">"; }

std::string create_assignment(const Assignment& assignment)
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
        throw std::runtime_error("Unhandled assignment type: " + to_string(assignment.type));
    }
}

std::string create_encode_decode_functions(const Assignment& assignment)
{
    if (absl::holds_alternative<BuiltinType>(assignment.type) &&
        absl::holds_alternative<SequenceType>(absl::get<BuiltinType>(assignment.type)))
    {
        const SequenceType& sequence = absl::get<SequenceType>(absl::get<BuiltinType>(assignment.type));
        std::string         res;
        const std::string   tags_class = assignment.name + "Tags";

        int i = 0;
        res += "enum class " + tags_class + " : int {\n";
        for (const ComponentType& component : sequence)
        {
            res += "    " + component.named_type.name + " = " + std::to_string(i++) + ",\n";
        }
        res += "};\n\n";

        res += "EncodeResult encode_with_specific_id(absl::Span<uint8_t> output, const " + assignment.name +
               "& input, Class class_, int tag)\n{\n";
        res += "    return encode_combine(output, class_, tag";
        for (const ComponentType& component : sequence)
        {
            res += ",\n                          input." + component.named_type.name + ", Class::context_specific, " +
                   "val(" + tags_class + "::" + component.named_type.name + ")";
        }
        res += ");\n}\n\n";

        res += "EncodeResult encode(absl::Span<uint8_t> output, const " + assignment.name + "& input)\n{\n";
        res += "    return encode_with_specific_id(output, input, Class::universal, val(UniversalTag::sequence_of));\n";
        res += "}\n\n";

        res += "constexpr const char " + assignment.name + "_name[] = \"" + assignment.name + "\";\n";
        res += "bool decode_with_specific_id(const BerView& input, " + assignment.name + "& output, Tag tag)\n{\n";
        res += "    return decode_combine< " + assignment.name + "_name>(input, tag";
        for (const ComponentType& component : sequence)
        {
            res += ",\n                          output." + component.named_type.name + ", val(" + tags_class +
                   "::" + component.named_type.name + ")";
        }
        res += ");\n}\n\n";

        res += "bool decode_with_specific_id(BerViewIterator& input, " + assignment.name + "& output, Tag tag)\n{\n";
        res += "    bool success = decode_with_specific_id(*input, output, tag) > 0;\n";
        res += "    ++input;\n";
        res += "    return success;\n";
        res += "}\n\n";

        res += "bool decode(absl::Span<const uint8_t> input, " + assignment.name + "& output)\n{\n";
        res += "    return decode_with_specific_id(BerView(input), output, val(UniversalTag::sequence_of));\n";
        res += "}\n\n";

        return res;
    }
    else
    {
        throw std::runtime_error("Unhandled assignment type: " + to_string(assignment.type));
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

    for (auto iter = tree.assignments.crbegin(); iter != tree.assignments.crend(); ++iter)
    {
        output += create_assignment(*iter) + "\n";
    }

    output += "\n\n\n\n\n\n/* Encoding and Decoding Functionality */\n\n\n";
    for (auto iter = tree.assignments.crbegin(); iter != tree.assignments.crend(); ++iter)
    {
        output += create_encode_decode_functions(*iter) + "\n";
    }
    return output;
}

void generate_output_file(const Asn1Tree& tree)
{
    std::string output;
    output += create_include("fast_ber/ber_types/All.hpp");
    output += create_include("fast_ber/util/Encode.hpp");
    output += create_include("fast_ber/util/Decode.hpp");
    output += "\n";

    output += add_namespace("fast_ber", add_namespace(tree.module_reference, create_body(tree)));
    output += "\n";

    std::cout << output;
}

int main(int argc, char** argv)
{
    if (argc != 2)
    {
        std::cout << "Please provide ASN file for parsing\n";
        return -1;
    }

    std::string   filename = argv[1];
    std::ifstream f(filename);
    if (!f.good())
    {
        std::cout << "Could not open input file\n";
        return -1;
    }

    std::string buffer(std::istreambuf_iterator<char>(f), {});

    Context context;
    context.cursor                  = buffer.c_str();
    context.location.begin.filename = &filename;
    context.location.end.filename   = &filename;

    yy::asn1_parser parser(context);

    const auto res = parser.parse();

    if (!res)
    {
        generate_output_file(context.asn1_tree);
    }
}
