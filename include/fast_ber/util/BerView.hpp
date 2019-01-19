#pragma once

#include "fast_ber/ber_types/Class.hpp"
#include "fast_ber/ber_types/Construction.hpp"
#include "fast_ber/util/Create.hpp"
#include "fast_ber/util/Extract.hpp"

#include "absl/types/span.h"

#include <cstring>

namespace fast_ber
{

class BerViewIterator;
class MutableBerViewIterator;
enum class End
{
    end
};

class BerView
{
  public:
    BerView() noexcept = default;
    BerView(absl::Span<const uint8_t> data) noexcept { assign(data); }
    BerView(absl::Span<const uint8_t> data, size_t header_length, size_t content_length) noexcept;

    void assign(absl::Span<const uint8_t> data) noexcept;
    void assign(absl::Span<const uint8_t> ber_data, size_t header_length, size_t content_length) noexcept;
    void assign(absl::Span<const uint8_t> ber_data, Tag tag, size_t tag_length, size_t header_length,
                size_t content_length) noexcept;

    bool         is_valid() const noexcept { return m_valid; }
    Construction construction() const noexcept { return get_construction(m_full_packet[0]); }
    Class        class_() const noexcept { return get_class(m_full_packet[0]); }
    long         tag() const noexcept { return m_tag; }
    size_t       identifier_length() const noexcept { return m_id_length; }
    size_t       header_length() const noexcept { return m_header_length; }

    absl::Span<const uint8_t> content() const noexcept { return absl::MakeSpan(content_data(), m_content_length); }
    const uint8_t*            content_data() const noexcept { return m_full_packet.data() + m_header_length; }
    size_t                    content_length() const noexcept { return m_content_length; }
    absl::Span<const uint8_t> ber() const noexcept { return m_full_packet; }
    const uint8_t*            ber_data() const noexcept { return m_full_packet.data(); }
    size_t                    ber_length() const noexcept { return m_full_packet.length(); }

    BerViewIterator begin() const noexcept;
    BerViewIterator end() const noexcept;

    size_t encode(absl::Span<uint8_t> buffer) const noexcept;
    size_t encode_content_and_length(absl::Span<uint8_t> buffer) const noexcept;

  private:
    absl::Span<const uint8_t> m_full_packet;
    size_t                    m_id_length;     // Also length field offset
    size_t                    m_header_length; // Also content offset
    size_t                    m_content_length;
    Tag                       m_tag;
    bool                      m_valid{false};
};

class MutableBerView : public BerView
{
  public:
    MutableBerView() noexcept : BerView() {}
    MutableBerView(absl::Span<uint8_t> data) noexcept : BerView(data) {}
    MutableBerView(absl::Span<uint8_t> a, size_t b, size_t c) : BerView(a, b, c) {}

    BerViewIterator        cbegin() const noexcept;
    BerViewIterator        cend() const noexcept;
    MutableBerViewIterator begin() noexcept;
    MutableBerViewIterator end() noexcept;

    uint8_t*                  content_data() noexcept { return const_cast<uint8_t*>(BerView::content_data()); }
    const uint8_t*            content_data() const noexcept { return BerView::content_data(); }
    absl::Span<uint8_t>       content() noexcept { return absl::MakeSpan(content_data(), BerView::content().size()); }
    absl::Span<const uint8_t> content() const noexcept { return BerView::content(); }
};

class BerViewIterator
{
  public:
    BerViewIterator(absl::Span<const uint8_t> buffer) : m_buffer(buffer), m_view(buffer)
    {
        if (!m_view.is_valid())
        {
            buffer = absl::Span<const uint8_t>();
        }
    }
    BerViewIterator(End) : m_buffer(), m_view() {}

    BerViewIterator& operator++()
    {
        m_buffer.remove_prefix(m_view.ber_length());
        m_view.assign(m_buffer);
        if (!m_view.is_valid())
        {
            m_buffer = absl::Span<const uint8_t>();
        }
        return *this;
    }

    const BerView& operator*() const { return m_view; }
    const BerView* operator->() const { return &m_view; }

    friend bool operator==(const BerViewIterator& lhs, const BerViewIterator& rhs)
    {
        if (lhs.m_buffer.empty() && rhs.m_buffer.empty())
        {
            return true;
        }
        else
        {
            return lhs.m_buffer.data() == rhs.m_buffer.data();
        }
    }

