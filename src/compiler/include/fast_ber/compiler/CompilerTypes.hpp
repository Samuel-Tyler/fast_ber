#pragma once

#include "fast_ber/compiler/CppGeneration.hpp"

#include "absl/memory/memory.h"
#include "absl/types/optional.h"
#include "absl/types/variant.h"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <list>
#include <map>
#include <memory>
#include <numeric>
#include <set>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

enum class TaggingMode
{
    explicit_,
    implicit,
    automatic
};

enum class Class
{
    universal,
    application,
    context_specific,
    private_,
};

enum class StorageMode
{
    static_,
    small_buffer_optimised,
    dynamic,
};

std::string to_string(Class class_, bool abbreviated);
std::string to_string(StorageMode mode);

struct ComponentType;
struct TaggedType;
using ComponentTypeList = std::vector<ComponentType>;

struct NamedType;
struct NamedNumber
{
    std::string name;
    int64_t     number;
};
struct EnumerationValue
{
    std::string             name;
    absl::optional<int64_t> value;
};

struct AnyType
{
};
struct BitStringType
{
};
struct BooleanType
{
};

enum class UniversalTag
{
    reserved          = 0,
    boolean           = 1,
    integer           = 2,
    bit_string        = 3,
    octet_string      = 4,
    null              = 5,
    object_identifier = 6,
    object_descriptor = 7,
    instance_of       = 8,
    external          = 8,
    real              = 9,
    enumerated        = 10,
    embedded_pdv      = 11,
    utf8_string       = 12,
    relative_oid      = 13,
    sequence          = 16,
    sequence_of       = 16,
    set               = 17,
    set_of            = 17,
    numeric_string    = 18,
    printable_string  = 19,
    teletex_string    = 20,
    t61_string        = 20,
    videotex_string   = 21,
    ia5_string        = 22,
    utc_time          = 23,
    generalized_time  = 24,
    graphic_string    = 25,
    visible_string    = 26,
    iso646_string     = 26,
    general_string    = 27,
    universal_string  = 28,
    character_string  = 29,
    bmp_string        = 30
};

inline std::string to_string(UniversalTag t) noexcept
{
    switch (t)
    {
    case UniversalTag::reserved:
        return "reserved";
    case UniversalTag::boolean:
        return "boolean";
    case UniversalTag::integer:
        return "integer";
    case UniversalTag::bit_string:
        return "bit_string";
    case UniversalTag::octet_string:
        return "octet_string";
    case UniversalTag::null:
        return "null";
    case UniversalTag::object_identifier:
        return "object_identifier";
    case UniversalTag::object_descriptor:
        return "object_descriptor";
    case UniversalTag::instance_of:
        return "instance_of";
    case UniversalTag::real:
        return "real";
    case UniversalTag::enumerated:
        return "enumerated";
    case UniversalTag::embedded_pdv:
        return "embedded_pdv";
    case UniversalTag::utf8_string:
        return "utf8_string";
    case UniversalTag::relative_oid:
        return "relative_oid";
    case UniversalTag::sequence:
        return "sequence";
    case UniversalTag::set:
        return "set";
    case UniversalTag::numeric_string:
        return "numeric_string";
    case UniversalTag::printable_string:
        return "printable_string";
    case UniversalTag::teletex_string:
        return "teletex_string";
    case UniversalTag::videotex_string:
        return "videotex_string";
    case UniversalTag::ia5_string:
        return "ia5_string";
    case UniversalTag::utc_time:
        return "utc_time";
    case UniversalTag::generalized_time:
        return "generalized_time";
    case UniversalTag::graphic_string:
        return "graphic_string";
    case UniversalTag::visible_string:
        return "visible_string";
    case UniversalTag::general_string:
        return "general_string";
    case UniversalTag::universal_string:
        return "universal_string";
    case UniversalTag::character_string:
        return "character_string";
    case UniversalTag::bmp_string:
        return "bmp_string";
    }
    return "Unknown Universal Tag";
}

enum class CharacterStringType
{
    unknown,
    bmp_string,
    general_string,
    graphic_string,
    ia5_string,
    iso646_string,
    numeric_string,
    printable_string,
    teletex_string,
    t61_string,
    universal_string,
    utf8_string,
    videotex_string,
    visible_string,
    character_string,
};

