#pragma once

#include "fast_ber/ber_types/Class.hpp"
#include "fast_ber/ber_types/Construction.hpp"
#include "fast_ber/util/EncodeIdentifiers.hpp"
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
    BerView(absl::Span<const uint8_t> input_ber_data) noexcept { assign(input_ber_data); }
    BerView(absl::Span<const uint8_t> input_ber_data, size_t input_header_length, size_t input_content_length) noexcept;
    BerView(absl::Span<const uint8_t> input_ber_data, Tag input_tag, size_t input_tag_length,
            size_t input_header_length, size_t content_length) noexcept;

    void assign(absl::Span<const uint8_t> input_ber_data) noexcept;
    void assign(absl::Span<const uint8_t> input_ber_data, size_t input_header_length,
                size_t input_content_length) noexcept;
    void assign(absl::Span<const uint8_t> input_ber_data, Tag input_tag, size_t input_tag_length,
                size_t input_header_length, size_t content_length) noexcept;

    bool         is_valid() const noexcept { return m_valid; }
    Construction construction() const noexcept { return get_construction(m_data[0]); }
    Class        class_() const noexcept { return get_class(m_data[0]); }
    Tag          tag() const noexcept { return m_tag; }
    size_t       identifier_length() const noexcept { return m_id_length; }
    size_t       header_length() const noexcept { return m_header_length; }

    absl::Span<const uint8_t> content() const noexcept { return absl::MakeSpan(content_data(), m_content_length); }
    const uint8_t*            content_data() const noexcept { return m_data + m_header_length; }
    size_t                    content_length() const noexcept { return m_content_length; }
    absl::Span<const uint8_t> ber() const noexcept { return absl::MakeSpan(ber_data(), ber_length()); }
    const uint8_t*            ber_data() const noexcept { return m_data; }
    size_t                    ber_length() const noexcept { return m_header_length + m_content_length; }

    BerViewIterator begin() const noexcept;
    BerViewIterator end() const noexcept;

    size_t encode(absl::Span<uint8_t> buffer) const noexcept;
    size_t encode_content_and_length(absl::Span<uint8_t> buffer) const noexcept;

  private:
    const uint8_t* m_data;
    Tag            m_tag;
    size_t         m_id_length;     // Also length field offset
    size_t         m_header_length; // Also content offset
    size_t         m_content_length;
    bool           m_valid{false};
};

class MutableBerView : public BerView
{
  public:
    MutableBerView() noexcept : BerView() {}
    MutableBerView(absl::Span<uint8_t> data) noexcept : BerView(data) {}
    MutableBerView(absl::Span<uint8_t> a, size_t b, size_t c) : BerView(a, b, c) {}
    MutableBerView(absl::Span<const uint8_t> a, Tag b, size_t c, size_t d, size_t e) noexcept : BerView(a, b, c, d, e)
    {
    }

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
    BerViewIterator(absl::Span<const uint8_t> buffer) noexcept : m_buffer(buffer), m_view(buffer)
    {
        if (!m_view.is_valid())
        {
            buffer = absl::Span<const uint8_t>();
        }
    }
    BerViewIterator(End) noexcept : m_buffer(), m_view() {}

    BerViewIterator& operator++() noexcept
    {
        m_buffer.remove_prefix(m_view.ber_length());
        m_view.assign(m_buffer);
        if (!m_view.is_valid())
        {
            m_buffer = absl::Span<const uint8_t>();
        }
        return *this;
    }

    const BerView& operator*() const noexcept { return m_view; }
    const BerView* operator->() const noexcept { return &m_view; }

    friend bool operator==(const BerViewIterator& lhs, const BerViewIterator& rhs) noexcept
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

    friend bool operator!=(const BerViewIterator& lhs, const BerViewIterator& rhs) noexcept { return !(lhs == rhs); }

  private:
    absl::Span<const uint8_t> m_buffer;
    BerView                   m_view;
};

