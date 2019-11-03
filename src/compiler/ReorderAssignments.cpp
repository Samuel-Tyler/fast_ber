#include "fast_ber/compiler/ReorderAssignments.hpp"
#include "fast_ber/compiler/Dependencies.hpp"
#include "fast_ber/compiler/ResolveType.hpp"

#include <iostream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

std::string to_string(const std::vector<Assignment>& assignments)
{
    std::string str;
    for (const Assignment& assignment : assignments)
    {
        if (assignment.name.empty())
        {
            str += "<missing name>\n";
        }
        else
        {
            str += assignment.name + '\n';
        }
    }
    return str;
}

void check_duplicated_names(const std::vector<Assignment>& assignments, const std::string& module_name)
{
    std::unordered_set<std::string> defined_names;
    for (const Assignment& assignment : assignments)
    {
        if (defined_names.count(assignment.name) > 0)
        {
            throw std::runtime_error("Error: Duplicated name \"" + assignment.name + "\" in module \"" + module_name +
                                     '\"');
        }
        defined_names.insert(assignment.name);
    }
}

void resolve_components_of(Asn1Tree& tree)
{
    for (Module& module : tree.modules)
    {
        for (Assignment& assignemnt : module.assignments)
        {
            if (absl::holds_alternative<TypeAssignment>(assignemnt.specific))
            {
                TypeAssignment& type_assignment = absl::get<TypeAssignment>(assignemnt.specific);
                if (is_sequence(type_assignment.type))
                {
                    SequenceType& sequence = absl::get<SequenceType>(absl::get<BuiltinType>(type_assignment.type));
                    for (auto iter = sequence.components.begin(); iter != sequence.components.end(); iter++)
                    {
                        if (iter->components_of)
                        {
                            if (is_defined(*iter->components_of))
                            {
                                const DefinedType& defined    = absl::get<DefinedType>(*iter->components_of);
                                const Type&        inheretied = type(resolve(tree, module.module_reference, defined));
                                if (is_sequence(inheretied))
                                {
                                    const SequenceType& inheretied_sequence =
                                        absl::get<SequenceType>(absl::get<BuiltinType>(inheretied));

                                    const size_t offset = std::distance(sequence.components.begin(), iter);
                                    sequence.components.insert(iter, inheretied_sequence.components.begin(),
                                                               inheretied_sequence.components.end());

                                    iter = sequence.components.begin();
                                    std::advance(iter, offset);
                                    std::advance(iter, inheretied_sequence.components.size());
                                }
                                else if (is_set(inheretied))
                                {
                                    const SetType& inheretied_set =
                                        absl::get<SetType>(absl::get<BuiltinType>(inheretied));

                                    const size_t offset = std::distance(sequence.components.begin(), iter);
                                    sequence.components.insert(iter, inheretied_set.components.begin(),
                                                               inheretied_set.components.end());

                                    iter = sequence.components.begin();
                                    std::advance(iter, offset);
                                    std::advance(iter, inheretied_set.components.size());
                                }
                            }
                            else if (is_sequence(*iter->components_of))
                            {
                                const SequenceType& inheretied_sequence =
                                    absl::get<SequenceType>(absl::get<BuiltinType>(*iter->components_of));

                                const size_t offset = std::distance(sequence.components.begin(), iter);
                                sequence.components.insert(iter, inheretied_sequence.components.begin(),
                                                           inheretied_sequence.components.end());

                                iter = sequence.components.begin();
                                std::advance(iter, offset);
                                std::advance(iter, inheretied_sequence.components.size());
                            }
                            else if (is_set(*iter->components_of))
                            {
                                const SetType& inheretied_set =
                                    absl::get<SetType>(absl::get<BuiltinType>(*iter->components_of));

                                const size_t offset = std::distance(sequence.components.begin(), iter);
                                sequence.components.insert(iter, inheretied_set.components.begin(),
                                                           inheretied_set.components.end());

                                iter = sequence.components.begin();
                                std::advance(iter, offset);
                                std::advance(iter, inheretied_set.components.size());
                            }
                            else
                            {
                                throw std::runtime_error("Strange type when resolving SEQUENCE OF in type" +
                                                         assignemnt.name);
                            }
                        }
                    }
                    sequence.components.erase(
                        std::remove_if(sequence.components.begin(), sequence.components.end(),
                                       [](ComponentType& component) { return component.components_of; }),
                        sequence.components.end());
                }
            }
        }
    }
}

