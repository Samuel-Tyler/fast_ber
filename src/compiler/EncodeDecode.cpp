#include "fast_ber/compiler/EncodeDecode.hpp"

#include "fast_ber/compiler/CppGeneration.hpp"
#include "fast_ber/compiler/Identifier.hpp"
#include "fast_ber/compiler/ResolveType.hpp"
#include "fast_ber/compiler/Visit.hpp"

#include "absl/strings/string_view.h"

#include <iostream>
#include <string>
#include <vector>

std::string make_component_function(const std::string& function, const NamedType& component, const Module& module,
                                    const Asn1Tree& tree)
{
    if (is_generated(resolve_type(tree, module.module_reference, component).type))
    {
        auto id = identifier(component.type, module, tree);
        if (!id.is_default_tagged)
            return function + "_with_id<" + id.name() + ">";
    }
    return function;
}

template <typename CollectionType>
CodeBlock create_collection_encode_functions(const std::string& name, const CollectionType& collection,
                                             const Module& module, const Asn1Tree& tree)
{
    CodeBlock block;

    block.add_line(create_template_definition({"Identifier_"}));
    block.add_line("inline EncodeResult " + name + "::encode_with_id(absl::Span<uint8_t> output) const noexcept");
    {
        auto scope = CodeScope(block);
        block.add_line("constexpr std::size_t header_length_guess = fast_ber::encoded_length(0, Identifier_{});");
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
        block.add_line("std::size_t content_length = 0;");

        for (const ComponentType& component : collection.components)
        {
            block.add_line("res = " + component.named_type.name + "." +
                           make_component_function("encode", component.named_type, module, tree) + "(content);");
            block.add_line("if (!res.success)");
            {
                auto scope2 = CodeScope(block);
                block.add_line("return res;");
            }
            block.add_line("content.remove_prefix(res.length);");
            block.add_line("content_length += res.length;");
        }
        block.add_line("return wrap_with_ber_header(output, content_length, Identifier_{}, header_length_guess);");
    }
    block.add_line();

    block.add_line(create_template_definition({"Identifier_"}));
    block.add_line("std::size_t " + name + "::encoded_length_with_id() const noexcept");
    {
        auto scope = CodeScope(block);
        block.add_line("std::size_t content_length = 0;");
        block.add_line();
        for (const ComponentType& component : collection.components)
        {
            block.add_line("content_length += this->" + component.named_type.name + "." +
                           make_component_function("encoded_length", component.named_type, module, tree) + "();");
        }
        block.add_line();
        block.add_line("return fast_ber::encoded_length(content_length, Identifier_{});");
    }
    block.add_line();
    return block;
}

