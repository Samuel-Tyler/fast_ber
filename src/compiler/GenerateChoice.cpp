#include "fast_ber/compiler/GenerateChoice.hpp"

#include "fast_ber/compiler/GenerateHelpers.hpp"
#include "fast_ber/compiler/Identifier.hpp"
#include "fast_ber/compiler/TypeAsString.hpp"
#include "fast_ber/compiler/Visit.hpp"

CodeBlock create_choice_definition(const ChoiceType& choice, const Module& module, const Asn1Tree& tree,
                                   const std::string& name, const std::string& identifier_override)
{
    bool        is_first = true;
    std::string type_list;
    for (const auto& named_type : choice.choices)
    {
        if (!is_first)
            type_list += ", ";
        is_first = false;

        type_list += make_type_name(named_type.name);
    }

    const auto  id = identifier_override.empty() ? identifier(choice, module, tree).name() : identifier_override;
    std::string storage =
        "::fast_ber::Choice<Choices<" + type_list + ">," + id + ", " + to_string(choice.storage) + ">";

    CodeBlock block;
    block.add_line("struct " + name);
    {
        CodeScope scope(block, true);

        int64_t tag_counter = 0;
        for (const auto& named_type : choice.choices)
        {
            if (module.tagging_default == TaggingMode::automatic)
            {
                std::string choice_id = Identifier(Class::context_specific, tag_counter++).name();
                block.add_block(create_type_assignment(make_type_name(named_type.name), named_type.type, module, tree,
                                                       choice_id, false));
            }
            else
            {
                block.add_block(
                    create_type_assignment(make_type_name(named_type.name), named_type.type, module, tree, {}, false));
            }
        }
        block.add_line("using Storage = " + storage + ';');
        block.add_line();

        // Type helpers
        block.add_line("struct Nothing_{};");
        block.add_line("template <std::size_t i, bool = i < (" + std::to_string(choice.choices.size()) + ")>");
        block.add_line("struct ToTypeImpl{ using type = Nothing_; };");

        block.add_line("template <std::size_t i>");
        block.add_line("struct ToTypeImpl<i, true>{ using type = dynamic::detail::TypeAtIndex<i, " + type_list +
                       ">; };");

        block.add_line("template <std::size_t i>");
        block.add_line("using ToType = typename ToTypeImpl<i>::type;");
        block.add_line("template <typename T_>");
        block.add_line("using ExactlyOnce = dynamic::detail::ExactlyOnce<T_, " + type_list + ">;");
        block.add_line("template <typename T_>");
        block.add_line("using AcceptedIndex = dynamic::detail::AcceptedIndex<T_&&, dynamic::DynamicVariant<" +
                       type_list + ">>;");
        block.add_line("template <typename T_>");
        block.add_line("using AcceptedType = ToType<AcceptedIndex<T_>::value>;");
        block.add_line();

        // Standard Constructors / Assignment
        block.add_line(name + "() noexcept {}");
        block.add_line(name + "(const " + name + "& rhs) noexcept : m_storage(rhs.m_storage) {}");
        block.add_line(name + "(" + name + "&& rhs) noexcept : m_storage(rhs.m_storage) {}");
        block.add_line(name + "& operator=(const " + name +
                       "& rhs) noexcept { this->m_storage = rhs.m_storage; return *this; }");
        block.add_line(name + "& operator=(" + name +
                       "&& rhs) noexcept { this->m_storage = std::move(rhs.m_storage); return *this; }");
        block.add_line();

        block.add_line("template <typename T_,");
        block.add_line("    typename = absl::enable_if_t<!std::is_same<absl::decay_t<T_>, " + name + ">::value>,");
        block.add_line("    typename = absl::enable_if_t<ExactlyOnce<AcceptedType<T_&&>>::value && "
                       "std::is_constructible<AcceptedType<T_&&>, T_&&>::value>>");
        block.add_line(name + "(T_&& t) noexcept(std::is_nothrow_constructible<AcceptedType<T_&&>, T_&&>::value) : "
                              "m_storage(std::forward<T_>(t)) {}");

        block.add_line("template <typename T_, typename... Args,");
        block.add_line(
            "    typename = absl::enable_if_t<ExactlyOnce<T_>::value && std::is_constructible<T_, Args&&...>::value>>");
        block.add_line("explicit " + name +
                       "(in_place_type_t<T_> i, Args&&... args) : m_storage(std::forward<Args>(i, args)...) {}");

        block.add_line("template <typename T_, typename U, typename... Args,");
        block.add_line("    typename = absl::enable_if_t<ExactlyOnce<T_>::value && std::is_constructible<T_, "
                       "std::initializer_list<U>&, Args&&...>::value>>");
        block.add_line("explicit " + name +
                       "(in_place_type_t<T_> ip, std::initializer_list<U> il, Args&&... args) : m_storage(ip, il, "
                       "std::forward<Args>(args)...) {}");

        block.add_line("template <std::size_t i, typename... Args,");
        block.add_line("    typename = absl::enable_if_t<std::is_constructible<ToType<i>, Args&&...>::value>>");
        block.add_line("explicit " + name +
                       "(in_place_index_t<i> ip, Args&&... args) : m_storage(ip, std::forward<Args>(args)...) {}");

        block.add_line();

        // Emplace
        block.add_line("template <typename T_, typename... Args,");
        block.add_line(
            "    typename = absl::enable_if_t<std::is_constructible<T_, Args...>::value && ExactlyOnce<T_>::value>>");
        block.add_line("T_& emplace(Args&&... args)");
        {
            CodeScope scope2(block);
            {
                block.add_line("return m_storage.emplace<T_>(std::forward<Args>(args)...);");
            }
        }
        block.add_line("template <std::size_t i, typename... Args>");
        block.add_line("    absl::enable_if_t<std::is_constructible<ToType<i>, Args...>::value,");
        block.add_line("    ToType<i>&>");
        block.add_line("emplace(Args&&... args)");
        {
            CodeScope scope2(block);
            {
                block.add_line("return m_storage.emplace<i>(std::forward<Args>(args)...);");
            }
        }
        block.add_line();

        // Index
        block.add_line("std::size_t index() const noexcept { return m_storage.index(); }");

        // Encode / Decode Functionality
        block.add_line(create_template_definition({"Identifier"}));
        block.add_line("std::size_t encoded_length_with_id() const noexcept;");
        block.add_line(create_template_definition({"Identifier"}));
        block.add_line("EncodeResult encode_with_id(absl::Span<uint8_t> output) const noexcept;");
        block.add_line(create_template_definition({"Identifier"}));
        block.add_line("DecodeResult decode_with_id(BerView output) noexcept;");
        block.add_line();

        block.add_line("std::size_t encoded_length() const noexcept");
        block.add_line("{ return encoded_length_with_id<" + id + ">(); }");
        block.add_line("EncodeResult encode(absl::Span<uint8_t> output) const noexcept");
        block.add_line("{ return encode_with_id<" + id + ">(output); }");
        block.add_line("DecodeResult decode(BerView input) noexcept");
        block.add_line("{ return decode_with_id<" + id + ">(input); }");
        block.add_line();

        // ID
        block.add_line("using AsnId = " + id + ";");
        block.add_line();

        // Impl Helper
        for (const std::string& constness : {std::string("const "), std::string()})
        {
            block.add_line(constness + "Storage& impl() " + constness + "noexcept");
            block.add_line("{ return m_storage; }");
        }

        block.add_line("private:");
        block.add_line("Storage m_storage;");
    }
    return block;
}

