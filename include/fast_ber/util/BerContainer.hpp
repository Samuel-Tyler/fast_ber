#pragma once

#include "fast_ber/util/BerView.hpp"
#include "fast_ber/util/Create.hpp"

#include "absl/container/inlined_vector.h"
#include "absl/types/span.h"

namespace fast_ber
{

enum class ConstructionMethod
{
    construct_with_provided_content,
    construct_from_ber_packet
};

// Owning container of a ber packet. Contents may or may not be valid. Memory is stored in a small buffer optimized
// vector. Can be constructed directly from encoded ber memory (assign_ber) or by specifiyng the desired contents
// (assign_contents)
class BerContainer
{
  public:
    BerContainer() noexcept;
    BerContainer(const BerView& view) noexcept { assign_ber(view); }
    BerContainer(const BerContainer& container) noexcept { assign_ber(container); }
    BerContainer(absl::Span<const uint8_t> data, ConstructionMethod method) noexcept;
    BerContainer(Construction construction, Class class_, int tag, absl::Span<const uint8_t> content) noexcept;

    BerContainer& operator=(const BerView& view) noexcept;
    BerContainer& operator=(const BerContainer& container) noexcept;
    size_t        assign_ber(const BerView& view) noexcept;
    size_t        assign_ber(const BerContainer& container) noexcept;
    size_t        assign_ber(const absl::Span<const uint8_t> data) noexcept;
    void          assign_content(const absl::Span<const uint8_t> content) noexcept;
    void assign_content(Construction construction, Class class_, int tag, absl::Span<const uint8_t> content) noexcept;
    void resize_content(size_t size);

    bool         is_valid() const noexcept { return m_view.is_valid(); }
    Construction construction() const noexcept { return m_view.construction(); }
    Class        class_() const noexcept { return m_view.class_(); }
    long         tag() const noexcept { return m_view.tag(); }
    size_t       identifier_length() const noexcept { return m_view.identifier_length(); }
    size_t       header_length() const noexcept { return m_view.header_length(); }

    absl::Span<uint8_t>       content() noexcept { return m_view.content(); }
    absl::Span<const uint8_t> content() const noexcept { return m_view.content(); }
    uint8_t*                  content_data() noexcept { return m_view.content_data(); }
    const uint8_t*            content_data() const noexcept { return m_view.content_data(); }
    size_t                    content_length() const noexcept { return m_view.content_length(); }

    absl::Span<const uint8_t> ber() const noexcept { return m_view.ber(); }
    const uint8_t*            ber_data() const noexcept { return m_view.ber_data(); }
    size_t                    ber_length() const noexcept { return m_view.ber_length(); }

    const BerView& view() const noexcept { return m_view; }

    size_t encode(absl::Span<uint8_t> buffer) const noexcept { return m_view.encode(buffer); }
    size_t encode_content_and_length(absl::Span<uint8_t> buffer) const noexcept;

  private:
    absl::InlinedVector<uint8_t, 100> m_data;
    MutableBerView                    m_view;
};

inline BerContainer::BerContainer() noexcept
    : m_data{0x80, 0x00}, m_view(absl::MakeSpan(m_data.begin(), m_data.size()), 0, 1, 2, 0)
{
}

inline BerContainer::BerContainer(absl::Span<const uint8_t> data, ConstructionMethod method) noexcept
    : m_data(data.begin(), data.end()), m_view(absl::MakeSpan(m_data.begin(), m_data.size()))
{
    if (method == ConstructionMethod::construct_with_provided_content)
    {
        assign_content(data);
    }
    else if (method == ConstructionMethod::construct_from_ber_packet)
    {
        assign_ber(data);
    }
    else
    {
        assert(0);
    }
}

inline BerContainer::BerContainer(Construction construction, Class class_, int tag,
                                  absl::Span<const uint8_t> content) noexcept
{
    assign_content(construction, class_, tag, content);
}

inline BerContainer& BerContainer::operator=(const BerView& view) noexcept
{
    assign_ber(view);
    return *this;
}

inline BerContainer& BerContainer::operator=(const BerContainer& container) noexcept
{
    assign_ber(container);
    return *this;
}

inline size_t BerContainer::assign_ber(const BerView& view) noexcept
{
    if (!view.is_valid())
    {
        return false;
    }

    m_data.assign(view.ber().begin(), view.ber().end());
    m_view.assign(absl::MakeSpan(m_data.data(), m_data.size()), view.tag(), view.identifier_length(),
                  view.header_length(), view.content_length());

    return view.ber_length();
}

inline size_t BerContainer::assign_ber(const BerContainer& container) noexcept { return assign_ber(container.view()); }
inline size_t BerContainer::assign_ber(const absl::Span<const uint8_t> data) noexcept
{
    m_data.assign(data.begin(), data.end());
    m_view.assign(absl::MakeSpan(m_data.begin(), m_data.size()));

    if (!m_view.is_valid())
    {
        return 0;
    }
    return ber_length();
}

inline void BerContainer::assign_content(const absl::Span<const uint8_t> content) noexcept
{
    m_data.resize(15 + content.size());
    m_data[0]            = 0x80; // No identifier provided, use a tag of 0
    size_t header_length = 1 + create_length(absl::Span<uint8_t>(m_data.data() + 1, m_data.size() - 1), content.size());

    m_data.resize(header_length + content.length());
    std::copy(content.data(), content.end(), m_data.data() + header_length);
    m_view.assign(m_data, 0, 1, header_length, content.size());

    assert(m_view.is_valid());
}

inline void BerContainer::assign_content(Construction construction, Class class_, int tag,
                                         absl::Span<const uint8_t> content) noexcept
{
    m_data.resize(30 + content.size());
    size_t header_length =
        create_header(absl::MakeSpan(m_data.data(), m_data.size()), construction, class_, tag, content.size());

    m_data.resize(header_length + content.length());
    std::copy(content.data(), content.end(), m_data.data() + header_length);
    m_view.assign(m_data, header_length, content.size());

    assert(m_view.is_valid());
}

inline void BerContainer::resize_content(size_t size)
{
    std::array<uint8_t, 20> length_buffer;

    size_t old_header_length = view().header_length();
    size_t old_size          = view().content_length();
    size_t length_offset     = view().identifier_length();
    size_t length_length     = create_length(absl::MakeSpan(length_buffer), size);
    size_t header_length     = length_offset + length_length;
    size_t complete_length   = header_length + size;

    m_data.resize(complete_length);
    std::memmove(m_data.data() + header_length, m_data.data() + old_header_length, std::min(old_size, size));
    std::copy(length_buffer.data(), length_buffer.data() + length_length, m_data.data() + length_offset);
    m_view.assign(absl::MakeSpan(m_data.data(), m_data.size()), header_length, size);

    assert(length_length != 0);
    assert(m_view.is_valid());
}

inline size_t BerContainer::encode_content_and_length(absl::Span<uint8_t> buffer) const noexcept
{
    return m_view.encode_content_and_length(buffer);
}

} // namespace fast_ber
