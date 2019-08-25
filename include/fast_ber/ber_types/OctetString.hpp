#pragma once

#include "fast_ber/util/BerLengthAndContentContainer.hpp"
#include "fast_ber/util/DecodeHelpers.hpp"
#include "fast_ber/util/EncodeHelpers.hpp"

#include "absl/strings/string_view.h"
#include "absl/types/span.h"

#include <algorithm>
#include <string>

namespace fast_ber
{

// A string with data stored with ber encoding. Interface mimics std::string
template <typename Identifier = ExplicitIdentifier<UniversalTag::integer>>
class OctetString
{
  public:
    OctetString() noexcept                       = default;
    OctetString(const OctetString& rhs) noexcept = default;
    OctetString(OctetString&& rhs) noexcept      = default;

    OctetString(const char* input_data) noexcept { assign(absl::string_view(input_data)); }
    OctetString(const std::string& input_data) noexcept { assign(absl::string_view(input_data)); }
    explicit OctetString(absl::Span<const uint8_t> input_data) noexcept { assign(input_data); }
    explicit OctetString(const BerView& view) noexcept { assign_ber(view); }
    explicit OctetString(const BerContainer& container) noexcept { assign_ber(container); }

    OctetString& operator=(const BerView& view) noexcept;
    OctetString& operator=(const BerContainer& rhs) noexcept;
    OctetString& operator=(const OctetString& rhs) noexcept;
    OctetString& operator=(const char* rhs) noexcept;
    OctetString& operator=(const std::string& rhs) noexcept;

    OctetString&   operator=(absl::Span<const uint8_t> buffer) noexcept;
    bool           operator==(absl::string_view view) const noexcept { return absl::string_view(*this) == view; }
    bool           operator==(const OctetString& rhs) const noexcept;
    bool           operator==(const std::string& rhs) const noexcept;
    bool           operator==(const char* rhs) const noexcept;
    bool           operator!=(absl::string_view view) const noexcept { return !((*this) == view); }
    bool           operator!=(const OctetString& rhs) const noexcept { return !((*this) == rhs); }
    bool           operator!=(const std::string& rhs) const noexcept { return !((*this) == rhs); }
    bool           operator!=(const char* rhs) const noexcept { return !((*this) == rhs); }
    uint8_t&       operator[](size_t n) noexcept { return data()[n]; }
    const uint8_t& operator[](size_t n) const noexcept { return data()[n]; }
    explicit       operator absl::string_view() const noexcept { return {c_str(), length()}; }

    uint8_t*                  data() noexcept { return m_contents.content_data(); }
    const uint8_t*            data() const noexcept { return m_contents.content_data(); }
    char*                     c_str() noexcept { return reinterpret_cast<char*>(data()); }
    const char*               c_str() const noexcept { return reinterpret_cast<const char*>(data()); }
    absl::Span<uint8_t>       span() noexcept { return m_contents.content(); }
    absl::Span<const uint8_t> span() const noexcept { return m_contents.content(); }
    uint8_t*                  begin() noexcept { return data(); }
    const uint8_t*            begin() const noexcept { return data(); }
    uint8_t*                  end() noexcept { return data() + length(); }
    const uint8_t*            end() const noexcept { return data() + length(); }
    size_t                    size() const noexcept { return m_contents.content_length(); }
    size_t                    length() const noexcept { return m_contents.content_length(); }
    std::string               value() const noexcept { return std::string(c_str(), length()); }

    void   assign(absl::string_view buffer) noexcept;
    void   assign(absl::Span<const uint8_t> buffer) noexcept;
    void   assign(const OctetString& rhs) noexcept;
    size_t assign_ber(const BerView& view) noexcept;
    size_t assign_ber(const BerContainer& container) noexcept;
    size_t assign_ber(absl::Span<const uint8_t> buffer) noexcept;
    void   resize(size_t i) noexcept { m_contents.resize_content(i); }

