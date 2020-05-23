#include "autogen/asn_compiler.hpp"

#include "fast_ber/compiler/CompilerTypes.hpp"
#include "fast_ber/compiler/CppGeneration.hpp"
#include "fast_ber/compiler/Dependencies.hpp"
#include "fast_ber/compiler/EncodeDecode.hpp"
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

std::string create_type_fwd(const std::string& name, const Type& type, const Module& module, const Asn1Tree& tree)
{
    TaggingInfo tag = identifier(type, module, tree);

    return create_template_definition({"Identifier = " + tag.name()}) + "\nstruct " + name + ";\n";
}

std::string create_type_assignment(const std::string& name, const Type& assignment_type, const Module& module,
                                   const Asn1Tree& tree)
{
    std::string res;

    if (is_set(assignment_type) || is_sequence(assignment_type))
    {
        res += create_template_definition({"Identifier"}) + '\n';
        res += "struct " + name + type_as_string(assignment_type, module, tree, name);
    }
    else if (is_enumerated(assignment_type))
    {
        res += "enum class " + name + "Values" + type_as_string(assignment_type, module, tree);
        res += create_template_definition({"Identifier"}) + '\n';
        res += "FAST_BER_ALIAS(" + name + ", " + "Enumerated<" + name + "Values, Identifier>);\n";
    }
    else
    {
        res += create_template_definition({"Identifier"}) + '\n';
        res +=
            "FAST_BER_ALIAS(" + name + ", " + type_as_string(assignment_type, module, tree, "", "Identifier") + ");\n";
    }
    return res;
}

std::string create_type_assignment(const Assignment& assignment, const Module& module, const Asn1Tree& tree)
{
    return create_type_assignment(assignment.name, absl::get<TypeAssignment>(assignment.specific).type, module, tree) +
           "\n";
}

