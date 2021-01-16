#include "autogen/asn_compiler.hpp"

#include "fast_ber/compiler/CompilerTypes.hpp"
#include "fast_ber/compiler/CppGeneration.hpp"
#include "fast_ber/compiler/Dependencies.hpp"
#include "fast_ber/compiler/EncodeDecode.hpp"
#include "fast_ber/compiler/GenerateHelpers.hpp"
#include "fast_ber/compiler/Identifier.hpp"
#include "fast_ber/compiler/Logging.hpp"
#include "fast_ber/compiler/ObjectClass.hpp"
#include "fast_ber/compiler/Parameters.hpp"
#include "fast_ber/compiler/ReorderAssignments.hpp"
#include "fast_ber/compiler/ResolveType.hpp"
#include "fast_ber/compiler/TypeAsString.hpp"
#include "fast_ber/compiler/ValueAsString.hpp"
#include "fast_ber/compiler/ValueType.hpp"

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

std::string create_type_fwd(const std::string& name, const Type&, const Module&, const Asn1Tree&)
{
    return "struct " + name + ";\n";
}

std::string create_assignment(const Asn1Tree& tree, const Module& module, const Assignment& assignment)
{
    try
    {
        if (absl::holds_alternative<ValueAssignment>(assignment.specific)) // Value assignment
        {
            log_debug(tree, "Creating value assignment: " + module.module_reference + "." + assignment.name);

            const ValueAssignment& value_assign = absl::get<ValueAssignment>(assignment.specific);
            const Type&            assigned_to_type =
                (is_defined(value_assign.type))
                               ? type(resolve(tree, module.module_reference, absl::get<DefinedType>(value_assign.type)))
                               : value_assign.type;

            std::string constness =
                is_oid(assigned_to_type) ? std::string("static const ") : std::string("static constexpr ");

            std::string result =
                constness + value_type(value_assign.type, module, tree) + " " + assignment.name + " = ";

            result += value_as_string(NamedType{assignment.name, assigned_to_type}, value_assign.value) + ";\n";
            return result;
        }
        else if (absl::holds_alternative<TypeAssignment>(assignment.specific))
        {
            log_debug(tree, "Creating type assignment: " + module.module_reference + "." + assignment.name);

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
        throw(std::runtime_error("failed creating assignment [" + assignment.name + "] [" + e.what() + "]"));
    }
}

std::string create_imports(const Asn1Tree& tree, const Module& module)
{

    std::string output;

    for (const Import& import : module.imports)
    {
        for (const auto& import_name : import.imported_values)
        {

            log_debug(tree, "Value import " + module.module_reference + "." + import_name + " <- " +
                                import.module_reference + "." + import_name);

            output += "static const auto " + import_name + " = " + import.module_reference + "::" + import_name + ";\n";
        }
        if (import.imported_values.size() > 0)
        {
            output += "\n";
        }
    }

    return output;
}
std::string create_body(const Asn1Tree& tree, const Module& module)
{
    std::string output;
    output += "\n";
    output += create_imports(tree, module);

    for (const Assignment& assignment : module.assignments)
    {
        output += create_assignment(tree, module, assignment);
    }

    return output;
}

std::string create_fwd_declarations(const Asn1Tree& tree, const Module& module)
{
    std::string output;

    for (const Assignment& assignment : module.assignments)
    {
        if (absl::holds_alternative<TypeAssignment>(assignment.specific))
        {
            log_debug(tree, "Creating forward definition: " + module.module_reference + "." + assignment.name);
            output +=
                create_type_fwd(assignment.name, absl::get<TypeAssignment>(assignment.specific).type, module, tree);
        }
    }

    return output;
}

std::string create_fwd_body(const Asn1Tree& tree)
{
    log_debug(tree, "Creating .fwd.hpp");
    std::string output;
    output += "/* Forward declration of ASN.1 types */\n\n";
    output += "#pragma once\n\n";
    output += create_include("fast_ber/ber_types/All.hpp");
    output += "\n";

    std::string definitions;
    definitions += "using namespace abbreviations;\n";

    for (const auto& module : tree.modules)
    {
        definitions += add_namespace(module.module_reference, create_fwd_declarations(tree, module));
    }

    output += add_namespace("fast_ber", definitions) + '\n';
    log_debug(tree, "Done creating .fwd.hpp");
    return output;
}

std::string create_detail_body(const Asn1Tree& tree)
{
    log_debug(tree, "Creating .detail.hpp");
    std::string output;
    output += "/* Functionality provided for Encoding and Decoding BER */\n\n";

    std::string body = "\n";

    for (const Module& module : tree.modules)
    {
        std::string helpers;
        for (const Assignment& assignment : module.assignments)
        {
            body += create_encode_functions(assignment, module, tree);
            body += create_decode_functions(assignment, module, tree);
            helpers += create_helper_functions(assignment);
        }

        body += add_namespace(module.module_reference, helpers);
    }
    output += add_namespace("fast_ber", body) + "\n\n";
    log_debug(tree, "Done creating .detail.hpp");
    return output;
}

std::string create_output_file(const Asn1Tree& tree, const std::string& fwd_filename,
                               const std::string& detail_filename)
{
    log_debug(tree, "Creating .hpp");
    std::string output;
    output += "#pragma once\n\n";
    output += create_include("fast_ber/ber_types/All.hpp");
    output += create_include(strip_path(fwd_filename)) + '\n';
    output += "\n";

    std::string definitions;
    definitions += "using namespace abbreviations;\n\n";

    for (const auto& module : tree.modules)
    {
        definitions += add_namespace(module.module_reference, create_body(tree, module));
    }

    output += add_namespace("fast_ber", definitions) + '\n';
    output += create_include(strip_path(detail_filename)) + '\n';
    log_debug(tree, "Done creating .hpp");
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
        const std::string& fwd_filame      = std::string(argv[argc - 1]) + ".fwd.hpp";
        const std::string& detail_filame   = std::string(argv[argc - 1]) + ".detail.hpp";

        std::ofstream output_file(output_filename);
        std::ofstream fwd_output_file(fwd_filame);
        std::ofstream detail_output_file(detail_filame);
        if (!output_file.good())
        {
            std::cerr << "Could not create output file: " + output_filename + "\n";
            return -1;
        }
        if (!fwd_output_file.good())
        {
            std::cerr << "Could not create output file: " + fwd_filame + "\n";
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
            module.assignments = split_imports(context.asn1_tree, module.assignments, module.imports);
            module.assignments = split_definitions(module.assignments);
            check_duplicated_names(module.assignments, module.module_reference);
            module.assignments = split_nested_structures(module);
        }

        resolve_components_of(context.asn1_tree);
        resolve_object_classes(context.asn1_tree);

        for (auto& module : context.asn1_tree.modules)
        {
            module.assignments = reorder_assignments(module.assignments, context.asn1_tree, module);
        }

        output_file << create_output_file(context.asn1_tree, fwd_filame, detail_filame);
        fwd_output_file << create_fwd_body(context.asn1_tree);
        detail_output_file << create_detail_body(context.asn1_tree);

        return 0;
    }
    catch (const std::exception& e)
    {
        std::cerr << "Compilation error: " << e.what() << "\n";
        return -1;
    }
}
