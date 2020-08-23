#include "fast_ber/compiler/EncodeDecode.hpp"
#include "fast_ber/compiler/Identifier.hpp"

#include "absl/strings/string_view.h"

#include <vector>

class CodeBlock
{
  public:
    size_t&       indentation() { return m_indentation; }
    const size_t& indentation() const { return m_indentation; }

    void add_line() { m_content += '\n'; }
    void add_line(const absl::string_view& line)
    {
        m_content += std::string(m_indentation * 4, ' ');
        m_content.insert(m_content.end(), line.begin(), line.end());
        m_content += '\n';
    }

    void add_block(const absl::string_view& block) { m_content.insert(m_content.end(), block.begin(), block.end()); }
    void add_block(const CodeBlock& block) { m_content += block.to_string(); }
    const std::string& to_string() const { return m_content; }

  private:
    size_t      m_indentation = 0;
    std::string m_content     = {};
};

class CodeScope
{
  public:
    CodeScope(CodeBlock& block) : m_block(block)
    {
        m_block.add_line("{");
        m_block.indentation()++;
    }
    CodeScope(CodeScope&) = delete;
    CodeScope(CodeScope&& rhs) : m_block(rhs.m_block) { rhs.active = false; }
    ~CodeScope()
    {
        if (active)
        {
            m_block.indentation()--;
            m_block.add_line("}");
        }
    }

  private:
    bool       active = true;
    CodeBlock& m_block;
};

std::string create_encode_functions(const std::vector<std::string>& namespaces, const std::string& assignment_name,
                                    const std::vector<Parameter>& parameters, const Type& type, const Module& module,
                                    const Asn1Tree& tree);
std::string create_decode_functions(const std::vector<std::string>& namespaces, const std::string& assignment_name,
                                    const std::vector<Parameter>& parameters, const Type& type, const Module& module,
                                    const Asn1Tree& tree);

template <typename CollectionType>
std::string
create_collection_encode_functions(const std::vector<std::string>& namespaces, const std::string& assignment_name,
                                   const std::vector<Parameter>& parameters, const CollectionType& collection,
                                   const Module& module, const Asn1Tree& tree)
{
    CodeBlock block;

    std::vector<std::string> child_namespaces = namespaces;
    child_namespaces.push_back(assignment_name);

    // Make encode functions for nested types
    for (const ComponentType& component : collection.components)
    {
        block.add_block(create_encode_functions(child_namespaces, component.named_type.name + "_type", parameters,
                                                component.named_type.type, module, tree));
    }

    std::string namespace_name = module.module_reference + "::";

    int count = 0;
    for (const std::string& ns : namespaces)
    {
        namespace_name += ns + "<Identifier" + std::to_string(count++) + ">::";
    }

    const std::string type_identifier = "Identifier" + std::to_string(count);
    const std::string name =
        "fast_ber::" + namespace_name + assignment_name + create_template_arguments({type_identifier});

    for (int i = 0; i <= count; i++)
    {
        block.add_line(create_template_definition({"Identifier" + std::to_string(i)}));
    }

    block.add_line("inline EncodeResult " + name + "::encode(absl::Span<uint8_t> output) const noexcept");
    {
        auto scope = CodeScope(block);
        block.add_line("constexpr size_t header_length_guess = fast_ber::encoded_length(0," + type_identifier + "{});");
        block.add_line("if (output.length() < header_length_guess)");

        {
            auto scope2 = CodeScope(block);
            block.add_line("return EncodeResult{false, 0};");
        }

        if (collection.components.size() != 0)
        {
            block.add_line("EncodeResult res;");
            block.add_line("auto content = output;");
            block.add_line("content.remove_prefix(header_length_guess);");
        }
        block.add_line("size_t content_length = 0;");

        for (const ComponentType& component : collection.components)
        {
            block.add_line("res = fast_ber::encode(content, this->" + component.named_type.name + ");");
            block.add_line("if (!res.success)");
            {
                auto scope2 = CodeScope(block);
                block.add_line("return res;");
            }
            block.add_line("content.remove_prefix(res.length);");
            block.add_line("content_length += res.length;");
        }
        block.add_line("return wrap_with_ber_header(output, content_length, " + type_identifier +
                       "{}, header_length_guess);");
    }
    block.add_line();

    for (int i = 0; i <= count; i++)
    {
        block.add_line(create_template_definition({"Identifier" + std::to_string(i)}));
    }
    block.add_line("size_t " + name + "::encoded_length() const noexcept");
    {
        auto scope = CodeScope(block);
        block.add_line("size_t content_length = 0;");
        block.add_line();
        for (const ComponentType& component : collection.components)
        {
            block.add_line("content_length += " + component.named_type.name + ".encoded_length();");
        }
        block.add_line();
        block.add_line("return fast_ber::encoded_length(content_length, " + type_identifier + "{});");
    }
    block.add_line();
    return block.to_string();
}

