#pragma once

#include "fast_ber/util/BerView.hpp"
#include "fast_ber/util/Create.hpp"

#include "absl/container/inlined_vector.h"
#include "absl/types/span.h"

namespace fast_ber
{

class BerContainer
{
  public:
    BerContainer() noexcept : m_data(), m_view() {}

    BerContainer(absl::Span<const uint8_t> data) noexcept;
    BerContainer(Construction construction, Class class_, int tag, absl::Span<const uint8_t> content) noexcept;

    void assign_ber(const absl::Span<const uint8_t> data) noexcept;
    void assign_contents(const absl::Span<const uint8_t> data) noexcept;

    bool         is_valid() const noexcept { return m_view.is_valid(); }
    Construction construction() const noexcept { return m_view.construction(); }
    Class        class_() const noexcept { return m_view.class_(); }
    long         tag() const noexcept { return m_view.tag(); }

    absl::Span<const uint8_t> data() const noexcept { return m_view.data(); }
    absl::Span<uint8_t>       content() noexcept { return m_view.content(); }
    absl::Span<const uint8_t> content() const noexcept { return m_view.content(); }

    const BerView& view() const noexcept { return m_view; }

  private:
    absl::InlinedVector<uint8_t, 100> m_data;
    MutableBerView                    m_view;
};

inline BerContainer::BerContainer(absl::Span<const uint8_t> data) noexcept
    : m_data(data.begin(), data.end()), m_view(absl::Span<const uint8_t>(m_data.begin(), m_data.size()))
{
}

inline BerContainer::BerContainer(Construction construction, Class class_, int tag,
                                  absl::Span<const uint8_t> content) noexcept
{
    m_data.resize(100 + content.size());
    size_t header_length =
        create_header(absl::Span<uint8_t>(m_data.begin(), m_data.size()), construction, class_, tag, content.length());
    m_data.resize(header_length + content.size());
}

inline void BerContainer::assign_ber(const absl::Span<const uint8_t> data) noexcept
{
    m_data.assign(data.begin(), data.end());
    m_view.assign(absl::Span<const uint8_t>(m_data.begin(), m_data.size()));
}

inline void BerContainer::assign_contents(const absl::Span<const uint8_t> data) noexcept
{
    m_data.resize(15 + data.size());
    m_data[0]            = 0; // No identifier provided, use id of 0
    size_t header_length = 1 + create_length(absl::Span<uint8_t>(m_data.data() + 1, m_data.size() - 1), data.size());
    m_data.resize(header_length + data.length());
    std::copy(data.data(), data.end(), m_data.data() + header_length);
    m_view.assign(m_data, header_length, data.size());
}

} // namespace fast_ber