void resolve_dependencies(const std::unordered_map<std::string, Assignment>& assignment_infos, const std::string& name,
                          std::unordered_set<std::string>& assigned_names,
                          std::unordered_set<std::string>& visited_names,
                          std::vector<Assignment>&         ordered_assignment_infos)
{
    const auto& assign_iter = assignment_infos.find(name);
    if (assign_iter == assignment_infos.end())
    {
        throw std::runtime_error("Reference to undefined type: " + name);
    }

    if (assigned_names.count(name) == 1)
    {
        // Already assigned
        return;
    }

    if (visited_names.count(name) == 1)
    {
        std::cerr << "Warning: Circular dependency when trying to resolve dependencies of " << name << std::endl;
        return;
    }

    visited_names.insert(name);

    const Assignment& assignment = assign_iter->second;
    for (const std::string& dependency : assignment.depends_on)
    {
        resolve_dependencies(assignment_infos, dependency, assigned_names, visited_names, ordered_assignment_infos);
    }

    ordered_assignment_infos.push_back(assignment);
    assigned_names.insert(name);
}

// Reorder assignments, defining
// Should be able to detect missing assignments and circular dependencies
std::vector<Assignment> reorder_assignments(std::vector<Assignment>& assignments, const std::vector<Import>& imports)
{
    std::unordered_map<std::string, Assignment> assignment_map;
    assignment_map.reserve(assignments.size());
    for (Assignment& assignment : assignments)
    {
        assignment.depends_on           = dependencies(assignment);
        assignment_map[assignment.name] = assignment;
    }

    std::unordered_set<std::string> assigned_names;
    std::unordered_set<std::string> visited_names;

    for (const Import& import : imports)
    {
        for (const std::string& import_name : import.imports)
        {
            assignment_map[import_name] = Assignment{};
            assigned_names.insert(import_name);
        }
    }

    std::vector<Assignment> ordered_assignments;
    ordered_assignments.reserve(assignments.size());

    for (const std::pair<std::string, Assignment>& assignment : assignment_map)
    {
        resolve_dependencies(assignment_map, assignment.first, assigned_names, visited_names, ordered_assignments);
    }

    if (assignments.size() != ordered_assignments.size())
    {
        throw std::runtime_error("Failed to re-order assignments, Unordered assigments:\n" + to_string(assignments) +
                                 " ordered assignments:\n" + to_string(ordered_assignments));
    }
    return ordered_assignments;
}