inline std::string to_string(CharacterStringType type)
{
    switch (type)
    {
    case CharacterStringType::bmp_string:
        return "BMPString";
    case CharacterStringType::general_string:
        return "GeneralString";
    case CharacterStringType::graphic_string:
        return "GraphicString";
    case CharacterStringType::ia5_string:
        return "IA5String";
    case CharacterStringType::iso646_string:
        return "ISO646String";
    case CharacterStringType::numeric_string:
        return "NumericString";
    case CharacterStringType::printable_string:
        return "PrintableString";
    case CharacterStringType::teletex_string:
        return "TeletexString";
    case CharacterStringType::t61_string:
        return "T161String";
    case CharacterStringType::universal_string:
        return "UniversalString";
    case CharacterStringType::utf8_string:
        return "UTF8String";
    case CharacterStringType::videotex_string:
        return "VideotexString";
    case CharacterStringType::visible_string:
        return "VisibleString";
    case CharacterStringType::character_string:
        return "CharacterString";
    case CharacterStringType::unknown:
        return "UnknownStringType";
    }
    return "UnknownStringType";
}
inline UniversalTag universal_tag(CharacterStringType type)
{
    switch (type)
    {
    case CharacterStringType::bmp_string:
        return UniversalTag::bmp_string;
    case CharacterStringType::general_string:
        return UniversalTag::general_string;
    case CharacterStringType::graphic_string:
        return UniversalTag::graphic_string;
    case CharacterStringType::ia5_string:
        return UniversalTag::ia5_string;
    case CharacterStringType::iso646_string:
        return UniversalTag::iso646_string;
    case CharacterStringType::numeric_string:
        return UniversalTag::numeric_string;
    case CharacterStringType::printable_string:
        return UniversalTag::printable_string;
    case CharacterStringType::teletex_string:
        return UniversalTag::teletex_string;
    case CharacterStringType::t61_string:
        return UniversalTag::t61_string;
    case CharacterStringType::universal_string:
        return UniversalTag::universal_string;
    case CharacterStringType::utf8_string:
        return UniversalTag::utf8_string;
    case CharacterStringType::videotex_string:
        return UniversalTag::videotex_string;
    case CharacterStringType::visible_string:
        return UniversalTag::visible_string;
    case CharacterStringType::character_string:
        return UniversalTag::character_string;
    case CharacterStringType::unknown:
        return UniversalTag::reserved;
    }
    return UniversalTag::reserved;
}

struct ChoiceType;
struct DateType
{
};
struct DateTimeType
{
};
struct DurationType
{
};
struct EmbeddedPDVType
{
};
struct EnumeratedType
{
    std::vector<EnumerationValue> enum_values;
    bool                          accept_anything;
};
struct ExternalType
{
};
struct GeneralizedTimeType
{
};
struct InstanceOfType
{
};
struct IntegerType
{
    std::vector<NamedNumber> named_numbers;
};
struct IRIType
{
};
struct NullType
{
};
struct ObjectClassFieldType;
struct ObjectDescriptorType
{
};
struct ObjectIdentifierType
{
};
struct OctetStringType
{
};
struct RealType
{
};
struct RelativeIRIType
{
};
struct RelativeOIDType
{
};
struct SequenceType
{
    ComponentTypeList components;
};
struct SequenceOfType;
struct SetType
{
    ComponentTypeList components;
};
struct SetOfType;

// unique ptr used to avoid circular reference on stack
struct PrefixedType
{
    PrefixedType() = default;
    PrefixedType(const TaggedType& type) : tagged_type(absl::make_unique<TaggedType>(type)) {}
    PrefixedType(const PrefixedType& rhs)
    {
        if (rhs.tagged_type)
        {
            tagged_type = absl::make_unique<TaggedType>(*rhs.tagged_type);
        }
    }
    PrefixedType& operator=(const PrefixedType& rhs)
    {
        if (rhs.tagged_type)
        {
            tagged_type = absl::make_unique<TaggedType>(*rhs.tagged_type);
        }
        return *this;
    }
    std::unique_ptr<TaggedType> tagged_type;
};
struct TimeType
{
};
struct TimeOfDayType
{
};
struct UTCTimeType
{
};
struct DefinedType;

using BuiltinType =
    absl::variant<AnyType, BitStringType, BooleanType, CharacterStringType, ChoiceType, DateType, DateTimeType,
                  DurationType, EmbeddedPDVType, EnumeratedType, ExternalType, GeneralizedTimeType, InstanceOfType,
                  IntegerType, IRIType, NullType, ObjectClassFieldType, ObjectDescriptorType, ObjectIdentifierType,
                  OctetStringType, RealType, RelativeIRIType, RelativeOIDType, SequenceType, SequenceOfType, SetType,
                  SetOfType, PrefixedType, TimeType, TimeOfDayType, UTCTimeType>;