    EncodeResult encode_content_and_length(absl::Span<uint8_t> buffer) const noexcept;

  private:
    BerLengthAndContentContainer m_contents;
}; // namespace fast_ber

template <typename Identifier>
constexpr inline ExplicitIdentifier<UniversalTag::octet_string> identifier(const OctetString<Identifier>*) noexcept
{
    return {};
}

template <typename Identifier>
inline OctetString<Identifier>& OctetString<Identifier>::operator=(absl::Span<const uint8_t> rhs) noexcept
{
    assign(rhs);
    return *this;
}

template <typename Identifier>
inline OctetString<Identifier>& OctetString<Identifier>::operator=(const char* rhs) noexcept
{
    assign(absl::string_view(rhs));
    return *this;
}

template <typename Identifier>
inline OctetString<Identifier>& OctetString<Identifier>::operator=(const std::string& rhs) noexcept
{
    assign(absl::string_view(rhs));
    return *this;
}

template <typename Identifier>
inline OctetString<Identifier>& OctetString<Identifier>::operator=(const OctetString<Identifier>& rhs) noexcept
{
    assign(rhs);
    return *this;
}

template <typename Identifier>
inline bool OctetString<Identifier>::operator==(const OctetString<Identifier>& rhs) const noexcept
{
    return absl::string_view(*this) == absl::string_view(rhs);
}

template <typename Identifier>
inline bool OctetString<Identifier>::operator==(const std::string& rhs) const noexcept
{
    return absl::string_view(*this) == absl::string_view(rhs);
}

template <typename Identifier>
inline bool OctetString<Identifier>::operator==(const char* rhs) const noexcept
{
    return absl::string_view(*this) == absl::string_view(rhs);
}

template <typename Identifier>
inline std::ostream& operator<<(std::ostream& os, const OctetString<Identifier>& str) noexcept
{
    return os << absl::string_view(str);
}

template <typename Identifier>
inline void OctetString<Identifier>::assign(absl::string_view buffer) noexcept
{
    m_contents.assign_content(absl::MakeSpan(reinterpret_cast<const uint8_t*>(buffer.data()), buffer.length()));
}

template <typename Identifier>
inline void OctetString<Identifier>::assign(absl::Span<const uint8_t> buffer) noexcept
{
    m_contents.assign_content(buffer);
}

template <typename Identifier>
inline void OctetString<Identifier>::assign(const OctetString& rhs) noexcept
{
    m_contents = rhs.m_contents;
}

template <typename Identifier>
inline size_t OctetString<Identifier>::assign_ber(const BerView& view) noexcept
{
    if (!view.is_valid() || view.construction() != Construction::primitive)
    {
        return false;
    }
    return m_contents.assign_ber(view);
}

template <typename Identifier>
inline size_t OctetString<Identifier>::assign_ber(const BerContainer& container) noexcept
{
    if (!container.is_valid() || container.construction() != Construction::primitive)
    {
        return false;
    }
    return m_contents.assign_ber(container);
}

template <typename Identifier>
inline size_t OctetString<Identifier>::assign_ber(absl::Span<const uint8_t> buffer) noexcept
{
    return m_contents.assign_ber(buffer);
}

template <typename Identifier>
inline EncodeResult OctetString<Identifier>::encode_content_and_length(absl::Span<uint8_t> buffer) const noexcept
{
    return m_contents.encode_content_and_length(buffer);
}

template <typename ID = ExplicitIdentifier<UniversalTag::octet_string>>
EncodeResult encode(absl::Span<uint8_t> output, const OctetString<>& object, const ID& id = ID{})
{
    return encode_impl(output, object, id);
}

template <typename ID = ExplicitIdentifier<UniversalTag::octet_string>>
DecodeResult decode(BerViewIterator& input, OctetString<>& output, const ID& id = {}) noexcept
{
    return decode_impl(input, output, id);
}

} // namespace fast_ber
