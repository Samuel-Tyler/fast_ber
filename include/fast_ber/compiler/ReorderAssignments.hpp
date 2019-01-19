#pragma once

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

struct AssignmentInfo
{
    std::string              name;
    std::string              assignment;
    std::vector<std::string> depends_on;
};

void resolve_dependencies(const std::unordered_map<std::string, AssignmentInfo>& assignment_infos,
                          const std::string& name, std::unordered_set<std::string>& assigned_names,
                          std::unordered_set<std::string>& visited_names,
                          std::vector<AssignmentInfo>&     ordered_assignment_infos)
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

    const AssignmentInfo& assignment = assign_iter->second;
    for (const std::string& dependency : assignment.depends_on)
    {
        resolve_dependencies(assignment_infos, dependency, assigned_names, visited_names, ordered_assignment_infos);
    }

    ordered_assignment_infos.push_back(assignment);
    assigned_names.insert(name);
}

// Reorder assignments, defining
// Should be able to detect missing assignments and circular dependencies
std::vector<AssignmentInfo> reorder_assignments(const std::unordered_map<std::string, AssignmentInfo>& assignment_infos)
{
    std::unordered_set<std::string> assigned_names;
    std::unordered_set<std::string> visited_names;

    std::vector<AssignmentInfo> ordered_assignment_infos;
    ordered_assignment_infos.reserve(assignment_infos.size());

    for (const std::pair<std::string, AssignmentInfo>& assignment : assignment_infos)
    {
        resolve_dependencies(assignment_infos, assignment.first, assigned_names, visited_names,
                             ordered_assignment_infos);
    }

    if (assignment_infos.size() != ordered_assignment_infos.size())
    {
        throw std::runtime_error("Failed to re-order assignments!");
    }
    return ordered_assignment_infos;
}