using Type = absl::variant<BuiltinType, DefinedType>;

struct DefinedType
{
    absl::optional<std::string> module_reference;
    std::string                 type_reference;
    std::vector<Type>           parameters;
};
struct SequenceOfType
{
    // Unique pointers used to prevent circular references
    bool                       has_name;
    std::unique_ptr<NamedType> named_type;
    std::unique_ptr<Type>      type;
    StorageMode                storage = StorageMode::small_buffer_optimised;

    SequenceOfType() = default;
    SequenceOfType(bool, std::unique_ptr<NamedType>&&, std::unique_ptr<Type>&&);
    SequenceOfType(const SequenceOfType& rhs);
    SequenceOfType& operator=(const SequenceOfType& rhs);
};
struct SetOfType
{
    // Unique pointers used to prevent circular references
    bool                       has_name;
    std::unique_ptr<NamedType> named_type;
    std::unique_ptr<Type>      type;
    StorageMode                storage = StorageMode::small_buffer_optimised;

    SetOfType() = default;
    SetOfType(bool, std::unique_ptr<NamedType>&&, std::unique_ptr<Type>&&);
    SetOfType(const SetOfType& rhs);
    SetOfType& operator=(const SetOfType& rhs);
};

struct ChoiceType
{
    std::vector<NamedType> choices;
    StorageMode            storage;
};

struct DefinedValue
{
    std::string reference;
};

struct ObjectClassFieldType
{
    DefinedType              referenced_object_class;
    std::vector<std::string> fieldnames;
};

struct BitStringValue
{
    std::string value;
};

struct HexStringValue
{
    std::string value;
};

struct CharStringValue
{
    std::string value;
};

struct BooleanValue
{
    bool value;
};

struct NullValue
{
};

struct TimeValue
{
    std::string time;
};

struct Value
{
    absl::variant<std::vector<Value>, int64_t, std::string, NamedNumber, BitStringValue, HexStringValue,
                  CharStringValue, DefinedValue, BooleanValue, NullValue, TimeValue, double>
        value_selection;
};

struct NamedType
{
    std::string name;
    Type        type;
};

struct NamedValue
{
    std::string name;
    Value       type;
};

struct ComponentType
{
    NamedType             named_type;
    bool                  is_optional;
    absl::optional<Value> default_value;
    absl::optional<Type>  components_of;
    StorageMode           optional_storage;
};

struct Tag
{
    Class class_;
    int   tag_number;
};

struct TaggedType
{
    Tag         tag;
    TaggingMode tagging_mode;
    Type        type;
};

struct TypeAssignment
{
    Type type;
};

struct ValueAssignment
{
    Type  type;
    Value value;
};

struct TypeField
{
};

struct FixedTypeValueField
{
    Type type;
};

struct ClassField
{
    std::string                                   name;
    absl::variant<TypeField, FixedTypeValueField> field;
};

struct ObjectClassAssignment
{
    std::vector<ClassField> fields;
};

struct ObjectSetAssignment
{
};

struct Parameter
{
    absl::optional<Type> governor;
    std::string          reference;
};

struct Dependency
{
    std::string                 name;
    absl::optional<std::string> module_reference;

    template <typename H>
    friend H AbslHashValue(H h, const Dependency& d)
    {
        return H::combine(std::move(h), d.name, d.module_reference);
    }
    bool operator==(const Dependency& rhs) const
    {
        return name == rhs.name && module_reference == rhs.module_reference;
    }
};

struct Assignment
{
    std::string                                                                                name;
    absl::variant<TypeAssignment, ValueAssignment, ObjectClassAssignment, ObjectSetAssignment> specific;
    std::vector<Dependency>                                                                    depends_on;
    std::vector<Parameter>                                                                     parameters;
};

struct Import
{
    std::string              module_reference;
    std::vector<std::string> imported_types;
    std::vector<std::string> imported_values;
};

struct Export
{
};

struct Module
{
    std::string             module_reference;
    TaggingMode             tagging_default;
    std::vector<Export>     exports;
    std::vector<Import>     imports;
    std::vector<Assignment> assignments;
};

struct Asn1Tree
{
    std::vector<Module> modules;
};

struct Identifier
{
    Class        class_     = Class::universal;
    int64_t      tag_number = 0;
    UniversalTag universal  = UniversalTag::reserved;

