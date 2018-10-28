#pragma once

#include "fast_ber/util/BerContainer.hpp"

#include "absl/strings/string_view.h"
#include "absl/types/span.h"

#include <algorithm>
#include <string>

namespace fast_ber
{

class OctetString
{
  public:
    explicit OctetString(absl::Span<const uint8_t> buffer) noexcept { assign(buffer); }
    explicit OctetString(absl::Span<const char> buffer) noexcept { assign(buffer); }
    explicit OctetString(const char* str) noexcept { assign(str); }
    OctetString() noexcept : m_contents() {}

    absl::Span<uint8_t>       content() noexcept { return m_contents.content(); }
    absl::Span<const uint8_t> content() const noexcept { return m_contents.content(); }
    uint8_t*                  data() noexcept { return m_contents.content().data(); }
    const uint8_t*            data() const noexcept { return m_contents.content().data(); }
    char*                     c_str() noexcept { return reinterpret_cast<char*>(data()); }
    const char*               c_str() const noexcept { return reinterpret_cast<const char*>(data()); }
    size_t                    length() const noexcept { return m_contents.content().size(); }
    size_t                    size() const noexcept { return m_contents.content().size(); }
    std::string               value() const noexcept { return std::string(c_str(), length()); }

    void resize();

    OctetString& operator=(absl::Span<const uint8_t> buffer) noexcept;
    OctetString& operator=(absl::Span<const char> buffer) noexcept;
    OctetString& operator=(const char* str) noexcept;

    void assign(absl::Span<const uint8_t> buffer) noexcept;
    void assign(absl::Span<const char> buffer) noexcept;
    void assign(const char* buffer) noexcept;

    bool assign_ber(absl::Span<const uint8_t> buffer) noexcept;

    size_t encode(absl::Span<uint8_t> buffer) const noexcept;

  private:
    BerContainer m_contents;
}; // namespace fast_ber

OctetString& OctetString::operator=(absl::Span<const uint8_t> buffer) noexcept
{
    assign(buffer);
    return *this;
}
OctetString& OctetString::operator=(absl::Span<const char> buffer) noexcept
{
    assign(buffer);
    return *this;
}
OctetString& OctetString::operator=(const char* str) noexcept
{
    assign(str);
    return *this;
}

void OctetString::assign(absl::Span<const uint8_t> buffer) noexcept { m_contents.assign_contents(buffer); }
void OctetString::assign(absl::Span<const char> buffer) noexcept
{
    assign(absl::Span<const uint8_t>(reinterpret_cast<const uint8_t*>(buffer.data()), buffer.size()));
}
void OctetString::assign(const char* str) noexcept { assign(absl::string_view(str)); }

} // namespace fast_ber
