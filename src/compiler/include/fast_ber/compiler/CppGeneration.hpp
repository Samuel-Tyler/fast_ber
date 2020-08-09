#pragma once

#include <set>
#include <string>
#include <vector>

#include "absl/strings/string_view.h"

std::string create_include(const std::string& path);

std::string create_template_definition(const std::vector<std::string>& types);

std::string create_template_arguments(const std::vector<std::string>& types);

std::string add_namespace(const std::string& name, const std::string& enclosed);

class CodeBlock
{
  public:
    size_t&       indentation() { return m_indentation; }
    const size_t& indentation() const { return m_indentation; }

    void add_line() { m_content += '\n'; }
    void add_line(const absl::string_view& line)
    {
        m_content += std::string(m_indentation * 4, ' ');
        m_content.insert(m_content.end(), line.begin(), line.end());
        m_content += '\n';
    }

    void add_block(const absl::string_view& block) { m_content.insert(m_content.end(), block.begin(), block.end()); }
    void add_block(const CodeBlock& block) { m_content += block.to_string(); }
    const std::string& to_string() const { return m_content; }

  private:
    size_t      m_indentation = 0;
    std::string m_content     = {};
};

class CodeScope
{
  public:
    CodeScope(CodeBlock& block) : m_block(block)
    {
        m_block.add_line("{");
        m_block.indentation()++;
    }
    CodeScope(CodeScope&) = delete;
    CodeScope(CodeScope&& rhs) : m_block(rhs.m_block) { rhs.active = false; }
    ~CodeScope()
    {
        if (active)
        {
            m_block.indentation()--;
            m_block.add_line("}");
        }
    }

  private:
    bool       active = true;
    CodeBlock& m_block;
};
