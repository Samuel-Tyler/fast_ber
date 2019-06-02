#pragma once

#include <set>
#include <string>
#include <vector>

std::string create_include(const std::string& path);

std::string create_template_definition(const std::set<std::string>& types);

std::string create_template_arguments(const std::set<std::string>& types);

std::string add_namespace(const std::string& name, const std::string& enclosed);
