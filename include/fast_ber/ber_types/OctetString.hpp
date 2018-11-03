#pragma once

#include "fast_ber/util/BerContainer.hpp"
#include "fast_ber/util/EncodeResult.hpp"

#include "absl/strings/string_view.h"
#include "absl/types/span.h"

#include <algorithm>
#include <string>

namespace fast_ber
{

// A string with data stored with ber encoding. Interface mimics std::string
class OctetString
{
  public:
    OctetString() noexcept : m_contents() {}
    OctetString(const OctetString& rhs) noexcept = default;
    OctetString(OctetString&& rhs) noexcept      = default;

    OctetString(const char* data) noexcept { assign(absl::string_view(data)); }
    OctetString(const std::string& data) noexcept { assign(absl::string_view(data)); }
    explicit OctetString(absl::Span<const uint8_t> data) noexcept { assign(data); }
    explicit OctetString(const BerView& view) noexcept { assign_ber(view); }
    explicit OctetString(const BerContainer& container) noexcept { assign_ber(container); }

    OctetString& operator=(const BerView& view) noexcept;
    OctetString& operator=(const BerContainer& rhs) noexcept;
    OctetString& operator=(const OctetString& rhs) noexcept;
    OctetString& operator=(const char* rhs) noexcept;
    OctetString& operator=(const std::string& rhs) noexcept;

    OctetString&   operator=(absl::Span<const uint8_t> buffer) noexcept;
    bool           operator==(absl::string_view view) const noexcept { return absl::string_view(*this) == view; }
    bool           operator!=(absl::string_view view) const noexcept { return !((*this) == view); }
    uint8_t&       operator[](size_t n) noexcept { return data()[n]; }
    const uint8_t& operator[](size_t n) const noexcept { return data()[n]; }
                   operator absl::string_view() const noexcept { return absl::string_view(c_str(), length()); }

    uint8_t*                  data() noexcept { return m_contents.content_data(); }
    const uint8_t*            data() const noexcept { return m_contents.content_data(); }
    char*                     c_str() noexcept { return reinterpret_cast<char*>(data()); }
    const char*               c_str() const noexcept { return reinterpret_cast<const char*>(data()); }
    size_t                    size() const noexcept { return m_contents.content_length(); }
    size_t                    length() const noexcept { return m_contents.content_length(); }
    std::string               value() const noexcept { return std::string(c_str(), length()); }
    absl::Span<uint8_t>       span() noexcept { return m_contents.content(); }
    absl::Span<const uint8_t> span() const noexcept { return m_contents.content(); }
    uint8_t*                  begin() noexcept { return data(); }
    const uint8_t*            begin() const noexcept { return data(); }
    uint8_t*                  end() noexcept { return data() + length(); }
    const uint8_t*            end() const noexcept { return data() + length(); }

    void   assign(absl::string_view buffer) noexcept;
    void   assign(absl::Span<const uint8_t> buffer) noexcept;
    void   assign(const OctetString& rhs) noexcept;
    size_t assign_ber(const BerView& view) noexcept;
    size_t assign_ber(const BerContainer& container) noexcept;
    size_t assign_ber(absl::Span<const uint8_t> buffer) noexcept;

    void   resize() noexcept;
    size_t encode_with_new_id(absl::Span<uint8_t> buffer, Class, Tag tag) const noexcept;

  private:
    BerContainer m_contents;
}; // namespace fast_ber

inline EncodeResult encode_with_new_id(absl::Span<uint8_t>& output, const OctetString& object, Class class_, int tag)
{
    size_t encode_length = object.encode_with_new_id(output, class_, tag);
    return EncodeResult{encode_length > 0, encode_length};
}

inline OctetString& OctetString::operator=(absl::Span<const uint8_t> rhs) noexcept
{
    assign(rhs);
    return *this;
}
inline OctetString& OctetString::operator=(const char* rhs) noexcept
{
    assign(absl::string_view(rhs));
    return *this;
}
inline OctetString& OctetString::operator=(const std::string& rhs) noexcept
{
    assign(absl::string_view(rhs));
    return *this;
}
inline OctetString& OctetString::operator=(const OctetString& rhs) noexcept
{
    assign(rhs);
    return *this;
}

inline void OctetString::assign(absl::string_view buffer) noexcept
{
    m_contents.assign_content(absl::MakeSpan(reinterpret_cast<const uint8_t*>(buffer.data()), buffer.length()));
}
inline void OctetString::assign(absl::Span<const uint8_t> buffer) noexcept { m_contents.assign_content(buffer); }
inline void OctetString::assign(const OctetString& rhs) noexcept { m_contents = rhs.m_contents; }

inline size_t OctetString::assign_ber(const BerView& view) noexcept
{
    if (!view.is_valid() || view.construction() != Construction::primitive)
    {
        return false;
    }
    return m_contents.assign_ber(view);
}
inline size_t OctetString::assign_ber(const BerContainer& container) noexcept
{
    if (!container.is_valid() || container.construction() != Construction::primitive)
    {
        return false;
    }
    return m_contents.assign_ber(container);
}
inline size_t OctetString::assign_ber(absl::Span<const uint8_t> buffer) noexcept
{
    return m_contents.assign_ber(buffer);
}

inline size_t OctetString::encode_with_new_id(absl::Span<uint8_t> buffer, Class class_, Tag tag) const noexcept
{
    return m_contents.encode_with_new_id(buffer, Construction::primitive, class_, tag);
}

} // namespace fast_ber