CodeBlock create_choice_encode_functions(const std::string& name, const ChoiceType& choice, const Module& module,
                                         const Asn1Tree& tree)
{
    CodeBlock block;

    block.add_line(create_template_definition({"Identifier_"}));
    block.add_line("inline EncodeResult " + name + "::encode_with_id(absl::Span<uint8_t> output) const noexcept");
    {
        auto scope1 = CodeScope(block);
        block.add_line("EncodeResult res;");
        block.add_line("auto content = output;");
        // If an alternative (non ChoiceId) identifier is provided choice type should be wrapped,
        // else use identifier of selected choice
        block.add_line("std::size_t header_length_guess = 0;");
        block.add_line("if (!IsChoiceId<Identifier_>::value)");
        {
            auto scope2 = CodeScope(block);
            block.add_line(" header_length_guess = fast_ber::encoded_length(0,Identifier_{});");

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
            for (std::size_t i = 0; i < choice.choices.size(); i++)
            {
                block.add_line("case " + std::to_string(i) + ":");
                block.add_line("	res = fast_ber::get<" + std::to_string(i) + ">(*this)." +
                               make_component_function("encode", choice.choices[i], module, tree) + "(content);");
                block.add_line("	break;");
            }
            block.add_line("default: assert(0);");
        }

        block.add_line("if (!IsChoiceId<Identifier_>::value)");
        {
            auto scope2 = CodeScope(block);
            block.add_line("if (!res.success)");
            {
                auto scope3 = CodeScope(block);
                block.add_line("return res;");
            }
            block.add_line("const std::size_t content_length = res.length;");
            block.add_line("res = wrap_with_ber_header(output, content_length, Identifier_{}, header_length_guess);");
            block.add_line("return res;");
        }
        block.add_line("else");
        {
            auto scope2 = CodeScope(block);
            block.add_line("return res;");
        }
    }

    block.add_line();

    block.add_line(create_template_definition({"Identifier_"}));
    block.add_line("inline std::size_t " + name + "::encoded_length_with_id() const noexcept");
    {
        auto scope1 = CodeScope(block);
        block.add_line("std::size_t content_length = 0;");
        block.add_line("switch (this->index())");
        {
            auto scope2 = CodeScope(block);
            for (std::size_t i = 0; i < choice.choices.size(); i++)
            {
                block.add_line("case " + std::to_string(i) + ":");
                block.add_line("	content_length = fast_ber::get<" + std::to_string(i) + ">(*this)." +
                               make_component_function("encoded_length", choice.choices[i], module, tree) + "();");
                block.add_line("	break;");
            }
            block.add_line("default: assert(0);");
        }

        block.add_line("if (!IsChoiceId<Identifier_>::value)");
        {
            auto scope2 = CodeScope(block);
            block.add_line("return fast_ber::encoded_length(content_length, Identifier_{});");
        }
        block.add_line("else");
        {
            auto scope2 = CodeScope(block);
            block.add_line("return content_length;");
        }
    }

    block.add_line();
    return block;
}

