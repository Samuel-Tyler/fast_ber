#pragma once

#include "fast_ber/util/DecodeHelpers.hpp"
#include "fast_ber/util/EncodeHelpers.hpp"
#include "fast_ber/util/FixedIdBerContainer.hpp"

#include "absl/strings/string_view.h"
#include "absl/types/span.h"

#include <algorithm>
#include <cctype>
#include <string>

namespace fast_ber
{

// A string with data stored with ber encoding. Interface mimics std::string
template <UniversalTag tag, typename Identifier>
class StringImpl
{
  public:
    StringImpl() noexcept = default;
    template <UniversalTag tag2, typename Identifier2>
    StringImpl(const StringImpl<tag2, Identifier2>& rhs) noexcept : m_contents(rhs.m_contents)
    {
    }
    template <UniversalTag tag2, typename Identifier2>
    StringImpl(StringImpl<tag2, Identifier2>&& rhs) noexcept : m_contents(std::move(rhs.m_contents))
    {
    }

    StringImpl(const char* input_data) noexcept { assign(absl::string_view(input_data)); }
    StringImpl(const std::string& input_data) noexcept { assign(absl::string_view(input_data)); }
    explicit StringImpl(absl::Span<const uint8_t> input_data) noexcept { assign(input_data); }
    explicit StringImpl(BerView view) noexcept { decode(view); }

    template <UniversalTag tag2, typename Identifier2>
    StringImpl& operator=(const StringImpl<tag2, Identifier2>& rhs) noexcept;
    StringImpl& operator=(const char* rhs) noexcept;
    StringImpl& operator=(const std::string& rhs) noexcept;

    StringImpl&    operator=(absl::Span<const uint8_t> buffer) noexcept;
    bool           operator==(absl::string_view view) const noexcept { return absl::string_view(*this) == view; }
    bool           operator==(const StringImpl& rhs) const noexcept;
    bool           operator==(const std::string& rhs) const noexcept;
    bool           operator==(const char* rhs) const noexcept;
    bool           operator!=(absl::string_view view) const noexcept { return !((*this) == view); }
    bool           operator!=(const StringImpl& rhs) const noexcept { return !((*this) == rhs); }
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

    template <UniversalTag tag2, typename Identifier2>
    void   assign(const StringImpl<tag2, Identifier2>& rhs) noexcept;
    void   assign(absl::string_view buffer) noexcept;
    void   assign(absl::Span<const uint8_t> buffer) noexcept;
    void   resize(size_t i) noexcept { m_contents.resize_content(i); }

    template <UniversalTag tag2, typename Identifier2>
    friend class StringImpl;

    using AsnId = Identifier;

    size_t       encoded_length() const noexcept { return m_contents.ber().length(); }
    EncodeResult encode(absl::Span<uint8_t> output) const noexcept { return m_contents.encode(output); }
    DecodeResult decode(BerView input) noexcept { return m_contents.decode(input); }

  private:
    FixedIdBerContainer<Identifier> m_contents;
}; // namespace fast_ber

template <UniversalTag tag, typename Identifier>
struct IdentifierType<StringImpl<tag, Identifier>>
{
    using type = Identifier;
};

template <UniversalTag tag, typename Identifier>
StringImpl<tag, Identifier>& StringImpl<tag, Identifier>::operator=(absl::Span<const uint8_t> rhs) noexcept
{
    assign(rhs);
    return *this;
}

template <UniversalTag tag, typename Identifier>
StringImpl<tag, Identifier>& StringImpl<tag, Identifier>::operator=(const char* rhs) noexcept
{
    assign(absl::string_view(rhs));
    return *this;
}

template <UniversalTag tag, typename Identifier>
StringImpl<tag, Identifier>& StringImpl<tag, Identifier>::operator=(const std::string& rhs) noexcept
{
    assign(absl::string_view(rhs));
    return *this;
}

template <UniversalTag tag, typename Identifier>
template <UniversalTag tag2, typename Identifier2>
StringImpl<tag, Identifier>& StringImpl<tag, Identifier>::operator=(const StringImpl<tag2, Identifier2>& rhs) noexcept
{
    assign(rhs);
    return *this;
}

template <UniversalTag tag, typename Identifier>
bool StringImpl<tag, Identifier>::operator==(const StringImpl<tag, Identifier>& rhs) const noexcept
{
    return absl::string_view(*this) == absl::string_view(rhs);
}

template <UniversalTag tag, typename Identifier>
bool StringImpl<tag, Identifier>::operator==(const std::string& rhs) const noexcept
{
    return absl::string_view(*this) == absl::string_view(rhs);
}

template <UniversalTag tag, typename Identifier>
bool StringImpl<tag, Identifier>::operator==(const char* rhs) const noexcept
{
    return absl::string_view(*this) == absl::string_view(rhs);
}

template <UniversalTag tag, typename Identifier>
std::ostream& operator<<(std::ostream& os, const StringImpl<tag, Identifier>& str) noexcept
{
    if (std::all_of(str.begin(), str.end(), [](char c) { return std::isprint(c); }))
    {
        return os << '"' << absl::string_view(str) << '"';
    }
    else
    {
        return os << "\"binary data\"";
    }
}

template <UniversalTag tag, typename Identifier>
void StringImpl<tag, Identifier>::assign(absl::string_view buffer) noexcept
{
    m_contents.assign_content(absl::MakeSpan(reinterpret_cast<const uint8_t*>(buffer.data()), buffer.length()));
}

template <UniversalTag tag, typename Identifier>
void StringImpl<tag, Identifier>::assign(absl::Span<const uint8_t> buffer) noexcept
{
    m_contents.assign_content(buffer);
}

template <UniversalTag tag, typename Identifier>
template <UniversalTag tag2, typename Identifier2>
void StringImpl<tag, Identifier>::assign(const StringImpl<tag2, Identifier2>& rhs) noexcept
{
    assign(rhs.span());
}

} // namespace fast_ber
