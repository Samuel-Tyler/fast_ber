#pragma once

#include "fast_ber/compiler/CompilerTypes.hpp"

std::string to_string(const std::vector<Assignment>& assignments);
void        check_duplicated_names(const std::vector<Assignment>& assignments, const std::string& module_name);

void resolve_components_of(Asn1Tree& tree);
void resolve_dependencies(const std::unordered_map<std::string, Assignment>& assignment_infos, const std::string& name,
                          std::unordered_set<std::string>& assigned_names,
                          std::unordered_set<std::string>& visited_names,
                          std::vector<Assignment>&         ordered_assignment_infos);

// Reorder assignments, defining
// Should be able to detect missing assignments and circular dependencies
std::vector<Assignment> reorder_assignments(std::vector<Assignment>& assignments, const std::vector<Import>& imports);

// Finds any sequence or set types nested within a type
void find_nested_structs(const Type& type, std::vector<NamedType>& nested_structs);

// Statements such as integer type definitions can introduce new statements, such as value assignments
std::vector<Assignment> split_definitions(const std::vector<Assignment>& assignments);

// structs (Sequence and Sets) cannot be defined within other definitions in C++, due to this nested assignments are
// split into top level assignment statements. Note - assignments are assumed to already have been ordered
std::vector<Assignment> split_nested_structures(const std::vector<Assignment>& assignments);

void resolve_module_dependencies(const std::unordered_map<std::string, Module>& module_map, const std::string& name,
                                 std::unordered_set<std::string>& assigned_names,
                                 std::unordered_set<std::string>& visited_names, std::vector<Module>& ordered_modules);

std::vector<Module> reorder_modules(const std::vector<Module>& modules);
