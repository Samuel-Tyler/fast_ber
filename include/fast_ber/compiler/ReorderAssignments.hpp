#pragma once

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "CompilerTypes.hpp"
#include "Dependencies.hpp"

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
        throw std::runtime_error("Circular dependency when trying to resolve dependencies of " + name);
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
        assignment.depends_on = dependenies(assignment);
        // Any parameterized references are already defined and do not need to be resolved,
        // Therefore remove any names which are defined as parameters from dependancy list
        assignment.depends_on.erase(std::remove_if(assignment.depends_on.begin(), assignment.depends_on.end(),
                                                   [&assignment](const std::string& dependancy) {
                                                       return assignment.parameters.count(dependancy) > 0;
                                                   }),
                                    assignment.depends_on.end());

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
        throw std::runtime_error("Failed to re-order assignments!");
    }
    return ordered_assignments;
}

int unnamed_definition_num = 0;
// Finds any sequence or set types nested within a type
void find_nested_structs(const Type& type, std::vector<NamedType>& nested_structs)
{
    if (is_set(type))
    {
        for (const ComponentType& component : absl::get<SetType>(absl::get<BuiltinType>(type)).components)
        {
            find_nested_structs(component.named_type.type, nested_structs);
        }
    }
    else if (is_sequence(type))
    {
        for (const ComponentType& component : absl::get<SequenceType>(absl::get<BuiltinType>(type)).components)
        {
            find_nested_structs(component.named_type.type, nested_structs);
        }
    }
    else if (is_set_of(type))
    {
        const SetOfType& set_of     = absl::get<SetOfType>(absl::get<BuiltinType>(type));
        const Type&      inner_type = set_of.has_name ? set_of.named_type->type : *set_of.type;
        if (is_set(inner_type))
        {
            nested_structs.push_back(NamedType{"UnnamedSet" + std::to_string(unnamed_definition_num++), inner_type});
        }
        else if (is_sequence(inner_type))
        {
            nested_structs.push_back(
                NamedType{"UnnamedSequence" + std::to_string(unnamed_definition_num++), inner_type});
        }
        find_nested_structs(inner_type, nested_structs);
    }
    else if (is_sequence_of(type))
    {
        const SequenceOfType& sequence_of = absl::get<SequenceOfType>(absl::get<BuiltinType>(type));
        const Type&           inner_type  = sequence_of.has_name ? sequence_of.named_type->type : *sequence_of.type;
        if (is_set(inner_type))
        {
            nested_structs.push_back(NamedType{"UnnamedSet" + std::to_string(unnamed_definition_num++), inner_type});
        }
        else if (is_sequence(inner_type))
        {
            nested_structs.push_back(
                NamedType{"UnnamedSequence" + std::to_string(unnamed_definition_num++), inner_type});
        }
        find_nested_structs(inner_type, nested_structs);
    }
    else if (is_choice(type))
    {
        const ChoiceType& choice = absl::get<ChoiceType>(absl::get<BuiltinType>(type));
        for (const NamedType& choice_selection : choice.choices)
        {
            if (is_set(choice_selection.type))
            {
                nested_structs.push_back(
                    NamedType{"UnnamedSet" + std::to_string(unnamed_definition_num++), choice_selection.type});
            }
            else if (is_sequence(choice_selection.type))
            {
                nested_structs.push_back(
                    NamedType{"UnnamedSequence" + std::to_string(unnamed_definition_num++), choice_selection.type});
            }
            find_nested_structs(choice_selection.type, nested_structs);
        }
    }
}

// structs (Sequence and Sets) cannot be defined within other definitions in C++, due to this nested assignments are
// split into top level assignment statements. Note - assignments are assumed to already have been ordered
std::vector<Assignment> split_nested_structures(const std::vector<Assignment>& assignments)
{
    std::vector<Assignment> split_assignments;
    split_assignments.reserve(assignments.size());

    for (const Assignment& assignment : assignments)
    {
        std::vector<NamedType> nested_structs;
        find_nested_structs(assignment.type, nested_structs);

        for (auto nested_iter = nested_structs.rbegin(); nested_iter != nested_structs.rend(); nested_iter++)
        {
            split_assignments.push_back(Assignment{nested_iter->name, nested_iter->type, {}, {}, {}});
        }

        split_assignments.push_back(assignment);
    }

    return split_assignments;
}

void resolve_module_dependencies(const std::unordered_map<std::string, Module>& module_map, const std::string& name,
                                 std::unordered_set<std::string>& assigned_names,
                                 std::unordered_set<std::string>& visited_names, std::vector<Module>& ordered_modules)
{
    const auto& module_iter = module_map.find(name);
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
        throw std::runtime_error("Circular dependency when trying to resolve dependencies of " + name);
    }

    visited_names.insert(name);

    const Module& module = module_iter->second;
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