std::string create_choice_encode_functions(const std::vector<std::string>& namespaces,
                                           const std::string& assignment_name, const std::vector<Parameter>&,
                                           const ChoiceType& choice, const Module& module, const Asn1Tree&)
{
    CodeBlock block;

    std::vector<std::string> child_namespaces = namespaces;
    child_namespaces.push_back(assignment_name);

    std::string namespace_name = module.module_reference + "::";

    int count = 0;
    for (const std::string& ns : namespaces)
    {
        namespace_name += ns + "<Identifier" + std::to_string(count++) + ">::";
    }

    const std::string type_identifier = "Identifier" + std::to_string(count);
    const std::string name =
        "fast_ber::" + namespace_name + assignment_name + create_template_arguments({type_identifier});

    for (int i = 0; i <= count; i++)
    {
        block.add_line(create_template_definition({"Identifier" + std::to_string(i)}));
    }

    block.add_line("inline EncodeResult " + name + "::encode(absl::Span<uint8_t> output) const noexcept");
    {
        auto scope1 = CodeScope(block);
        block.add_line("EncodeResult res;");
        block.add_line("auto content = output;");

        // If an alternative (non ChoiceId) identifier is provided choice type should be wrapped,
        // else use identifier of selected choice
        block.add_line("size_t header_length_guess = 0;");
        block.add_line("if (!IsChoiceId<" + type_identifier + ">::value)");
        {
            auto scope2 = CodeScope(block);
            block.add_line(" header_length_guess = fast_ber::encoded_length(0," + type_identifier + "{});");

            block.add_line("if (output.length() < header_length_guess)");
            {
                auto scope3 = CodeScope(block);
                block.add_line("return EncodeResult{false, 0};");
            }
            block.add_line("content.remove_prefix(header_length_guess);");
        }

        block.add_line("switch (this->index())");
        {
            auto scope2 = CodeScope(block);
            for (size_t i = 0; i < choice.choices.size(); i++)
            {
                block.add_line("case " + std::to_string(i) + ":");
                block.add_line("	res = fast_ber::get<" + std::to_string(i) + ">(*this)" + ".encode(content);");
                block.add_line("	break;");
            }
            block.add_line("default: assert(0);");
        }

        block.add_line("if (!IsChoiceId<" + type_identifier + ">::value)");
        {
            auto scope2 = CodeScope(block);
            block.add_line("if (!res.success)");
            {
                auto scope3 = CodeScope(block);
                block.add_line("return res;");
            }
            block.add_line("const size_t content_length = res.length;");
            block.add_line("res =  wrap_with_ber_header(output, content_length, " + type_identifier +
                           "{}, header_length_guess);");
            block.add_line("return res;");
        }
        block.add_line("else");
        {
            auto scope2 = CodeScope(block);
            block.add_line("return res;");
        }
    }

    block.add_line();

    for (int i = 0; i <= count; i++)
    {
        block.add_line(create_template_definition({"Identifier" + std::to_string(i)}));
    }

    block.add_line("inline std::size_t " + name + "::encoded_length() const noexcept");
    {
        auto scope1 = CodeScope(block);
        block.add_line("std::size_t content_length = 0;");
        block.add_line("switch (this->index())");
        {
            auto scope2 = CodeScope(block);
            for (size_t i = 0; i < choice.choices.size(); i++)
            {
                block.add_line("case " + std::to_string(i) + ":");
                block.add_line("	content_length = fast_ber::get<" + std::to_string(i) + ">(*this)" +
                               ".encoded_length();");
                block.add_line("	break;");
            }
            block.add_line("default: assert(0);");
        }

        block.add_line("if (!IsChoiceId<" + type_identifier + ">::value)");
        {
            auto scope2 = CodeScope(block);
            block.add_line("return fast_ber::encoded_length(content_length, " + type_identifier + "{});");
        }
        block.add_line("else");
        {
            auto scope2 = CodeScope(block);
            block.add_line("return content_length;");
        }
    }

    block.add_line();
    return block.to_string();
}

