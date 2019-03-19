#pragma once

#include "fast_ber/util/BerContainer.hpp"
#include "fast_ber/util/EncodeHelpers.hpp"

#include "absl/container/inlined_vector.h"
#include "absl/types/span.h"

namespace fast_ber
{

// Owning container of a ber packet's content and length. Contents are always valid. Memory is stored in a small
// buffer optimized vector. Can be constructed directly from encoded ber memory (assign_ber) or by specifiyng the
// desired contents (assign_contents)
class BerLengthAndContentContainer
{
  public:
    BerLengthAndContentContainer() noexcept : m_data{0}, m_content_offset{1} {}
    BerLengthAndContentContainer(const BerView& view) noexcept { assign_ber(view); }
    BerLengthAndContentContainer(const BerLengthAndContentContainer& container) noexcept
        : m_data{container.m_data}, m_content_offset{container.m_content_offset}
    {
    }
    BerLengthAndContentContainer(
        absl::Span<const uint8_t> data,
        ConstructionMethod        method = ConstructionMethod::construct_with_provided_content) noexcept;

    bool operator==(const BerLengthAndContentContainer& rhs) const noexcept { return m_data == rhs.m_data; }
    bool operator!=(const BerLengthAndContentContainer& rhs) const noexcept { return !(*this == rhs); }
    BerLengthAndContentContainer& operator=(const BerView& view) noexcept;
    BerLengthAndContentContainer& operator=(const BerLengthAndContentContainer& container) noexcept;
    size_t                        assign_ber(const BerView& view) noexcept;
    size_t                        assign_ber(const BerContainer& container) noexcept;
    size_t                        assign_ber(const absl::Span<const uint8_t> data) noexcept;
    void                          assign_content(const absl::Span<const uint8_t> input_content) noexcept;
    void                          resize_content(size_t size) noexcept;

    absl::Span<uint8_t>       content() noexcept { return absl::MakeSpan(content_data(), content_length()); }
    absl::Span<const uint8_t> content() const noexcept { return absl::MakeSpan(content_data(), content_length()); }
    uint8_t*                  content_data() noexcept { return m_data.data() + m_content_offset; }
    const uint8_t*            content_data() const noexcept { return m_data.data() + m_content_offset; }
    size_t                    content_length() const noexcept { return m_data.size() - m_content_offset; }

    EncodeResult encode_content_and_length(absl::Span<uint8_t> buffer) const noexcept;

  private:
    absl::InlinedVector<uint8_t, 100> m_data;
    size_t                            m_content_offset;
};

inline BerLengthAndContentContainer& BerLengthAndContentContainer::operator=(const BerView& view) noexcept
{
    assign_ber(view);
    return *this;
}

inline BerLengthAndContentContainer& BerLengthAndContentContainer::
                                     operator=(const BerLengthAndContentContainer& container) noexcept
{
    this->m_data           = container.m_data;
    this->m_content_offset = container.m_content_offset;
    return *this;
}

inline size_t BerLengthAndContentContainer::assign_ber(const BerView& view) noexcept
{
    if (!view.is_valid())
    {
        return false;
    }

    m_content_offset                       = view.header_length() - view.identifier_length();
    const size_t length_and_content_length = view.content_length() + m_content_offset;

    m_data.assign(view.ber_data() + view.identifier_length(),
                  view.ber_data() + view.identifier_length() + length_and_content_length);
    return view.identifier_length() + length_and_content_length;
}

inline size_t BerLengthAndContentContainer::assign_ber(const BerContainer& container) noexcept
{
    return assign_ber(container.view());
}

inline size_t BerLengthAndContentContainer::assign_ber(const absl::Span<const uint8_t> data) noexcept
{
    return assign_ber(BerView(data));
}

inline void BerLengthAndContentContainer::assign_content(const absl::Span<const uint8_t> input_content) noexcept
{
    m_data.resize(15);
    m_content_offset = encode_length(absl::Span<uint8_t>(m_data), input_content.size());

    m_data.resize(m_content_offset + input_content.size());
    std::copy(input_content.data(), input_content.end(), m_data.data() + m_content_offset);
}

inline void BerLengthAndContentContainer::resize_content(size_t size) noexcept
{
    size_t old_content_offset = m_content_offset;
    size_t old_size           = m_data.size() - m_content_offset;
    m_content_offset          = encoded_length_length(size);

    m_data.resize(m_content_offset + size);
    if (m_content_offset != old_content_offset)
    {
        std::memmove(m_data.data() + m_content_offset, m_data.data() + old_content_offset, std::min(old_size, size));
    }
    encode_length(absl::MakeSpan(m_data), size);
}

inline EncodeResult BerLengthAndContentContainer::encode_content_and_length(absl::Span<uint8_t> buffer) const noexcept
{
    if (buffer.size() < m_data.size())
    {
        return EncodeResult{false, 0};
    }

    std::memcpy(buffer.data(), m_data.data(), m_data.size());
    return EncodeResult{true, m_data.size()};
}

} // namespace fast_ber