template <typename CollectionType>
CodeBlock create_collection_decode_functions(const std::string& name, const CollectionType& collection,
                                             const Module& module, const Asn1Tree& tree)
{
    CodeBlock block;
    block.add_line(create_template_definition({"Identifier_"}));
    block.add_line("DecodeResult " + name + "::decode_with_id(BerView input) noexcept");
    {
        auto scope = CodeScope(block);
        block.add_line("if (!input.is_valid())");
        {
            auto scope2 = CodeScope(block);
            block.add_line(R"(FAST_BER_ERROR("Invalid packet when decoding collection [)" + name + R"(]");)");
            block.add_line("return DecodeResult{false};");
        }
        block.add_line("if (!has_correct_header(input, Identifier_{}, Construction::constructed))");
        {
            auto scope2 = CodeScope(block);
            block.add_line(
                R"(FAST_BER_ERROR("Invalid identifier [", input.identifier(), "] when decoding collection [)" + name +
                R"(]");)");
            block.add_line("return DecodeResult{false};");
        }

        if (collection.components.size() > 0)
        {
            block.add_line("DecodeResult res;");
            block.add_line("auto iterator = (Identifier_::depth() == 1) ? input.begin()");
            block.add_line("                                            : input.begin()->begin();");

            if (std::is_same<CollectionType, SetType>::value)
            {
                block.add_line("auto const end = (Identifier_::depth() == 1) ? input.end()");
                block.add_line("                                             : input.begin()->end();");
                block.add_line();

                block.add_line("std::array<std::size_t, " + std::to_string(collection.components.size()) +
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
                            auto        scope4 = CodeScope(block);
                            std::size_t i      = 0;
                            for (const ComponentType& component : collection.components)
                            {
                                block.add_line("case " + std::to_string(i) + ":");
                                block.add_line("res = this->" + component.named_type.name + "." +
                                               make_component_function("decode", component.named_type, module, tree) +
                                               "(*iterator);");
                                block.add_line("if (!res.success)");
                                {
                                    auto scope5 = CodeScope(block);
                                    {
                                        block.add_line(R"(FAST_BER_ERROR("failed to decode member [)" +
                                                       component.named_type.name + R"(] of collection [)" + name +
                                                       R"(]");)");
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
                                    auto        scope4 = CodeScope(block);
                                    std::size_t i      = 0;
                                    for (const ComponentType& component : collection.components)
                                    {
                                        const std::vector<Identifier>& ids =
                                            outer_identifiers(component.named_type.type, module, tree);
                                        for (const Identifier& id : ids)
                                        {
                                            if (id.class_ == class_)
                                            {
                                                block.add_line("case " + std::to_string(id.tag_number) + ":");
                                                block.add_line("res = this->" + component.named_type.name + "." +
                                                               make_component_function("decode", component.named_type,
                                                                                       module, tree) +
                                                               "(*iterator);");

                                                block.add_line("if (!res.success)");
                                                {
                                                    auto scope5 = CodeScope(block);
                                                    {
                                                        block.add_line(R"(FAST_BER_ERROR("failed to decode member [)" +
                                                                       component.named_type.name +
                                                                       R"(] of collection [)" + name + R"(]");)");
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
                    if (!collection.allow_extensions)
                    {
                        block.add_line(R"(FAST_BER_ERROR("Invalid ID when decoding set [)" + name +
                                       R"(] [", iterator->identifier(), "]");)");
                        block.add_line("return fast_ber::DecodeResult{false};");
                    }
                    else
                    {
                        block.add_line("++iterator;");
                    }
                }
                std::size_t i = 0;
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
                            block.add_line(R"(FAST_BER_ERROR("Missing non-optional member [)" +
                                           component.named_type.name + R"(] of set [)" + name + R"(]");)");
                            block.add_line("return fast_ber::DecodeResult{false};");
                        }
                    }
                    block.add_line("if (decode_counts[" + std::to_string(i) + "] > 1)");
                    {
                        auto scope3 = CodeScope(block);
                        block.add_line(R"(FAST_BER_ERROR("Member [)" + component.named_type.name +
                                       R"(] present multiple times in set [)" + name + R"(]");)");
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
                            block.add_line("res = this->" + component.named_type.name + "." +
                                           make_component_function("decode", component.named_type, module, tree) +
                                           "(*iterator);");

                            block.add_line("if (!res.success)");
                            {
                                auto scope3 = CodeScope(block);
                                {
                                    block.add_line(R"(FAST_BER_ERROR("failed to decode member [)" +
                                                   component.named_type.name + R"(] of collection [)" + name +
                                                   R"(]");)");

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
                        block.add_line("res = this->" + component.named_type.name + "." +
                                       make_component_function("decode", component.named_type, module, tree) +
                                       "(*iterator);");

                        block.add_line("if (!res.success)");
                        {
                            auto scope2 = CodeScope(block);
                            block.add_line(R"(FAST_BER_ERROR("failed to decode member [)" + component.named_type.name +
                                           R"(] of collection [)" + name + R"(]");)");
                            block.add_line("return res;");
                        }
                        block.add_line("++iterator;");
                    }
                }
            }
        }
        block.add_line("return DecodeResult{true};");
    }

    return block;
}

