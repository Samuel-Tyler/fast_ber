#include "fast_ber/compiler/CppGeneration.hpp"

std::string create_include(const std::string& path) { return "#include \"" + path + "\"\n"; }

std::string create_template_definition(const std::vector<std::string>& types)
{
    if (types.empty())
    {
        return "";
    }

    std::string definition = "template <";
    bool        is_first   = true;
    for (const std::string& type : types)
    {
        if (!is_first)
        {
            definition += ", ";
        }
        definition += "typename " + type;
        is_first = false;
    }

    definition += ">";
    return definition;
}

std::string create_template_arguments(const std::vector<std::string>& types)
{
    if (types.empty())
    {
        return "";
    }

    std::string arguments = "<";
    bool        is_first  = true;
    for (const std::string& type : types)
    {
        if (!is_first)
        {
            arguments += ", ";
        }
        arguments += type;
        is_first = false;
    }

    arguments += ">";
    return arguments;
}

std::string add_namespace(const std::string& name, const std::string& enclosed)
{
    std::string output;

    output += "namespace " + name + " {\n";
    output += enclosed;
    output += "} // End namespace " + name + "\n";

    return output;
}

// Creates a C++ type following conventions of project (ie starts with capital letter)
std::string make_type_name(std::string name)
{
    assert(!name.empty());
    name[0] = std::toupper(name[0]);
    return name;
}
