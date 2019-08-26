#include "fast_ber/compiler/CompilerTypes.hpp"

SetOfType::SetOfType(bool a, std::unique_ptr<NamedType>&& b, std::unique_ptr<Type>&& c)
    : has_name(a), named_type(std::move(b)), type(std::move(c))
{
}
SetOfType::SetOfType(const SetOfType& rhs)
    : has_name(rhs.has_name), named_type(rhs.named_type ? new NamedType(*rhs.named_type) : nullptr),
      type(rhs.type ? new Type(*rhs.type) : nullptr)
{
}
SetOfType& SetOfType::operator=(const SetOfType& rhs)
{
    has_name   = rhs.has_name;
    named_type = rhs.named_type ? std::unique_ptr<NamedType>(new NamedType(*rhs.named_type)) : nullptr;
    type       = rhs.type ? std::unique_ptr<Type>(new Type(*rhs.type)) : nullptr;
    return *this;
}
SequenceOfType::SequenceOfType(bool a, std::unique_ptr<NamedType>&& b, std::unique_ptr<Type>&& c)
    : has_name(a), named_type(std::move(b)), type(std::move(c))
{
}
SequenceOfType::SequenceOfType(const SequenceOfType& rhs)
    : has_name(rhs.has_name),
      named_type(rhs.named_type ? std::unique_ptr<NamedType>(new NamedType(*rhs.named_type)) : nullptr),
      type(rhs.type ? new Type(*rhs.type) : nullptr)
{
}
SequenceOfType& SequenceOfType::operator=(const SequenceOfType& rhs)
{
    has_name   = rhs.has_name;
    named_type = rhs.named_type ? std::unique_ptr<NamedType>(new NamedType(*rhs.named_type)) : nullptr;
    type       = rhs.type ? std::unique_ptr<Type>(new Type(*rhs.type)) : nullptr;
    return *this;
}

static const std::unordered_set<std::string> reserved_keywords = {"alignas",
                                                                  "alignof",
                                                                  "and",
                                                                  "and_eq",
                                                                  "asm",
                                                                  "atomic_cancel",
                                                                  "atomic_commit",
                                                                  "atomic_noexcept",
                                                                  "auto",
                                                                  "bitand",
                                                                  "bitor",
                                                                  "bool",
                                                                  "break",
                                                                  "case",
                                                                  "catch",
                                                                  "char",
                                                                  "char8_t",
                                                                  "char16_t",
                                                                  "char32_t",
                                                                  "class",
                                                                  "compl",
                                                                  "concept",
                                                                  "const",
                                                                  "consteval",
                                                                  "constexpr",
                                                                  "const_cast",
                                                                  "continue",
                                                                  "co_await",
                                                                  "co_return",
                                                                  "co_yield",
                                                                  "decltype",
                                                                  "default",
                                                                  "delete",
                                                                  "do",
                                                                  "double",
                                                                  "dynamic_cast",
                                                                  "else",
                                                                  "enum",
                                                                  "explicit",
                                                                  "export",
                                                                  "extern",
                                                                  "false",
                                                                  "float",
                                                                  "for",
                                                                  "friend",
                                                                  "goto",
                                                                  "if",
                                                                  "import",
                                                                  "inline",
                                                                  "int",
                                                                  "long",
                                                                  "module",
                                                                  "mutable",
                                                                  "namespace",
                                                                  "new",
                                                                  "noexcept",
                                                                  "not",
                                                                  "not_eq",
                                                                  "nullptr",
                                                                  "operator",
                                                                  "or",
                                                                  "or_eq",
                                                                  "private",
                                                                  "protected",
                                                                  "public",
                                                                  "reflexpr",
                                                                  "register",
                                                                  "reinterpret_cast",
                                                                  "requires",
                                                                  "return",
                                                                  "short",
                                                                  "signed",
                                                                  "sizeof",
                                                                  "static",
                                                                  "static_assert",
                                                                  "static_cast",
                                                                  "struct",
                                                                  "switch",
                                                                  "synchronized",
                                                                  "template",
                                                                  "this",
                                                                  "thread_local",
                                                                  "throw",
                                                                  "true",
                                                                  "try",
                                                                  "typedef",
                                                                  "typeid",
                                                                  "typename",
                                                                  "union",
                                                                  "unsigned",
                                                                  "using",
                                                                  "virtual",
                                                                  "void",
                                                                  "volatile",
                                                                  "wchar_t",
                                                                  "while",
                                                                  "xor",
                                                                  "xor_eq"};

// Switch asn '-' for C++ '_'
// Rename any names which are reserved in C++
std::string santize_name(const std::string& name)
{
    auto copy = name;
    std::replace(copy.begin(), copy.end(), '-', '_');

    if (reserved_keywords.count(copy) > 0)
    {
        return copy + "_";
    }
    return copy;
}

std::string to_string(Class class_)
{
    switch (class_)
    {
    case Class::universal:
        return "universal";
    case Class::application:
        return "application";
    case Class::context_specific:
        return "context_specific";
    case Class::private_:
        return "private_";
    default:
        return "";
    }
}

std::string make_type_optional(const std::string& type) { return "Optional<" + type + ">"; }

bool is_any(const Type& type)
{
    return absl::holds_alternative<BuiltinType>(type) && absl::holds_alternative<AnyType>(absl::get<BuiltinType>(type));
}

bool is_bit_string(const Type& type)
{
    return absl::holds_alternative<BuiltinType>(type) &&
           absl::holds_alternative<BitStringType>(absl::get<BuiltinType>(type));
}

bool is_set(const Type& type)
{
    return absl::holds_alternative<BuiltinType>(type) && absl::holds_alternative<SetType>(absl::get<BuiltinType>(type));
}

bool is_sequence(const Type& type)
{
    return absl::holds_alternative<BuiltinType>(type) &&
           absl::holds_alternative<SequenceType>(absl::get<BuiltinType>(type));
}

bool is_set_of(const Type& type)
{
    return absl::holds_alternative<BuiltinType>(type) &&
           absl::holds_alternative<SetOfType>(absl::get<BuiltinType>(type));
}

bool is_sequence_of(const Type& type)
{
    return absl::holds_alternative<BuiltinType>(type) &&
           absl::holds_alternative<SequenceOfType>(absl::get<BuiltinType>(type));
}

bool is_enumerated(const Type& type)
{
    return absl::holds_alternative<BuiltinType>(type) &&
           absl::holds_alternative<EnumeratedType>(absl::get<BuiltinType>(type));
}

bool is_choice(const Type& type)
{
    return absl::holds_alternative<BuiltinType>(type) &&
           absl::holds_alternative<ChoiceType>(absl::get<BuiltinType>(type));
}

bool is_prefixed(const Type& type)
{
    return absl::holds_alternative<BuiltinType>(type) &&
           absl::holds_alternative<PrefixedType>(absl::get<BuiltinType>(type));
}

bool is_integer(const Type& type)
{
    return absl::holds_alternative<BuiltinType>(type) &&
           absl::holds_alternative<IntegerType>(absl::get<BuiltinType>(type));
}

bool is_oid(const Type& type)
{
    return absl::holds_alternative<BuiltinType>(type) &&
           absl::holds_alternative<ObjectIdentifierType>(absl::get<BuiltinType>(type));
}

bool is_defined(const Type& type) { return absl::holds_alternative<DefinedType>(type); }