int unnamed_definition_num = 0;
// Finds any sequence or set types nested within a type
void find_nested_structs(const Module& module, Type& type, std::vector<NamedType>& nested_structs)
{
    if (is_set(type))
    {
        for (ComponentType& component : absl::get<SetType>(absl::get<BuiltinType>(type)).components)
        {
            if (is_enumerated(component.named_type.type))
            {
                const std::string& name = "UnnamedEnum" + std::to_string(unnamed_definition_num++);
                nested_structs.push_back(NamedType{name, component.named_type.type});
                component.named_type.type = DefinedType{module.module_reference, name, {}};
            }
            else
            {
                find_nested_structs(module, component.named_type.type, nested_structs);
            }
        }
    }
    else if (is_sequence(type))
    {
        for (ComponentType& component : absl::get<SequenceType>(absl::get<BuiltinType>(type)).components)
        {
            if (is_enumerated(component.named_type.type))
            {
                const std::string& name = "UnnamedEnum" + std::to_string(unnamed_definition_num++);
                nested_structs.push_back(NamedType{name, component.named_type.type});
                component.named_type.type = DefinedType{module.module_reference, name, {}};
            }
            else
            {
                find_nested_structs(module, component.named_type.type, nested_structs);
            }
        }
    }
    else if (is_set_of(type))
    {
        SetOfType& set_of     = absl::get<SetOfType>(absl::get<BuiltinType>(type));
        Type&      inner_type = set_of.has_name ? set_of.named_type->type : *set_of.type;
        find_nested_structs(module, inner_type, nested_structs);

        if (is_set(inner_type))
        {
            const std::string& name = "UnnamedSet" + std::to_string(unnamed_definition_num++);
            nested_structs.push_back(NamedType{name, inner_type});
            inner_type = DefinedType{module.module_reference, name, {}};
        }
        else if (is_sequence(inner_type))
        {
            const std::string& name = "UnnamedSequence" + std::to_string(unnamed_definition_num++);
            nested_structs.push_back(NamedType{name, inner_type});
            inner_type = DefinedType{module.module_reference, name, {}};
        }
        else if (is_enumerated(inner_type))
        {
            const std::string& name = "UnnamedEnum" + std::to_string(unnamed_definition_num++);
            nested_structs.push_back(NamedType{name, inner_type});
            inner_type = DefinedType{module.module_reference, name, {}};
        }
    }
    else if (is_sequence_of(type))
    {
        SequenceOfType& sequence_of = absl::get<SequenceOfType>(absl::get<BuiltinType>(type));
        Type&           inner_type  = sequence_of.has_name ? sequence_of.named_type->type : *sequence_of.type;

        find_nested_structs(module, inner_type, nested_structs);

        if (is_set(inner_type))
        {
            const std::string& name = "UnnamedSet" + std::to_string(unnamed_definition_num++);
            nested_structs.push_back(NamedType{name, inner_type});
            inner_type = DefinedType{module.module_reference, name, {}};
        }
        else if (is_sequence(inner_type))
        {
            const std::string& name = "UnnamedSequence" + std::to_string(unnamed_definition_num++);
            nested_structs.push_back(NamedType{name, inner_type});
            inner_type = DefinedType{module.module_reference, name, {}};
        }
        else if (is_enumerated(inner_type))
        {
            const std::string& name = "UnnamedEnum" + std::to_string(unnamed_definition_num++);
            nested_structs.push_back(NamedType{name, inner_type});
            inner_type = DefinedType{module.module_reference, name, {}};
        }
    }
    else if (is_choice(type))
    {
        ChoiceType& choice = absl::get<ChoiceType>(absl::get<BuiltinType>(type));
        for (NamedType& choice_selection : choice.choices)
        {
            Type& inner_type = choice_selection.type;
            find_nested_structs(module, choice_selection.type, nested_structs);

            if (is_set(inner_type))
            {
                const std::string& name = "UnnamedSet" + std::to_string(unnamed_definition_num++);
                nested_structs.push_back(NamedType{name, inner_type});
                inner_type = DefinedType{module.module_reference, name, {}};
            }
            else if (is_sequence(inner_type))
            {
                const std::string& name = "UnnamedSequence" + std::to_string(unnamed_definition_num++);
                nested_structs.push_back(NamedType{name, inner_type});
                inner_type = DefinedType{module.module_reference, name, {}};
            }
            else if (is_enumerated(inner_type))
            {
                const std::string& name = "UnnamedEnum" + std::to_string(unnamed_definition_num++);
                nested_structs.push_back(NamedType{name, inner_type});
                inner_type = DefinedType{module.module_reference, name, {}};
            }
        }
    }
    else if (is_prefixed(type))
    {
        Type& inner_type = absl::get<PrefixedType>(absl::get<BuiltinType>(type)).tagged_type->type;
        find_nested_structs(module, inner_type, nested_structs);

        if (is_set(inner_type))
        {
            const std::string& name = "UnnamedSet" + std::to_string(unnamed_definition_num++);
            nested_structs.push_back(NamedType{name, inner_type});
            inner_type = DefinedType{module.module_reference, name, {}};
        }
        else if (is_sequence(inner_type))
        {
            const std::string& name = "UnnamedSequence" + std::to_string(unnamed_definition_num++);
            nested_structs.push_back(NamedType{name, inner_type});
            inner_type = DefinedType{module.module_reference, name, {}};
        }
        else if (is_enumerated(inner_type))
        {
            const std::string& name = "UnnamedEnum" + std::to_string(unnamed_definition_num++);
            nested_structs.push_back(NamedType{name, inner_type});
            inner_type = DefinedType{module.module_reference, name, {}};
        }
    }
}

