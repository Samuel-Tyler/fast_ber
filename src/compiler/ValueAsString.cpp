#include "fast_ber/compiler/ValueAsString.hpp"

#include <iostream>

std::string cpp_value(const HexStringValue& hex)
{
    std::string res = "\"";
    size_t      i   = 0;

    if (hex.value.length() % 2 == 1)
    {
        const std::string& byte = std::string(hex.value.begin(), hex.value.begin() + 1);
        res += "\\";
        res += std::to_string(std::stoi(byte, nullptr, 16));
        i++;
    }

    for (; i < hex.value.length(); i += 2)
    {
        const std::string& byte = std::string(hex.value.begin() + i, hex.value.begin() + i + 2);
        res += "\\";
        res += std::to_string(std::stoi(byte, nullptr, 16));
    }

    return res + "\"";
}

std::string value_as_string(const NamedType& value_type, const Value& value)
{
    assert(!is_defined(value_type.type)); // value_type should be concrete

    std::string result;
    if (is_oid(value_type.type))
    {
        result += "ObjectIdentifier<>{";
        try
        {
            const ObjectIdComponents& object_id = ObjectIdComponents(value);

            for (size_t i = 0; i < object_id.components.size(); i++)
            {
                if (object_id.components[i].value)
                {
                    result += std::to_string(*object_id.components[i].value);
                }
                else
                {
                    result += std::to_string(0);
                }

                if (i < object_id.components.size() - 1)
                {
                    result += ", ";
                }
            }
        }
        catch (const std::runtime_error& e)
        {
            std::cerr << std::string("Warning: Value is not an object identifier : ") + e.what() << std::endl;
            return "";
        }
        result += "}";
    }
    else if (is_bit_string(value_type.type))
    {
        if (absl::holds_alternative<BitStringValue>(value.value_selection))
        {
            const BitStringValue& bstring = absl::get<BitStringValue>(value.value_selection);
            (void)bstring; // TODO: convert bstring to cstring
            result += "\"\"";
        }
        else
        {
            result += "\"\"";
        }
    }
    else if (is_enumerated(value_type.type))
    {
        if (absl::holds_alternative<DefinedValue>(value.value_selection))
        {
            const DefinedValue& defined = absl::get<DefinedValue>(value.value_selection);
            result += value_type.name + "Values::" + defined.reference;
        }
        else
        {
            throw std::runtime_error("Unexpected Enumerated type");
        }
    }
    else if (absl::holds_alternative<std::string>(value.value_selection))
    {
        const std::string& string = absl::get<std::string>(value.value_selection);
        result += string;
    }
    else if (absl::holds_alternative<HexStringValue>(value.value_selection))
    {
        const HexStringValue& hstring = absl::get<HexStringValue>(value.value_selection);
        result += "absl::string_view(" + cpp_value(hstring) + ")";
    }
    else if (absl::holds_alternative<CharStringValue>(value.value_selection))
    {
        const CharStringValue& cstring = absl::get<CharStringValue>(value.value_selection);
        result += "absl::string_view(" + cstring.value + ")";
    }
    else if (absl::holds_alternative<int64_t>(value.value_selection))
    {
        const int64_t integer = absl::get<int64_t>(value.value_selection);
        result += std::to_string(integer);
    }
    else if (absl::holds_alternative<DefinedValue>(value.value_selection))
    {
        const DefinedValue& defined = absl::get<DefinedValue>(value.value_selection);
        result += defined.reference;
    }
    else if (absl::holds_alternative<BooleanValue>(value.value_selection))
    {
        const BooleanValue& boolean = absl::get<BooleanValue>(value.value_selection);
        result += (boolean.value) ? "true" : "false";
    }
    else if (absl::holds_alternative<double>(value.value_selection))
    {
        const double& real = absl::get<double>(value.value_selection);
        result += std::to_string(real);
    }
    else
    {
        throw std::runtime_error("Strange value assign");
    }
    return result;
}