    friend bool operator!=(const BerViewIterator& lhs, const BerViewIterator& rhs) { return !(lhs == rhs); }

  private:
    absl::Span<const uint8_t> m_buffer;
    BerView                   m_view;
};

class MutableBerViewIterator
{
  public:
    MutableBerViewIterator(absl::Span<uint8_t> buffer) : m_buffer(buffer), m_view(buffer)
    {
        if (!m_view.is_valid())
        {
            buffer = absl::Span<uint8_t>();
        }
    }
    MutableBerViewIterator(End) : m_buffer(), m_view() {}

    MutableBerViewIterator& operator++()
    {
        m_buffer.remove_prefix(m_view.ber_length());
        m_view.assign(m_buffer);
        if (!m_view.is_valid())
        {
            m_buffer = absl::Span<uint8_t>();
        }
        return *this;
    }

    const BerView& operator*() const { return m_view; }
    const BerView* operator->() const { return &m_view; }

    friend bool operator==(const MutableBerViewIterator& lhs, const MutableBerViewIterator& rhs)
    {
        if (lhs.m_buffer.empty() && rhs.m_buffer.empty())
        {
            return true;
        }
        else
        {
            return lhs.m_buffer.data() == rhs.m_buffer.data();
        }
    }

    friend bool operator!=(const MutableBerViewIterator& lhs, const MutableBerViewIterator& rhs)
    {
        return !(lhs == rhs);
    }

  private:
    absl::Span<uint8_t> m_buffer;
    MutableBerView      m_view;
};

inline BerView::BerView(absl::Span<const uint8_t> data, size_t header_length, size_t content_length) noexcept
{
    assign(data, header_length, content_length);
}

inline void BerView::assign(absl::Span<const uint8_t> data) noexcept
{
    m_valid                 = false;
    uint64_t content_length = 0;

    size_t tag_length      = extract_tag(data, m_tag);
    size_t len_length      = extract_length(data, content_length, tag_length);
    size_t header_length   = tag_length + len_length;
    size_t complete_length = header_length + content_length;

    if (tag_length == 0 || len_length == 0 || complete_length > data.length())
    {
        m_id_length      = 0;
        m_header_length  = 0;
        m_content_length = 0;
        return;
    }

    m_full_packet    = absl::MakeSpan(data.data(), complete_length);
    m_id_length      = tag_length;
    m_header_length  = header_length;
    m_content_length = content_length;
    m_valid          = true;
}

inline void BerView::assign(absl::Span<const uint8_t> ber_data, size_t header_length, size_t content_length) noexcept
{
    size_t tag_length = extract_tag(ber_data, m_tag);
    assign(ber_data, m_tag, tag_length, header_length, content_length);
}

inline void BerView::assign(absl::Span<const uint8_t> ber_data, Tag tag, size_t tag_length, size_t header_length,
                            size_t content_length) noexcept
{
    size_t complete_length = header_length + content_length;

    m_full_packet    = absl::MakeSpan(ber_data.data(), complete_length);
    m_id_length      = tag_length;
    m_header_length  = header_length;
    m_content_length = content_length;
    m_tag            = tag;
    m_valid          = true;
}

inline BerViewIterator        BerView::begin() const noexcept { return {content()}; }
inline BerViewIterator        BerView::end() const noexcept { return {End::end}; }
inline BerViewIterator        MutableBerView::cbegin() const noexcept { return BerView::begin(); }
inline BerViewIterator        MutableBerView::cend() const noexcept { return BerView::end(); }
inline MutableBerViewIterator MutableBerView::begin() noexcept { return {content()}; }
inline MutableBerViewIterator MutableBerView::end() noexcept { return {End::end}; }

inline size_t BerView::encode(absl::Span<uint8_t> buffer) const noexcept
{
    if (ber_length() > buffer.size())
    {
        return 0;
    }

    std::memcpy(buffer.data(), ber_data(), ber_length());
    return ber_length();
}

inline size_t BerView::encode_content_and_length(absl::Span<uint8_t> buffer) const noexcept
{
    auto ber = this->ber();
    ber.remove_prefix(identifier_length());
    if (ber.size() > buffer.size())
    {
        return 0;
    }

    std::memcpy(buffer.data(), ber.data(), ber.size());
    return ber.length();
}

} // namespace fast_ber