CodeBlock create_choice_decode_functions(const std::string& name, const ChoiceType& choice, const Module& module,
                                         const Asn1Tree& tree)
{
    CodeBlock block;

    block.add_line(create_template_definition({"Identifier"}));
    block.add_line("inline DecodeResult " + name + "::decode_with_id(BerView input) noexcept");
    {
        auto scope1 = CodeScope(block);
        block.add_line("BerView content(input);");
        block.add_line("if (!IsChoiceId<Identifier>::value)");
        {
            auto scope2 = CodeScope(block);
            block.add_line("if (!input.is_valid())");
            {
                auto scope3 = CodeScope(block);
                block.add_line(R"(FAST_BER_ERROR("Invalid packet when decoding choice [)" + name + R"(]");)");
                block.add_line("return DecodeResult{false};");
            }
            block.add_line("if (!has_correct_header(input, Identifier{}, Construction::constructed))");
            {
                auto scope3 = CodeScope(block);
                block.add_line(
                    R"(FAST_BER_ERROR("Invalid header when decoding choice type [", input.identifier(), "] in choice [)" +
                    name + R"(]");)");
                block.add_line("return DecodeResult{false};");
            }

            block.add_line(
                "BerViewIterator child = (Identifier::depth() == 1) ? input.begin() : input.begin()->begin();");
            block.add_line("if (!child->is_valid())");
            {
                auto scope3 = CodeScope(block);
                block.add_line(R"(FAST_BER_ERROR("Invalid child packet when decoding choice [)" + name + R"(]");)");

                block.add_line("return DecodeResult{false};");
            }
            block.add_line("content = *child;");
        }

        if (module.tagging_default == TaggingMode::automatic)
        {
            block.add_line("switch (content.tag())");
            {
                auto scope2 = CodeScope(block);
                for (std::size_t i = 0; i < choice.choices.size(); i++)
                {
                    block.add_line("case " + std::to_string(i) + ":");
                    block.add_line("	return this->template emplace<" + std::to_string(i) + ">()." +
                                   make_component_function("decode", choice.choices[i], module, tree) + "(content);");
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
                    auto        scope2 = CodeScope(block);
                    std::size_t i      = 0;
                    for (const NamedType& named_type : choice.choices)
                    {
                        const std::vector<Identifier>& ids = outer_identifiers(named_type.type, module, tree);
                        for (const Identifier& id : ids)
                        {
                            if (id.class_ == class_)
                            {
                                block.add_line("case " + std::to_string(id.tag_number) + ":");
                                block.add_line("	return this->template emplace<" + std::to_string(i) + ">()." +
                                               make_component_function("decode", choice.choices[i], module, tree) +
                                               "(content);");
                            }
                        }
                        i++;
                    }
                }
            }
        }
        block.add_line(R"(FAST_BER_ERROR("Unknown tag [", content.identifier(), "] in choice [)" + name + R"(]");)");
        block.add_line("return DecodeResult{false};");
    }

    block.add_line();
    return block;
}

CodeBlock create_encode_functions_impl(const Asn1Tree& tree, const Module& module, const Type& type,
                                       const std::string& name)
{
    if (is_sequence(type))
    {
        const SequenceType& sequence = absl::get<SequenceType>(absl::get<BuiltinType>(type));
        return create_collection_encode_functions(name, sequence, module, tree);
    }
    else if (is_set(type))
    {
        const SetType& set = absl::get<SetType>(absl::get<BuiltinType>(type));
        return create_collection_encode_functions(name, set, module, tree);
    }
    else if (is_choice(type))
    {
        const ChoiceType& choice = absl::get<ChoiceType>(absl::get<BuiltinType>(type));
        return create_choice_encode_functions(name, choice, module, tree);
    }
    return {};
}

CodeBlock create_decode_functions_impl(const Asn1Tree& tree, const Module& module, const Type& type,
                                       const std::string& name)
{
    if (is_sequence(type))
    {
        const SequenceType& sequence = absl::get<SequenceType>(absl::get<BuiltinType>(type));
        return create_collection_decode_functions(name, sequence, module, tree);
    }
    else if (is_set(type))
    {
        const SetType& set = absl::get<SetType>(absl::get<BuiltinType>(type));
        return create_collection_decode_functions(name, set, module, tree);
    }
    else if (is_choice(type))
    {
        const ChoiceType& choice = absl::get<ChoiceType>(absl::get<BuiltinType>(type));
        return create_choice_decode_functions(name, choice, module, tree);
    }
    return {};
}

std::string create_encode_functions(const Assignment& assignment, const Module& module, const Asn1Tree& tree)
{
    if (absl::holds_alternative<TypeAssignment>(assignment.specific))
    {
        return visit_all_types(tree, module, assignment, create_encode_functions_impl).to_string();
    }

    return "";
}

std::string create_decode_functions(const Assignment& assignment, const Module& module, const Asn1Tree& tree)
{
    if (absl::holds_alternative<TypeAssignment>(assignment.specific))
    {
        return visit_all_types(tree, module, assignment, create_decode_functions_impl).to_string();
    }

    return "";
}
