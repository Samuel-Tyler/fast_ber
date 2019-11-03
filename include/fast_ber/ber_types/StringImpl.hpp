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
    explicit StringImpl(const BerView& view) noexcept { assign_ber(view); }
    explicit StringImpl(const BerContainer& container) noexcept { assign_ber(container); }

    StringImpl& operator=(const BerView& view) noexcept;
    StringImpl& operator=(const BerContainer& rhs) noexcept;
    StringImpl& operator=(const StringImpl& rhs) noexcept;
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

    void   assign(absl::string_view buffer) noexcept;
    void   assign(absl::Span<const uint8_t> buffer) noexcept;
    void   assign(const StringImpl& rhs) noexcept;
    size_t assign_ber(const BerView& view) noexcept;
    size_t assign_ber(const BerContainer& container) noexcept;
    size_t assign_ber(absl::Span<const uint8_t> buffer) noexcept;
    void   resize(size_t i) noexcept { m_contents.resize_content(i); }

    EncodeResult encode_content_and_length(absl::Span<uint8_t> buffer) const noexcept;

    using Id = Identifier;

  private:
    BerLengthAndContentContainer m_contents;
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
StringImpl<tag, Identifier>& StringImpl<tag, Identifier>::operator=(const StringImpl<tag, Identifier>& rhs) noexcept
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
    return os << '"' << absl::string_view(str) << '"';
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
void StringImpl<tag, Identifier>::assign(const StringImpl& rhs) noexcept
{
    m_contents = rhs.m_contents;
}

template <UniversalTag tag, typename Identifier>
size_t StringImpl<tag, Identifier>::assign_ber(const BerView& view) noexcept
{
    if (!view.is_valid() || view.construction() != Construction::primitive)
    {
        return false;
    }
    m_contents.assign(view);
    return 1;
}

template <UniversalTag tag, typename Identifier>
size_t StringImpl<tag, Identifier>::assign_ber(const BerContainer& container) noexcept
{
    if (!container.is_valid() || container.construction() != Construction::primitive)
    {
        return false;
    }
    m_contents.assign(container);
    return 1;
}

template <UniversalTag tag, typename Identifier>
size_t StringImpl<tag, Identifier>::assign_ber(absl::Span<const uint8_t> buffer) noexcept
{
    m_contents.assign(buffer);
    return 1;
}

template <UniversalTag tag, typename Identifier>
EncodeResult StringImpl<tag, Identifier>::encode_content_and_length(absl::Span<uint8_t> buffer) const noexcept
{
    return m_contents.content_and_length_to_raw(buffer);
}

template <UniversalTag tag, typename DefaultIdentifier, typename ID = DefaultIdentifier>
EncodeResult encode(absl::Span<uint8_t> output, const StringImpl<tag, DefaultIdentifier>& object, const ID& id = ID{})
{
    return encode_impl(output, object, id);
}

template <UniversalTag tag, typename DefaultIdentifier, typename ID = DefaultIdentifier>
DecodeResult decode(BerViewIterator& input, StringImpl<tag, DefaultIdentifier>& output, const ID& id = {}) noexcept
{
    return decode_impl(input, output, id);
}

template <UniversalTag tag, typename Identifier>
EncodeResult encode_content_and_length(absl::Span<uint8_t> output, const StringImpl<tag, Identifier>& object) noexcept
{
    return object.encode_content_and_length(output);
}

template <UniversalTag tag, typename Identifier>
DecodeResult decode_content_and_length(BerViewIterator& input, StringImpl<tag, Identifier>& output) noexcept
{
    (void)input;
    (void)output;

    return {};
}

} // namespace fast_ber
