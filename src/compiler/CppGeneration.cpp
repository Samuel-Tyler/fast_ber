#include "fast_ber/compiler/CppGeneration.hpp"

#include <cctype>
#include <sstream>

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
std::string make_type_name(std::string name, absl::string_view parent_name)
{
    assert(!name.empty());
    name[0] = std::toupper(name[0]);
    if (name == parent_name) // C++ types may not share a name with their enclosing class
    {
        return name + '_';
    }
    return name;
}

const std::ostream& CodeBlock::stream(std::ostream& os, std::size_t indentation = 0) const
{
    for (const CodeBlockContents& content : m_content)
    {
        std::size_t depth = indentation + content.indentation;
        if (absl::holds_alternative<CodeBlock>(content.data))
        {
            absl::get<CodeBlock>(content.data).stream(os, depth);
        }
        else
        {
            const std::string& line = absl::get<std::string>(content.data);
            if (!line.empty())
            {
                for (std::size_t i = 0; i < depth * 4; i++)
                {
                    os << ' ';
                }
                os << line;
            }
            os << '\n';
        }
    }
    return os;
}

std::string CodeBlock::to_string() const
{
    std::stringstream ss;
    stream(ss);
    return ss.str();
}