CodeBlock create_choice_getters(const std::string& name, const ChoiceType& choice)
{
    CodeBlock block;

    for (const std::string& constness : {std::string("const "), std::string()})
    {
        std::size_t i = 0;
        for (const NamedType& named_type : choice.choices)
        {
            block.add_line(constness + name + "::" + make_type_name(named_type.name, name) + "& get_impl(" + constness +
                           name + "& choice, std::integral_constant<std::size_t, " + std::to_string(i) + ">) noexcept");
            {
                CodeScope scope(block);
                block.add_line("return fast_ber::get<" + std::to_string(i) + ">(choice.impl());");
            }
            ++i;
        }

        block.add_line("template <std::size_t i>");
        block.add_line("decltype(get_impl(std::declval<" + constness + name +
                       "&>(), std::integral_constant<std::size_t, i>())) get(" + constness + name +
                       "& choice) noexcept");

        {
            CodeScope scope(block);
            block.add_line("return get_impl(choice, std::integral_constant<std::size_t, i>());");
        }

        block.add_line(create_template_definition({"Choice"}));
        block.add_line(constness + "Choice& get(" + constness + name + "& choice) noexcept");
        {
            CodeScope scope(block);
            block.add_line("return fast_ber::get<Choice>(choice.impl());");
        }
        block.add_line();
    }
    return block;
}

