#pragma once

#include "fast_ber/compiler/CompilerTypes.hpp"

#include "absl/container/flat_hash_set.h"

std::vector<Dependency> depends_on(const AnyType&);
std::vector<Dependency> depends_on(const BitStringType&);
std::vector<Dependency> depends_on(const BooleanType&);
std::vector<Dependency> depends_on(const CharacterStringType&);
std::vector<Dependency> depends_on(const ChoiceType);
std::vector<Dependency> depends_on(const DateType&);
std::vector<Dependency> depends_on(const DateTimeType&);
std::vector<Dependency> depends_on(const DurationType&);
std::vector<Dependency> depends_on(const EmbeddedPDVType&);
std::vector<Dependency> depends_on(const EnumeratedType&);
std::vector<Dependency> depends_on(const ExternalType&);
std::vector<Dependency> depends_on(const GeneralizedTimeType&);
std::vector<Dependency> depends_on(const InstanceOfType&);
std::vector<Dependency> depends_on(const IntegerType&);
std::vector<Dependency> depends_on(const IRIType&);
std::vector<Dependency> depends_on(const NullType&);
std::vector<Dependency> depends_on(const ObjectClassFieldType&);
std::vector<Dependency> depends_on(const ObjectDescriptorType&);
std::vector<Dependency> depends_on(const ObjectIdentifierType&);
std::vector<Dependency> depends_on(const OctetStringType&);
std::vector<Dependency> depends_on(const RealType&);
std::vector<Dependency> depends_on(const RelativeIRIType&);
std::vector<Dependency> depends_on(const RelativeOIDType&);
std::vector<Dependency> depends_on(const SequenceType& sequence);
std::vector<Dependency> depends_on(const SequenceOfType& sequence);
std::vector<Dependency> depends_on(const SetType& set);
std::vector<Dependency> depends_on(const SetOfType& set);
std::vector<Dependency> depends_on(const PrefixedType&);
std::vector<Dependency> depends_on(const TimeType&);
std::vector<Dependency> depends_on(const TimeOfDayType&);
std::vector<Dependency> depends_on(const UTCTimeType&);
std::vector<Dependency> depends_on(const DefinedType&);
std::vector<Dependency> depends_on(const BuiltinType& type);
std::vector<Dependency> depends_on(const Type& type);

std::vector<Dependency> dependencies(const Type& type);
std::vector<Dependency> dependencies(const Assignment& assignment);

void get_dependencies_recursive(const std::string& type_name, const std::string& module_name,
                                const std::unordered_map<std::string, Assignment>& assignment_map,
                                absl::flat_hash_set<Dependency>&                   depends);
