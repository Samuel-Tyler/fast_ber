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
std::vector<Assignment> reorder_assignments(std::vector<Assignment>& assignments, std::vector<Import> imports)
{
    std::unordered_map<std::string, Assignment> assignment_map;
    assignment_map.reserve(assignments.size());
    for (Assignment& assignment : assignments)
    {
        assignment.depends_on           = dependenies(assignment);
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

void resolve_module_dependencies(const std::unordered_map<std::string, Module>& module_map, const std::string& name,
                                 std::unordered_set<std::string>& assigned_names,
                                 std::unordered_set<std::string>& visited_names, std::vector<Module>& ordered_modules)
{
    const auto& module_iter = module_map.find(name);
    if (module_iter == module_map.end())
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

    resolve_module_dependencies(module_map, modules.begin()->module_reference, assigned_modules, visited_modules,
                                ordered_modules);
    if (modules.size() != ordered_modules.size())
    {
        throw std::runtime_error("Failed to re-order modules!");
    }
    return ordered_modules;
}