    Identifier() = default;
    explicit Identifier(Tag tag) : class_(tag.class_), tag_number(tag.tag_number) {}
    explicit Identifier(Class c, int64_t tag) : class_(c), tag_number(tag) {}
    explicit Identifier(UniversalTag tag)
        : class_(Class::universal), tag_number(static_cast<int64_t>(tag)), universal(tag)
    {
    }

    std::string name() const
    {
        if (universal == UniversalTag::reserved)
        {
            return "Id<" + to_string(class_, true) + ", " + std::to_string(tag_number) + ">";
        }
        else
        {
            return "ExplicitId<UniversalTag::" + to_string(universal) + ">";
        }
    }

    template <typename H>
    friend H AbslHashValue(H h, const Identifier& i)
    {
        return H::combine(std::move(h), i.class_, i.tag_number, i.universal);
    }
    bool operator==(const Identifier& rhs) const
    {
        return class_ == rhs.class_ && tag_number == rhs.tag_number && universal == rhs.universal;
    }
};

struct TaggingInfo
{
    absl::optional<Identifier> outer_tag;
    Identifier                 inner_tag;
    std::vector<Identifier>    choice_ids;
    bool                       is_default_tagged;

    std::string name() const
    {
        if (outer_tag)
        {
            return "DoubleId<" + outer_tag->name() + ", " + inner_tag.name() + ">";
        }
        if (choice_ids.size() > 0)
        {
            std::string res      = "ChoiceId<";
            bool        is_first = true;
            for (const Identifier& id : choice_ids)
            {
                if (!is_first)
                {
                    res += ", ";
                }

                res += id.name();
                is_first = false;
            }
            return res + ">";
        }
        else
        {
            return inner_tag.name();
        }
    }

    std::vector<Identifier> outer_tags() const
    {
        if (outer_tag)
        {
            return {*outer_tag};
        }
        if (choice_ids.size() > 0)
        {
            return choice_ids;
        }
        return {inner_tag};
    }
};

struct ObjectIdComponentValue
{
    absl::optional<std::string> name;
    absl::optional<int64_t>     value;
};

struct ObjectIdComponents
{
    ObjectIdComponents(const Value& value)
    {
        if (!absl::holds_alternative<std::vector<Value>>(value.value_selection))
        {
            throw std::runtime_error("Failed to interpret value as object identifier");
        }
        const std::vector<Value>& value_list = absl::get<std::vector<Value>>(value.value_selection);
        components.reserve(value_list.size());
        for (const Value& component : value_list)
        {
            if (absl::holds_alternative<DefinedValue>(component.value_selection))
            {
                const std::string& name = absl::get<DefinedValue>(component.value_selection).reference;
                components.push_back(ObjectIdComponentValue{name, absl::nullopt});
            }
            else if (absl::holds_alternative<std::string>(component.value_selection))
            {
                const std::string& name = absl::get<std::string>(component.value_selection);
                components.push_back(ObjectIdComponentValue{name, absl::nullopt});
            }
            else if (absl::holds_alternative<int64_t>(component.value_selection))
            {
                const int64_t& number = absl::get<int64_t>(component.value_selection);
                components.push_back(ObjectIdComponentValue{absl::nullopt, number});
            }
            else if (absl::holds_alternative<NamedNumber>(component.value_selection))
            {
                const NamedNumber& named_number = absl::get<NamedNumber>(component.value_selection);
                components.push_back(ObjectIdComponentValue{named_number.name, named_number.number});
            }
            else
            {
                throw std::runtime_error("Failed to interpret value as object identifier");
            }
        }
    }
    std::vector<ObjectIdComponentValue> components;
};

// Switch asn '-' for C++ '_'
// Rename any names which are reserved in C++
std::string santize_name(const std::string& name);

std::string make_type_optional(const std::string& type, StorageMode mode);

bool is_bit_string(const Type& type);
bool is_set(const Type& type);
bool is_sequence(const Type& type);
bool is_set_of(const Type& type);
bool is_sequence_of(const Type& type);
bool is_enumerated(const Type& type);
bool is_choice(const Type& type);
bool is_prefixed(const Type& type);
bool is_integer(const Type& type);
bool is_octet_string(const Type& type);
bool is_boolean(const Type& type);
bool is_oid(const Type& type);
bool is_defined(const Type& type);
bool is_generated(const Type& type);
bool is_choice_set_or_sequence(const Type& type);

struct Context;

std::string gen_anon_member_name();
