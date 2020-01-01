#include "fast_ber/compiler/Dependencies.hpp"

std::vector<Dependency> depends_on(const BitStringType&) { return {}; }
std::vector<Dependency> depends_on(const BooleanType&) { return {}; }
std::vector<Dependency> depends_on(const CharacterStringType&) { return {}; }
std::vector<Dependency> depends_on(const ChoiceType choice)
{
    std::vector<Dependency> depends;
    for (const auto& named_type : choice.choices)
    {
        const auto& additional = depends_on(named_type.type);
        depends.insert(depends.end(), additional.begin(), additional.end());
    }
    return depends;
}
std::vector<Dependency> depends_on(const AnyType&) { return {}; }
std::vector<Dependency> depends_on(const DateType&) { return {}; }
std::vector<Dependency> depends_on(const DateTimeType&) { return {}; }
std::vector<Dependency> depends_on(const DurationType&) { return {}; }
std::vector<Dependency> depends_on(const EmbeddedPDVType&) { return {}; }
std::vector<Dependency> depends_on(const EnumeratedType&) { return {}; }
std::vector<Dependency> depends_on(const ExternalType&) { return {}; }
std::vector<Dependency> depends_on(const GeneralizedTimeType&) { return {}; }
std::vector<Dependency> depends_on(const InstanceOfType&) { return {}; }
std::vector<Dependency> depends_on(const IntegerType&) { return {}; }
std::vector<Dependency> depends_on(const IRIType&) { return {}; }
std::vector<Dependency> depends_on(const NullType&) { return {}; }
std::vector<Dependency> depends_on(const ObjectClassFieldType&) { return {}; }
std::vector<Dependency> depends_on(const ObjectDescriptorType&) { return {}; }
std::vector<Dependency> depends_on(const ObjectIdentifierType&) { return {}; }
std::vector<Dependency> depends_on(const OctetStringType&) { return {}; }
std::vector<Dependency> depends_on(const RealType&) { return {}; }
std::vector<Dependency> depends_on(const RelativeIRIType&) { return {}; }
std::vector<Dependency> depends_on(const RelativeOIDType&) { return {}; }
std::vector<Dependency> depends_on(const SequenceType& sequence)
{
    std::vector<Dependency> depends;
    for (const ComponentType& component : sequence.components)
    {
        const auto& additional = depends_on(component.named_type.type);
        depends.insert(depends.end(), additional.begin(), additional.end());
    }
    return depends;
}
std::vector<Dependency> depends_on(const SequenceOfType& sequence)
{
    if (sequence.has_name)
    {
        return depends_on(sequence.named_type->type);
    }
    else
    {
        return depends_on(*sequence.type);
    };
}
std::vector<Dependency> depends_on(const SetType& set)
{
    std::vector<Dependency> depends;
    for (const ComponentType& component : set.components)
    {
        const auto& additional = depends_on(component.named_type.type);
        depends.insert(depends.end(), additional.begin(), additional.end());
    }
    return depends;
}
std::vector<Dependency> depends_on(const SetOfType& set)
{
    if (set.has_name)
    {
        return depends_on(set.named_type->type);
    }
    else
    {
        return {depends_on(*set.type)};
    };
}
std::vector<Dependency> depends_on(const PrefixedType& prefixed_type)
{
    return depends_on(prefixed_type.tagged_type->type);
}
std::vector<Dependency> depends_on(const TimeType&) { return {}; }
std::vector<Dependency> depends_on(const TimeOfDayType&) { return {}; }
std::vector<Dependency> depends_on(const UTCTimeType&) { return {}; }
std::vector<Dependency> depends_on(const DefinedType& defined)
{
    std::vector<Dependency> depends{{defined.type_reference}};

    for (const Type& paramater : defined.parameters)
    {
        const std::vector<Dependency> param_depends = depends_on(paramater);
        depends.insert(depends.end(), param_depends.begin(), param_depends.end());
    }

    return depends;
}

struct DependsOnHelper
{
    template <typename T>
    std::vector<Dependency> operator()(const T& t)
    {
        return depends_on(t);
    }
};
static DependsOnHelper  depends_on_helper;
std::vector<Dependency> depends_on(const BuiltinType& type) { return absl::visit(depends_on_helper, type); }
std::vector<Dependency> depends_on(const Type& type) { return absl::visit(depends_on_helper, type); }
std::vector<Dependency> depends_on(const Value& value)
{
    if (absl::holds_alternative<DefinedValue>(value.value_selection))
    {
        return {{absl::get<DefinedValue>(value.value_selection).reference}};
    };
    return {};
}

std::vector<Dependency> dependencies(const Type& type) { return depends_on(type); }
std::vector<Dependency> dependencies(const Assignment& assignment)
{
    if (absl::holds_alternative<TypeAssignment>(assignment.specific))
    {
        return depends_on(absl::get<TypeAssignment>(assignment.specific).type);
    }
    else if (absl::holds_alternative<ValueAssignment>(assignment.specific))
    {
        auto type_depends  = depends_on(absl::get<ValueAssignment>(assignment.specific).type);
        auto value_depends = depends_on(absl::get<ValueAssignment>(assignment.specific).value);

        type_depends.insert(type_depends.end(), value_depends.begin(), value_depends.end());
        return type_depends;
    }
    return {};
}
