#include "asn1_sam.hpp"
#include "fast_ber/compiler/CompilerTypes.hpp"

std::string create_assignment(const Assignment& assignment)
{
    if (std::holds_alternative<BuiltinType>(assignment.type) &&
        std::holds_alternative<SequenceType>(std::get<BuiltinType>(assignment.type)))
    {
        const SequenceType& sequence = std::get<SequenceType>(std::get<BuiltinType>(assignment.type));

        std::string res = "struct " + assignment.name + " {\n";

        for (const ComponentType& component : sequence)
        {
            res += "    " + (to_string(component.named_type.type) + " " + component.named_type.name + ";\n");
        }
        res += "};\n\n";

        res += "size_t encode(absl::Span<uint8_t> output, const " + assignment.name + "& input)\n{\n";
        res += "    return encode_combine(output";
        for (const ComponentType& component : sequence)
        {
            res += ", input." + component.named_type.name;
        }
        res += ");\n}\n";
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
    return output;
}

void generate_output_file(const Asn1Tree& tree)
{
    std::string output;
    output += create_include("fast_ber/ber_types/All.hpp");
    output += create_include("fast_ber/util/Encode.hpp");
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
