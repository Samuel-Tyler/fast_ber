#pragma once

#include <iosfwd>
#include <set>
#include <string>
#include <vector>

#include "absl/strings/string_view.h"
#include "absl/types/variant.h"

std::string create_include(const std::string& path);

std::string create_template_definition(const std::vector<std::string>& types);

std::string create_template_arguments(const std::vector<std::string>& types);

std::string add_namespace(const std::string& name, const std::string& enclosed);

std::string make_type_name(std::string name, absl::string_view parent_name = {});

struct CodeBlockContents;

class CodeBlock
{
  public:
    size_t&       indentation() { return m_indentation; }
    const size_t& indentation() const { return m_indentation; }

    void add_line(const absl::string_view& block = {});
    void add_block(CodeBlock block);

    const std::ostream& stream(std::ostream& os, std::size_t indentation) const;
    std::string         to_string() const;

  private:
    std::size_t                    m_indentation = 0;
    std::vector<CodeBlockContents> m_content;
};

struct CodeBlockContents
{
    std::size_t                           indentation;
    absl::variant<std::string, CodeBlock> data;
};

inline void CodeBlock::add_line(const absl::string_view& block)
{
    m_content.push_back(CodeBlockContents{m_indentation, std::string(block)});
}
inline void CodeBlock::add_block(CodeBlock block)
{
    m_content.push_back(CodeBlockContents{m_indentation, std::move(block)});
}

class CodeScope
{
  public:
    CodeScope(CodeBlock& block, bool semicolon = false) : m_block(block), m_semicolon(semicolon)
    {
        m_block.add_line("{");
        m_block.indentation()++;
    }
    CodeScope(CodeScope&) = delete;
    CodeScope(CodeScope&& rhs) : m_block(rhs.m_block), m_active(rhs.m_active), m_semicolon(rhs.m_semicolon)
    {
        rhs.m_active = false;
    }
    ~CodeScope()
    {
        if (m_active)
        {
            m_block.indentation()--;
            m_block.add_line(m_semicolon ? "};" : "}");
        }
    }

  private:
    bool       m_active = true;
    CodeBlock& m_block;
    bool       m_semicolon;
};