std::string create_assignment(const Asn1Tree& tree, const Module& module, const Assignment& assignment)
{
    try
    {
        if (absl::holds_alternative<ValueAssignment>(assignment.specific)) // Value assignment
        {
            log_debug(tree, "Creating value assignment: " + module.module_reference + "." + assignment.name);

            const ValueAssignment& value_assign = absl::get<ValueAssignment>(assignment.specific);
            std::string            result =
                "static const " + value_type(value_assign.type, module, tree) + " " + assignment.name + " = ";

            const Type& assigned_to_type =
                (is_defined(value_assign.type))
                    ? type(resolve(tree, module.module_reference, absl::get<DefinedType>(value_assign.type)))
                    : value_assign.type;

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

template <typename CollectionType>
std::string create_identifier_functions_recursive(const std::string& assignment_name, const CollectionType& collection,
                                                  const std::string&              module_name,
                                                  const std::vector<std::string>& namespace_names)
{
    std::string res;

    auto child_namespaces = namespace_names;
    child_namespaces.push_back(assignment_name);

    for (const auto& child : collection.components)
    {
        if (is_sequence(child.named_type.type))
        {
            res += create_identifier_functions_recursive(
                child.named_type.name + "_type", absl::get<SequenceType>(absl::get<BuiltinType>(child.named_type.type)),
                module_name, child_namespaces);
        }
        else if (is_set(child.named_type.type))
        {
            res += create_identifier_functions_recursive(
                child.named_type.name + "_type", absl::get<SetType>(absl::get<BuiltinType>(child.named_type.type)),
                module_name, child_namespaces);
        }
    }

    std::string name = module_name + "::";
    for (const std::string& ns : namespace_names)
    {
        name += ns + "<>::template ";
    }
    name += assignment_name + create_template_arguments({"Identifier"});

    res += create_template_definition({"Identifier"}) + '\n';
    res += "struct IdentifierType<" + name + ">{ using type = Identifier ; };\n";

    return res;
}

std::string create_identifier_functions(const Assignment& assignment, const Module& module)
{
    std::string res;

    if (absl::holds_alternative<TypeAssignment>(assignment.specific))
    {
        const TypeAssignment& type_assign = absl::get<TypeAssignment>(assignment.specific);
        if (is_sequence(type_assign.type))
        {
            const SequenceType& sequence = absl::get<SequenceType>(absl::get<BuiltinType>(type_assign.type));
            return create_identifier_functions_recursive(assignment.name, sequence,
                                                         "fast_ber::" + module.module_reference, {});
        }
        else if (is_set(type_assign.type))
        {
            const SetType& set = absl::get<SetType>(absl::get<BuiltinType>(type_assign.type));
            return create_identifier_functions_recursive(assignment.name, set, "fast_ber::" + module.module_reference,
                                                         {});
        }
    }
    return "";
}

template <typename CollectionType>
std::string create_ostream_operators(const CollectionType& collection, const std::string& name_with_template_params)
{
    std::string res;
    res += create_template_definition({"Identifier"}) + '\n';
    res += "std::ostream& operator<<(std::ostream& os, const " + name_with_template_params + "& object)\n";
    res += "{\n";
    res += "    bool first = true;\n";
    res += "    os << '{';\n";
    if (collection.components.size() == 0)
    {
        res += "    (void)object;\n";
        res += "    (void)first;\n";
    }

    for (const ComponentType& component : collection.components)
    {
        res += "    if (!first) os << \", \";\n";
        res +=
            "    os << \"\\\"" + component.named_type.name + "\\\" : \" << object." + component.named_type.name + ";\n";
        res += "    first = false;\n";
    }
    res += "    os << '}';\n";
    res += "    return os;\n";
    res += "}\n\n";
    return res;
}

template <typename CollectionType>
std::string create_collection_equality_operators(const CollectionType& collection, std::vector<std::string> parents,
                                                 const std::string& name)
{
    std::string full_namespace;
    for (const std::string& parent : parents)
    {
        full_namespace += parent + "<>::template ";
    }

    std::string name_with_template_params   = full_namespace + name + create_template_arguments({"Identifier"});
    std::string name_with_template_params_1 = full_namespace + name + create_template_arguments({"Identifier1"});
    std::string name_with_template_params_2 = full_namespace + name + create_template_arguments({"Identifier2"});

    std::string res;

    res += create_template_definition({"Identifier1", "Identifier2"}) + '\n';
    res += "bool operator==(";
    res += "const " + name_with_template_params_1 + "& lhs, ";
    res += "const " + name_with_template_params_2 + "& rhs)\n";
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

    res += create_template_definition({"Identifier1", "Identifier2"}) + '\n';
    res += "bool operator!=(";
    res += "const " + name_with_template_params_1 + "& lhs, ";
    res += "const " + name_with_template_params_2 + "& rhs)\n";
    res += "{\n";
    res += "    return !(lhs == rhs);\n}\n\n";

    res += create_ostream_operators(collection, name_with_template_params);

    parents.push_back(name);

    std::string child_equality;
    // Create assignments for any children too
    for (const ComponentType& component : collection.components)
    {
        if (is_sequence(component.named_type.type))
        {
            const SequenceType& sequence = absl::get<SequenceType>(absl::get<BuiltinType>(component.named_type.type));
            child_equality +=
                create_collection_equality_operators(sequence, parents, component.named_type.name + "_type");
        }
        else if (is_set(component.named_type.type))
        {
            const SetType& set = absl::get<SetType>(absl::get<BuiltinType>(component.named_type.type));
            child_equality += create_collection_equality_operators(set, parents, component.named_type.name + "_type");
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
            return create_collection_equality_operators(sequence, {}, assignment.name);
        }
        else if (is_set(type_assignment.type))
        {
            const SetType& set = absl::get<SetType>(absl::get<BuiltinType>(type_assignment.type));
            return create_collection_equality_operators(set, {}, assignment.name);
        }
        else if (is_enumerated(type_assignment.type))
        {
            const EnumeratedType& e    = absl::get<EnumeratedType>(absl::get<BuiltinType>(type_assignment.type));
            const std::string     name = assignment.name + "Values";

            std::string res;
            res += "const char* to_string(" + name + " e)\n";
            res += "{\n";
            res += "    switch (e)\n";
            res += "    {\n";
            for (const EnumerationValue& enum_value : e.enum_values)
            {
                res += "    case " + name + "::" + enum_value.name + ": return \"" + enum_value.name + "\";\n";
            }

            res += "    default: return \"Invalid state!\";\n";
            res += "    }\n";
            res += "}\n\n";

            res += "std::ostream& operator<<(std::ostream& os, " + name + " object)\n";
            res += "{\n";
            res += "    os << to_string(object);\n";
            res += "    return os;\n";
            res += "}\n\n";
            return res;
        }
    }

    return "";
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

    std::string body;

    for (const Module& module : tree.modules)
    {
        // Inside namespace due to argument dependant lookup rules
        for (const Assignment& assignment : module.assignments)
        {
            body += create_identifier_functions(assignment, module);
        }
    }

    body += '\n';

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