template <typename CollectionType>
std::string
create_collection_decode_functions(const std::vector<std::string>& namespaces, const std::string& assignment_name,
                                   const std::vector<Parameter>& parameters, const CollectionType& collection,
                                   const Module& module, const Asn1Tree& tree)
{
    std::string namespace_name = module.module_reference + "::";

    std::vector<std::string> child_namespaces = namespaces;
    child_namespaces.push_back(assignment_name);

    int count = 0;
    for (const std::string& ns : namespaces)
    {
        namespace_name += ns + "<Identifier" + std::to_string(count++) + ">::";
    }

    const std::string type_identifier = "Identifier" + std::to_string(count);
    const std::string name =
        "fast_ber::" + namespace_name + assignment_name + create_template_arguments({type_identifier});

    CodeBlock block;
    for (int i = 0; i <= count; i++)
    {
        block.add_line(create_template_definition({"Identifier" + std::to_string(i)}));
    }

    block.add_line("DecodeResult " + name + "::decode(BerView input) noexcept");
    {
        auto scope = CodeScope(block);
        block.add_line("if (!has_correct_header(input, " + type_identifier + "{}, Construction::constructed))");
        {
            auto scope2 = CodeScope(block);
            block.add_line("return DecodeResult{false};");
        }

        if (collection.components.size() > 0)
        {
            block.add_line("DecodeResult res;");
            block.add_line("auto iterator = (" + type_identifier + "::depth() == 1) ? input.begin()");
            block.add_line("                                            : input.begin()->begin();");

            if (std::is_same<CollectionType, SetType>::value)
            {
                block.add_line("auto const end = (" + type_identifier + "::depth() == 1) ? input.end()");
                block.add_line("                                             : input.begin()->end();");
                block.add_line();

                block.add_line("std::array<size_t, " + std::to_string(collection.components.size()) +
                               "> decode_counts = {};");
                block.add_line("while (iterator != end)");
                {
                    auto scope2 = CodeScope(block);
                    if (module.tagging_default == TaggingMode::automatic)
                    {
                        block.add_line("if (iterator->class_() == " + to_string(Class::context_specific) + ")");
                        auto scope3 = CodeScope(block);
                        {
                            block.add_line("switch (iterator->tag())");
                            auto   scope4 = CodeScope(block);
                            size_t i      = 0;
                            for (const ComponentType& component : collection.components)
                            {
                                block.add_line("case " + std::to_string(i) + ":");
                                block.add_line("res = " + component.named_type.name + ".decode(*iterator);");
                                block.add_line("if (!res.success)");
                                {
                                    auto scope5 = CodeScope(block);
                                    {
                                        block.add_line("return res;");
                                    }
                                }
                                block.add_line("++decode_counts[" + std::to_string(i) + "];");
                                block.add_line("++iterator;");
                                block.add_line("continue;");
                                i++;
                            }
                        }
                    }
                    else
                    {
                        for (Class class_ : {
                                 Class::universal,
                                 Class::application,
                                 Class::context_specific,
                                 Class::private_,
                             })
                        {
                            if (std::any_of(collection.components.begin(), collection.components.end(),
                                            [&](const ComponentType& component) {
                                                const std::vector<Identifier>& outer_ids =
                                                    outer_identifiers(component.named_type.type, module, tree);
                                                return std::any_of(
                                                    outer_ids.begin(), outer_ids.end(),
                                                    [class_](const Identifier& id) { return id.class_ == class_; });
                                            }))
                            {

                                block.add_line("if (iterator->class_() == " + to_string(class_) + ")");
                                auto scope3 = CodeScope(block);
                                {
                                    block.add_line("switch (iterator->tag())");
                                    auto   scope4 = CodeScope(block);
                                    size_t i      = 0;
                                    for (const ComponentType& component : collection.components)
                                    {
                                        const std::vector<Identifier>& ids =
                                            outer_identifiers(component.named_type.type, module, tree);
                                        for (const Identifier& id : ids)
                                        {
                                            if (id.class_ == class_)
                                            {
                                                block.add_line("case " + std::to_string(id.tag_number) + ":");
                                                block.add_line("res = " + component.named_type.name +
                                                               ".decode(*iterator);");
                                                block.add_line("if (!res.success)");
                                                {
                                                    auto scope5 = CodeScope(block);
                                                    {
                                                        block.add_line("return res;");
                                                    }
                                                }
                                                block.add_line("++decode_counts[" + std::to_string(i) + "];");
                                                block.add_line("++iterator;");
                                                block.add_line("continue;");
                                            }
                                        }
                                        i++;
                                    }
                                }
                            }
                        }
                    }
                    block.add_line(R"(std::cerr << "Invalid ID when decoding set [)" + name +
                                   R"(] [" << iterator->identifier() << "]" << std::endl;)");
                    block.add_line("return fast_ber::DecodeResult{false};");
                }
                size_t i = 0;
                for (const ComponentType& component : collection.components)
                {

                    block.add_line("if (decode_counts[" + std::to_string(i) + "] == 0)");
                    {
                        auto scope3 = CodeScope(block);

                        if (component.is_optional)
                        {
                            block.add_line("this->" + component.named_type.name + " = fast_ber::empty;");
                        }
                        else if (component.default_value)
                        {
                            block.add_line("this->" + component.named_type.name + ".set_to_default();");
                        }
                        else
                        {
                            block.add_line(R"(std::cerr << "Missing non-optional member [)" +
                                           component.named_type.name + R"(] of set [)" + name + R"()]" << std::endl;)");
                            block.add_line("return fast_ber::DecodeResult{false};");
                        }
                    }
                    block.add_line("if (decode_counts[" + std::to_string(i) + "] > 1)");
                    {
                        auto scope3 = CodeScope(block);
                        block.add_line(R"(std::cerr << "Member [)" + component.named_type.name +
                                       R"(] present multiple times in set [)" + name + R"()]" << std::endl;)");
                        block.add_line("return fast_ber::DecodeResult{false};");
                    }
                    i++;
                }
            }
            else
            {
                for (const ComponentType& component : collection.components)
                {
                    if (component.is_optional || component.default_value)
                    {
                        std::string id_check = "if (iterator->is_valid() && (false ";
                        for (const Identifier& id : outer_identifiers(component.named_type.type, module, tree))
                        {
                            id_check += " || " + id.name() + "::check_id_match(iterator->class_(), iterator->tag())";
                        }
                        id_check += "))";
                        block.add_line(id_check);

                        {
                            auto scope2 = CodeScope(block);
                            block.add_line("res = this->" + component.named_type.name + ".decode(*iterator);");
                            block.add_line("if (!res.success)");
                            {
                                auto scope3 = CodeScope(block);
                                {
                                    block.add_line("return res;");
                                }
                            }
                            block.add_line("++iterator;");
                        }
                        block.add_line("else");
                        {
                            auto scope3 = CodeScope(block);
                            if (component.is_optional)
                            {
                                block.add_line("this->" + component.named_type.name + " = fast_ber::empty;");
                            }
                            else
                            {
                                block.add_line("this->" + component.named_type.name + ".set_to_default();");
                            }
                        }
                    }
                    else
                    {
                        block.add_line("res = this->" + component.named_type.name + ".decode(*iterator);");
                        block.add_line("if (!res.success)");
                        {
                            auto scope2 = CodeScope(block);
                            block.add_line("return res;");
                        }
                        block.add_line("++iterator;");
                    }
                }
            }
        }
        block.add_line("return DecodeResult{true};");
    }

    // Make encode functions for nested types
    for (const ComponentType& component : collection.components)
    {
        block.add_block(create_decode_functions(child_namespaces, component.named_type.name + "_type", parameters,
                                                component.named_type.type, module, tree));
    }
    return block.to_string();
}

