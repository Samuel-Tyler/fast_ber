#include "fast_ber/compiler/CompilerTypes.hpp"

void generate_output_file(const Asn1Tree& tree)
{
    std::cout << "#include \"fast_ber/ber_types/All.hpp\"\n";
    std::cout << "#include \"fast_ber/util/Encode.hpp\"\n\n";
    std::cout << "namespace fast_ber {\n\n";
    std::cout << "namespace " << tree.module_reference << " {\n\n";

    for (auto iter = tree.assignments.crbegin(); iter != tree.assignments.crend(); ++iter)
    {
        std::cout << to_string(*iter) << "\n";
    }

    std::cout << "} // End namespace " + tree.module_reference + "\n";
    std::cout << "} // End namespace fast_ber \n";
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