// Statements such as integer type definitions can introduce new statements, such as value assignments
std::vector<Assignment> split_definitions(const std::vector<Assignment>& assignments)
{
    return assignments;
    std::vector<Assignment> split_assignments;
    split_assignments.reserve(assignments.size());

    for (const Assignment& assignment : assignments)
    {
        if (absl::holds_alternative<TypeAssignment>(assignment.specific))
        {
            const TypeAssignment& type_assign = absl::get<TypeAssignment>(assignment.specific);
            if (is_integer(type_assign.type))
            {
                const IntegerType& integer = absl::get<IntegerType>(absl::get<BuiltinType>(type_assign.type));
                for (const NamedNumber& named_number : integer.named_numbers)
                {
                    split_assignments.push_back(
                        Assignment{named_number.name,
                                   ValueAssignment{BuiltinType{IntegerType{}}, Value{named_number.number}},
                                   {},
                                   {}});
                }
            }
        }
        split_assignments.push_back(assignment);
    }

    return split_assignments;
}

// structs (Sequence and Sets) cannot be defined within other definitions in C++, due to this nested assignments are
// split into top level assignment statements. Note - assignments are assumed to already have been ordered
std::vector<Assignment> split_nested_structures(const Module& module)
{
    std::vector<Assignment> split_assignments;
    split_assignments.reserve(module.assignments.size());

    for (Assignment assignment : module.assignments)
    {
        if (absl::holds_alternative<TypeAssignment>(assignment.specific))
        {
            std::vector<NamedType> nested_structs;
            find_nested_structs(module, absl::get<TypeAssignment>(assignment.specific).type, nested_structs);

            for (auto nested_iter = nested_structs.rbegin(); nested_iter != nested_structs.rend(); nested_iter++)
            {
                split_assignments.push_back(Assignment{nested_iter->name, TypeAssignment{nested_iter->type}, {}, {}});
            }
        }
        split_assignments.push_back(assignment);
    }

    return split_assignments;
}

void resolve_module_dependencies(const std::unordered_map<std::string, Module>& module_map, const std::string& name,
                                 std::unordered_set<std::string>& assigned_names,
                                 std::unordered_set<std::string>& visited_names, std::vector<Module>& ordered_modules)
{
    const auto&   module_iter = module_map.find(name);
    const Module& module      = module_iter->second;

    if (module_iter == module_map.end())
    {
        throw std::runtime_error("Reference to undefined module: " + name);
    }

    if (assigned_names.count(name) == 1)
    {
        // Already assigned
        return;
    }

    if (visited_names.count(name) == 1)
    {
        std::cerr << "Warning: Circular dependency when trying to resolve dependencies of " << name << std::endl;
        ordered_modules.push_back(module);
        assigned_names.insert(name);
        return;
    }

    visited_names.insert(name);

    for (const Import& import : module.imports)
    {
        resolve_module_dependencies(module_map, import.module_reference, assigned_names, visited_names,
                                    ordered_modules);
    }

    ordered_modules.push_back(module);
    assigned_names.insert(name);
}

std::vector<Module> reorder_modules(const std::vector<Module>& modules)
{
    std::vector<Module> ordered_modules;
    ordered_modules.reserve(modules.size());

    std::unordered_map<std::string, Module> module_map;
    for (const Module& module : modules)
    {
        module_map[module.module_reference] = module;
    }

    std::unordered_set<std::string> assigned_modules;
    std::unordered_set<std::string> visited_modules;

    for (const Module& module : modules)
    {
        resolve_module_dependencies(module_map, module.module_reference, assigned_modules, visited_modules,
                                    ordered_modules);
    }
    if (modules.size() != ordered_modules.size())
    {
        throw std::runtime_error("Failed to re-order modules!");
    }
    return ordered_modules;
}