std::string create_choice_decode_functions(const std::vector<std::string>& namespaces,
                                           const std::string& assignment_name, const std::vector<Parameter>&,
                                           const ChoiceType& choice, const Module& module, const Asn1Tree& tree)
{
    CodeBlock block;

    std::vector<std::string> child_namespaces = namespaces;
    child_namespaces.push_back(assignment_name);

    std::string namespace_name = module.module_reference + "::";

    int count = 0;
    for (const std::string& ns : namespaces)
    {
        namespace_name += ns + "<Identifier" + std::to_string(count++) + ">::";
    }

    const std::string type_identifier = "Identifier" + std::to_string(count);
    const std::string name =
        "fast_ber::" + namespace_name + assignment_name + create_template_arguments({type_identifier});

    for (int i = 0; i <= count; i++)
    {
        block.add_line(create_template_definition({"Identifier" + std::to_string(i)}));
    }

    block.add_line("inline DecodeResult " + name + "::decode(BerView input) noexcept");
    {
        auto scope1 = CodeScope(block);
        block.add_line("BerView content(input);");
        block.add_line("if (!IsChoiceId<" + type_identifier + ">::value)");
        {
            auto scope2 = CodeScope(block);
            block.add_line("if (!input.is_valid())");
            {
                auto scope3 = CodeScope(block);
                block.add_line(R"(std::cerr << "Invalid packet when decoding choice [)" + name + R"(]" << std::endl;)");
                block.add_line("return DecodeResult{false};");
            }
            block.add_line("if (!has_correct_header(input, " + type_identifier + "{}, Construction::constructed))");
            {
                auto scope3 = CodeScope(block);
                block.add_line(
                    R"(std::cerr << "Invalid header when decoding choice type [" << input.identifier() << "] in choice [)" +
                    name + R"(]" << std::endl;)");
                block.add_line("return DecodeResult{false};");
            }

            block.add_line("BerViewIterator child = (" + type_identifier +
                           "::depth() == 1) ? input.begin() : input.begin()->begin();");
            block.add_line("if (!child->is_valid())");
            {
                auto scope3 = CodeScope(block);
                block.add_line("return DecodeResult{false};");
            }
            block.add_line("content = *child;");
        }

        if (module.tagging_default == TaggingMode::automatic)
        {
            block.add_line("switch (content.tag())");
            {
                auto scope2 = CodeScope(block);
                for (size_t i = 0; i < choice.choices.size(); i++)
                {
                    block.add_line("case " + std::to_string(i) + ":");
                    block.add_line("	return this->template emplace<" + std::to_string(i) + ">().decode(content);");
                }
            }
        }
        else
        {
            for (Class class_ : {
                     Class::universal,
                     Class::application,
                     Class::context_specific,
                     Class::private_,
                 })
            {
                const std::vector<Identifier>& outer_ids = outer_identifiers(choice, module, tree);
                if (std::any_of(outer_ids.begin(), outer_ids.end(),
                                [class_](const Identifier& id) { return id.class_ == class_; }))
                {
                    block.add_line("switch (content.tag())");
                    auto   scope2 = CodeScope(block);
                    size_t i      = 0;
                    for (const NamedType& named_type : choice.choices)
                    {
                        const std::vector<Identifier>& ids = outer_identifiers(named_type.type, module, tree);
                        for (const Identifier& id : ids)
                        {
                            if (id.class_ == class_)
                            {
                                block.add_line("case " + std::to_string(id.tag_number) + ":");
                                block.add_line("	return this->template emplace<" + std::to_string(i) +
                                               ">().decode(content);");
                            }
                        }
                        i++;
                    }
                }
            }
        }
        block.add_line(R"(std::cerr << "Unknown tag [" << content.identifier() << "] in choice [)" + name +
                       R"(]" << std::endl;)");
        block.add_line("return DecodeResult{false};");
    }

    block.add_line();
    return block.to_string();
}