class MutableBerViewIterator
{
  public:
    MutableBerViewIterator(absl::Span<uint8_t> buffer) noexcept : m_buffer(buffer), m_view(buffer)
    {
        if (!m_view.is_valid())
        {
            buffer = absl::Span<uint8_t>();
        }
    }
    MutableBerViewIterator(End) noexcept : m_buffer(), m_view() {}

    MutableBerViewIterator& operator++() noexcept
    {
        m_buffer.remove_prefix(m_view.ber_length());
        m_view.assign(m_buffer);
        if (!m_view.is_valid())
        {
            m_buffer = absl::Span<uint8_t>();
        }
        return *this;
    }

    const BerView& operator*() const noexcept { return m_view; }
    const BerView* operator->() const noexcept { return &m_view; }

    friend bool operator==(const MutableBerViewIterator& lhs, const MutableBerViewIterator& rhs) noexcept
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

    friend bool operator!=(const MutableBerViewIterator& lhs, const MutableBerViewIterator& rhs) noexcept
    {
        return !(lhs == rhs);
    }

  private:
    absl::Span<uint8_t> m_buffer;
    MutableBerView      m_view;
};

inline BerView::BerView(absl::Span<const uint8_t> input_ber_data, size_t input_header_length,
                        size_t input_content_length) noexcept
{
    assign(input_ber_data, input_header_length, input_content_length);
}

inline BerView::BerView(absl::Span<const uint8_t> input_ber_data, Tag input_tag, size_t input_tag_length,
                        size_t input_header_length, size_t input_content_length) noexcept
{
    assign(input_ber_data, input_tag, input_tag_length, input_header_length, input_content_length);
}

inline void BerView::assign(absl::Span<const uint8_t> input_ber_data) noexcept
{
    m_valid                       = false;
    uint64_t input_content_length = 0;

    size_t input_tag_length      = extract_tag(input_ber_data, m_tag);
    size_t input_len_length      = extract_length(input_ber_data, input_content_length, input_tag_length);
    size_t input_header_length   = input_tag_length + input_len_length;
    size_t input_complete_length = input_header_length + input_content_length;

    if (input_tag_length == 0 || input_len_length == 0 || input_complete_length > input_ber_data.length())
    {
        m_id_length      = 0;
        m_header_length  = 0;
        m_content_length = 0;
        return;
    }

    m_data           = input_ber_data.data();
    m_id_length      = input_tag_length;
    m_header_length  = input_header_length;
    m_content_length = input_content_length;
    m_valid          = true;
}

inline void BerView::assign(absl::Span<const uint8_t> input_ber_data, size_t input_header_length,
                            size_t input_content_length) noexcept
{
    size_t input_tag_length = extract_tag(input_ber_data, m_tag);
    assign(input_ber_data, m_tag, input_tag_length, input_header_length, input_content_length);
}

inline void BerView::assign(absl::Span<const uint8_t> input_ber_data, Tag input_tag, size_t input_tag_length,
                            size_t input_header_length, size_t input_content_length) noexcept
{
    m_data           = input_ber_data.data();
    m_id_length      = input_tag_length;
    m_header_length  = input_header_length;
    m_content_length = input_content_length;
    m_tag            = input_tag;
    m_valid          = true;
}

inline BerViewIterator        BerView::begin() const noexcept { return BerViewIterator{content()}; }
inline BerViewIterator        BerView::end() const noexcept { return BerViewIterator{End::end}; }
inline BerViewIterator        MutableBerView::cbegin() const noexcept { return BerView::begin(); }
inline BerViewIterator        MutableBerView::cend() const noexcept { return BerView::end(); }
inline MutableBerViewIterator MutableBerView::begin() noexcept { return MutableBerViewIterator{content()}; }
inline MutableBerViewIterator MutableBerView::end() noexcept { return MutableBerViewIterator{End::end}; }

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
    auto ber_span = this->ber();
    ber_span.remove_prefix(identifier_length());
    if (ber_span.size() > buffer.size())
    {
        return 0;
    }

    std::memcpy(buffer.data(), ber_span.data(), ber_span.size());
    return ber_span.length();
}

} // namespace fast_ber
