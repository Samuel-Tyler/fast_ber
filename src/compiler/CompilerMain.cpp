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

AssignmentInfo create_assignment(const Assignment& assignment, TaggingMode tagging_mode)
{
    AssignmentInfo info;
    info.name       = assignment.name;
    info.depends_on = dependenies(assignment.type);

    if (absl::holds_alternative<BuiltinType>(assignment.type) &&
        absl::holds_alternative<SequenceType>(absl::get<BuiltinType>(assignment.type)))
    {
        const SequenceType& sequence = absl::get<SequenceType>(absl::get<BuiltinType>(assignment.type));
        info.assignment              = "struct " + assignment.name + to_string(sequence);
    }
    else if (absl::holds_alternative<BuiltinType>(assignment.type) &&
             absl::holds_alternative<SetType>(absl::get<BuiltinType>(assignment.type)))
    {
        const SetType& set = absl::get<SetType>(absl::get<BuiltinType>(assignment.type));
        info.assignment    = "struct " + assignment.name + to_string(set);
    }
    else if (absl::holds_alternative<BuiltinType>(assignment.type) &&
             absl::holds_alternative<EnumeratedType>(absl::get<BuiltinType>(assignment.type)))
    {
        const EnumeratedType& enumerated = absl::get<EnumeratedType>(absl::get<BuiltinType>(assignment.type));
        info.assignment                  = "enum class " + assignment.name + to_string(enumerated);
    }
    else
    {
        if (absl::holds_alternative<BuiltinType>(assignment.type) &&
            (absl::holds_alternative<PrefixedType>(absl::get<BuiltinType>(assignment.type)) ||
             absl::holds_alternative<ChoiceType>(absl::get<BuiltinType>(assignment.type)) ||
             absl::holds_alternative<SequenceOfType>(absl::get<BuiltinType>(assignment.type))))
        {
            // Alter the tag in tagged types
            info.assignment = "using " + assignment.name + " = " + "TaggedType<" + to_string(assignment.type) + ", " +
                              universal_tag(assignment.type, tagging_mode) + ">;\n\n";
        }
        else
        {
            info.assignment = "using " + assignment.name + " = " + to_string(assignment.type) + ";\n\n";
        }
    }

    return info;
}

std::string create_encode_functions(const Assignment& assignment, TaggingMode tagging_mode)
{
    if (absl::holds_alternative<BuiltinType>(assignment.type) &&
        absl::holds_alternative<SequenceType>(absl::get<BuiltinType>(assignment.type)))
    {
        std::string         res;
        const SequenceType& sequence   = absl::get<SequenceType>(absl::get<BuiltinType>(assignment.type));
        const std::string   tags_class = assignment.name + "Tags";

        res += "namespace " + tags_class + " {\n";
        int tag_counter = 0;
        for (const ComponentType& component : sequence.components)
        {
            res += "static const auto " + component.named_type.name + " = ";
            if (absl::holds_alternative<BuiltinType>(component.named_type.type) &&
                (absl::holds_alternative<PrefixedType>(absl::get<BuiltinType>(component.named_type.type)) ||
                 absl::holds_alternative<ChoiceType>(absl::get<BuiltinType>(component.named_type.type)) ||
                 absl::holds_alternative<SequenceOfType>(absl::get<BuiltinType>(component.named_type.type))))
            {
                res += universal_tag(component.named_type.type, tagging_mode);
            }
            else
            {
                res += "ImplicitIdentifier<Class::context_specific, " + std::to_string(tag_counter++) + ">";
            }
            res += "{};\n";
        }
        res += "}\n\n";

        res += "template <typename ID = ExplicitIdentifier<UniversalTag::sequence_of>>\n";
        res += "inline EncodeResult encode_with_specific_id(absl::Span<uint8_t> output, const " + assignment.name +
               "& input, const ID& id = ID{}) noexcept\n{\n";
        res += "    return encode_combine(output, id";
        for (const ComponentType& component : sequence.components)
        {
            res += ",\n                          input." + component.named_type.name + ", " + tags_class +
                   "::" + component.named_type.name;
        }
        res += ");\n}\n\n";

        res += "template <typename ID = ExplicitIdentifier<UniversalTag::sequence_of>>";
        res += "inline EncodeResult encode(absl::Span<uint8_t> output, const " + assignment.name +
               "& input) noexcept\n{\n";
        res += "    return encode_with_specific_id(output, input, ID{});\n";
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
        res += "template <typename ID = ExplicitIdentifier<UniversalTag::sequence_of>>\n";
        res += "inline bool decode_with_specific_id(const BerView& input, " + assignment.name +
               "& output, const ID& id = ID{}) noexcept\n{\n";
        res += "    return decode_combine<" + assignment.name + "_name>(input, id";
        for (const ComponentType& component : sequence.components)
        {
            res += ",\n                          output." + component.named_type.name + ", " + tags_class +
                   "::" + component.named_type.name;
        }
        res += ");\n}\n\n";

        res += "template <typename ID = ExplicitIdentifier<UniversalTag::sequence_of>>\n";
        res += "inline bool decode_with_specific_id(BerViewIterator& input, " + assignment.name +
               "& output, const ID& id = ID{}) noexcept\n{\n";
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

    std::unordered_map<std::string, AssignmentInfo> assignment_infos;
    assignment_infos.reserve(tree.assignments.size());

    for (const Assignment& assignment : tree.assignments)
    {
        const AssignmentInfo& assignment_info  = create_assignment(assignment, tree.tagging_default);
        assignment_infos[assignment_info.name] = assignment_info;
    }

    const std::vector<AssignmentInfo>& ordered_assignment_infos = reorder_assignments(assignment_infos);
    for (const AssignmentInfo& assignment : ordered_assignment_infos)
    {
        output += assignment.assignment;
    }

    output += create_include(strip_path(detail_filename)) + '\n';
    return output;
}

std::string create_detail_body(const Asn1Tree& tree)
{
    std::string output;
    output += "\n";

    output += "/* Functionality provided for Encoding and Decoding BER */\n\n";
    for (const Assignment& assignment : tree.assignments)
    {
        output += create_encode_functions(assignment, tree.tagging_default);
        output += create_decode_functions(assignment) + "\n";
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