std::string create_encode_functions(const std::vector<std::string>& namespaces, const std::string& assignment_name,
                                    const std::vector<Parameter>& parameters, const Type& type, const Module& module,
                                    const Asn1Tree& tree)
{
    if (is_sequence(type))
    {
        const SequenceType& sequence = absl::get<SequenceType>(absl::get<BuiltinType>(type));
        return create_collection_encode_functions(namespaces, assignment_name, parameters, sequence, module, tree);
    }
    else if (is_set(type))
    {
        const SetType& set = absl::get<SetType>(absl::get<BuiltinType>(type));
        return create_collection_encode_functions(namespaces, assignment_name, parameters, set, module, tree);
    }
    else if (is_choice(type))
    {
        const ChoiceType& choice = absl::get<ChoiceType>(absl::get<BuiltinType>(type));
        return create_choice_encode_functions(namespaces, assignment_name, parameters, choice, module, tree);
    }
    return "";
}

std::string create_decode_functions(const std::vector<std::string>& namespaces, const std::string& assignment_name,
                                    const std::vector<Parameter>& parameters, const Type& type, const Module& module,
                                    const Asn1Tree& tree)
{
    if (is_sequence(type))
    {
        const SequenceType& sequence = absl::get<SequenceType>(absl::get<BuiltinType>(type));
        return create_collection_decode_functions(namespaces, assignment_name, parameters, sequence, module, tree);
    }
    else if (is_set(type))
    {
        const SetType& set = absl::get<SetType>(absl::get<BuiltinType>(type));
        return create_collection_decode_functions(namespaces, assignment_name, parameters, set, module, tree);
    }
    else if (is_choice(type))
    {
        const ChoiceType& choice = absl::get<ChoiceType>(absl::get<BuiltinType>(type));
        return create_choice_decode_functions(namespaces, assignment_name, parameters, choice, module, tree);
    }
    return "";
}

std::string create_encode_functions(const Assignment& assignment, const Module& module, const Asn1Tree& tree)
{
    if (absl::holds_alternative<TypeAssignment>(assignment.specific))
    {
        return create_encode_functions({}, assignment.name, assignment.parameters, type(assignment), module, tree);
    }

    return "";
}

std::string create_decode_functions(const Assignment& assignment, const Module& module, const Asn1Tree& tree)
{
    if (absl::holds_alternative<TypeAssignment>(assignment.specific))
    {
        return create_decode_functions({}, assignment.name, assignment.parameters, type(assignment), module, tree);
    }

    return "";
}
