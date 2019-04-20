#pragma once

#include "fast_ber/compiler/CompilerTypes.hpp"

std::vector<std::string> depends_on(const BitStringType&);
std::vector<std::string> depends_on(const BooleanType&);
std::vector<std::string> depends_on(const CharacterStringType&);
std::vector<std::string> depends_on(const ChoiceType);
std::vector<std::string> depends_on(const DateType&);
std::vector<std::string> depends_on(const DateTimeType&);
std::vector<std::string> depends_on(const DurationType&);
std::vector<std::string> depends_on(const EmbeddedPDVType&);
std::vector<std::string> depends_on(const EnumeratedType&);
std::vector<std::string> depends_on(const ExternalType&);
std::vector<std::string> depends_on(const GeneralizedTimeType&);
std::vector<std::string> depends_on(const InstanceOfType&);
std::vector<std::string> depends_on(const IntegerType&);
std::vector<std::string> depends_on(const IRIType&);
std::vector<std::string> depends_on(const NullType&);
std::vector<std::string> depends_on(const ObjectClassFieldType&);
std::vector<std::string> depends_on(const ObjectDescriptorType&);
std::vector<std::string> depends_on(const ObjectIdentifierType&);
std::vector<std::string> depends_on(const OctetStringType&);
std::vector<std::string> depends_on(const RealType&);
std::vector<std::string> depends_on(const RelativeIRIType&);
std::vector<std::string> depends_on(const RelativeOIDType&);
std::vector<std::string> depends_on(const SequenceType& sequence);
std::vector<std::string> depends_on(const SequenceOfType& sequence);
std::vector<std::string> depends_on(const SetType& set);
std::vector<std::string> depends_on(const SetOfType& set);
std::vector<std::string> depends_on(const PrefixedType&);
std::vector<std::string> depends_on(const TimeType&);
std::vector<std::string> depends_on(const TimeOfDayType&);
std::vector<std::string> depends_on(const UTCTimeType&);

std::vector<std::string> depends_on(const DefinedType&);
std::vector<std::string> depends_on(const BuiltinType& type);
std::vector<std::string> depends_on(const Type& type);

std::vector<std::string> depends_on(const BitStringType&) { return {}; }
std::vector<std::string> depends_on(const BooleanType&) { return {}; }
std::vector<std::string> depends_on(const CharacterStringType&) { return {}; }
std::vector<std::string> depends_on(const ChoiceType choice)
{
    std::vector<std::string> depends;
    for (const auto& named_type : choice.choices)
    {
        const auto& additional = depends_on(named_type.type);
        depends.insert(depends.end(), additional.begin(), additional.end());
    }
    return depends;
}
std::vector<std::string> depends_on(const DateType&) { return {}; }
std::vector<std::string> depends_on(const DateTimeType&) { return {}; }
std::vector<std::string> depends_on(const DurationType&) { return {}; }
std::vector<std::string> depends_on(const EmbeddedPDVType&) { return {}; }
std::vector<std::string> depends_on(const EnumeratedType&) { return {}; }
std::vector<std::string> depends_on(const ExternalType&) { return {}; }
std::vector<std::string> depends_on(const GeneralizedTimeType&) { return {}; }
std::vector<std::string> depends_on(const InstanceOfType&) { return {}; }
std::vector<std::string> depends_on(const IntegerType&) { return {}; }
std::vector<std::string> depends_on(const IRIType&) { return {}; }
std::vector<std::string> depends_on(const NullType&) { return {}; }
std::vector<std::string> depends_on(const ObjectClassFieldType&) { return {}; }
std::vector<std::string> depends_on(const ObjectDescriptorType&) { return {}; }
std::vector<std::string> depends_on(const ObjectIdentifierType&) { return {}; }
std::vector<std::string> depends_on(const OctetStringType&) { return {}; }
std::vector<std::string> depends_on(const RealType&) { return {}; }
std::vector<std::string> depends_on(const RelativeIRIType&) { return {}; }
std::vector<std::string> depends_on(const RelativeOIDType&) { return {}; }
std::vector<std::string> depends_on(const SequenceType& sequence)
{
    std::vector<std::string> depends;
    for (const ComponentType& component : sequence.components)
    {
        const auto& additional = depends_on(component.named_type.type);
        depends.insert(depends.end(), additional.begin(), additional.end());
    }
    return depends;
}
std::vector<std::string> depends_on(const SequenceOfType& sequence)
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
std::vector<std::string> depends_on(const SetType& set)
{
    std::vector<std::string> depends;
    for (const ComponentType& component : set.components)
    {
        const auto& additional = depends_on(component.named_type.type);
        depends.insert(depends.end(), additional.begin(), additional.end());
    }
    return depends;
}
std::vector<std::string> depends_on(const SetOfType& set)
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
std::vector<std::string> depends_on(const PrefixedType& prefixed_type)
{
    return depends_on(prefixed_type.tagged_type->type);
}
std::vector<std::string> depends_on(const TimeType&) { return {}; }
std::vector<std::string> depends_on(const TimeOfDayType&) { return {}; }
std::vector<std::string> depends_on(const UTCTimeType&) { return {}; }
std::vector<std::string> depends_on(const DefinedType& defined)
{
    std::vector<std::string> depends{defined.name};

    for (const Type& paramater : defined.parameters)
    {
        const std::vector<std::string> param_depends = depends_on(paramater);
        depends.insert(depends.end(), param_depends.begin(), param_depends.end());
    }

    return depends;
}

struct DependsOnHelper
{
    template <typename T>
    std::vector<std::string> operator()(const T& t)
    {
        return depends_on(t);
    }
};
static DependsOnHelper   depends_on_helper;
std::vector<std::string> depends_on(const BuiltinType& type) { return absl::visit(depends_on_helper, type); }
std::vector<std::string> depends_on(const Type& type) { return absl::visit(depends_on_helper, type); }

std::vector<std::string> dependenies(const Type& type) { return depends_on(type); }
std::vector<std::string> dependenies(const Assignment& assignment)
{
    if (absl::holds_alternative<TypeAssignment>(assignment.specific))
    {
        return depends_on(absl::get<TypeAssignment>(assignment.specific).type);
    }
    return {};
}
