#pragma once

#include "fast_ber/ber_types/Class.hpp"
#include "fast_ber/ber_types/Construction.hpp"
#include "fast_ber/util/Extract.hpp"

#include "absl/types/span.h"

namespace fast_ber
{

class BerView
{
  public:
    BerView() noexcept : m_valid(false) {}
    BerView(absl::Span<const uint8_t> data) noexcept { assign(data); }
    BerView(const absl::Span<const uint8_t> data, size_t header_length, size_t content_length) noexcept
    {
        assign(data, header_length, content_length);
    }

    void assign(const absl::Span<const uint8_t> data) noexcept;
    void assign(const absl::Span<const uint8_t> data, size_t header_length, size_t content_length) noexcept;

    bool is_valid() const noexcept { return m_valid; }

    Construction construction() const noexcept { return get_construction(m_full_packet[0]); }
    Class        class_() const noexcept { return get_class(m_full_packet[0]); }
    long         tag() const noexcept { return m_tag; }

    absl::Span<const uint8_t>& data() const noexcept { return m_full_packet; }
    absl::Span<const uint8_t>& content() const noexcept { return m_content; }
    size_t                     identifier_length() const noexcept { return m_id_length; }

  private:
    absl::Span<const uint8_t> m_full_packet;
    absl::Span<const uint8_t> m_content;

    size_t m_id_length;
    long   m_tag;
    bool   m_valid;
};

class MutableBerView : public BerView
{
  public:
    MutableBerView() noexcept : BerView() {}
    MutableBerView(absl::Span<const uint8_t> data) noexcept : BerView(data) {}
    MutableBerView(const absl::Span<const uint8_t> a, size_t b, size_t c) : BerView(a, b, c) {}

    absl::Span<uint8_t> content() noexcept
    {
        return absl::MakeSpan(const_cast<uint8_t*>(BerView::content().data()), BerView::content().size());
    }
    absl::Span<const uint8_t> content() const noexcept { return BerView::content(); }
};

inline void BerView::assign(const absl::Span<const uint8_t> data) noexcept
{
    m_valid = false;
    uint64_t content_length;

    size_t tag_length = extract_tag(data, m_tag);
    size_t len_length = extract_length(data, content_length, tag_length);

    size_t header_length   = tag_length + len_length;
    size_t complete_length = header_length + content_length;
    if (tag_length == 0 || len_length == 0 || complete_length > data.length())
    {
        return;
    }

    m_valid       = true;
    m_full_packet = absl::Span<const uint8_t>(data.data(), complete_length);
    m_content     = absl::Span<const uint8_t>(data.data() + header_length, content_length);
    m_id_length   = tag_length;
}

inline void BerView::assign(const absl::Span<const uint8_t> data, size_t header_length, size_t content_length) noexcept
{
    size_t complete_length = header_length + content_length;
    assert(data.length() == complete_length);

    m_full_packet = absl::Span<const uint8_t>(data.data(), complete_length);
    m_content     = absl::Span<const uint8_t>(data.data() + header_length, content_length);
    m_id_length   = extract_tag(data, m_tag);
}

} // namespace fast_ber