CodeBlock create_choice_types(const std::string& name, const ChoiceType&)
{
    CodeBlock block;

    block.add_line("template <std::size_t i>");
    block.add_line("struct variant_alternative<i, " + name + ">");
    CodeScope scope(block, true);
    {
        block.add_line("using type = fast_ber::variant_alternative_t<i, " + name + "::Storage>;");
    }
    block.add_line();

    return block;
}

CodeBlock create_choice_holds_alternative(const std::string& name, const ChoiceType& choice)
{
    CodeBlock block;

    block.add_line("template <typename T>");
    block.add_line("bool holds_alternative(const " + name + "& c) noexcept");

    {
        CodeScope   scope(block);
        std::size_t i = 0;
        for (const auto& named_type : choice.choices)
        {
            const std::string choice_type_name = name + "::" + make_type_name(named_type.name);
            block.add_line("if (std::is_same<T, " + choice_type_name + ">::value && c.index() == " + std::to_string(i) +
                           ") { return true; }");
            ++i;
        }

        block.add_line("return false;");
    }
    block.add_line();

    return block;
}

CodeBlock create_choice_helpers(const ChoiceType&, const std::string& name, bool forward_definition_only)
{
    std::string maybe_semicolon = forward_definition_only ? ";" : "";
    CodeBlock   block;
    block.add_line("bool operator==(const " + name + "& lhs, const " + name + "& rhs) noexcept" + maybe_semicolon);
    if (!forward_definition_only)
    {
        CodeScope scope1(block);
        block.add_line("return lhs.impl() == rhs.impl();");
    }
    block.add_line();

    block.add_line("bool operator!=(const " + name + "& lhs, const " + name + "& rhs) noexcept" + maybe_semicolon);
    if (!forward_definition_only)
    {
        CodeScope scope1(block);
        block.add_line("return !(lhs == rhs);");
    }
    block.add_line();

    block.add_line("std::ostream& operator<<(std::ostream& os, const " + name + "& object)" + maybe_semicolon);
    if (!forward_definition_only)
    {
        CodeScope scope1(block);
        block.add_line("return os << object.impl();");
    }
    block.add_line();

    return block;
}

CodeBlock create_choice_functions_impl(const Asn1Tree&, const Module&, const Type& type, const std::string& name)
{
    if (!is_choice(type))
    {
        return {};
    }

    CodeBlock         block;
    const ChoiceType& choice = absl::get<ChoiceType>(absl::get<BuiltinType>(type));

    // block.add_block(create_choice_helpers(choice, name, true));
    block.add_block(create_choice_types(name, choice));
    block.add_block(create_choice_holds_alternative(name, choice));
    block.add_block(create_choice_getters(name, choice));

    return block;
}

std::string create_choice_functions(const Asn1Tree& tree, const Module& module, const Assignment& assignment)
{
    return visit_all_types(tree, module, assignment, create_choice_functions_impl).to_string();
}
