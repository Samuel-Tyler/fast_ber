#pragma once

#include "fast_ber/ber_types/Identifier.hpp"
#include "fast_ber/util/BerContainer.hpp"
#include "fast_ber/util/BerView.hpp"
#include "fast_ber/util/EncodeIdentifiers.hpp"

#include "absl/container/inlined_vector.h"
#include "absl/types/span.h"

#include <array>

namespace fast_ber
{

// Owning container of a ber packet. Contents may or may not be valid. Memory is stored in a small buffer optimized
// vector. Can be constructed directly from encoded ber memory (assign_ber) or by specifiyng the desired contents
// (assign_contents)

template <typename Identifier>
class FixedIdBerContainer
{
  public:
    FixedIdBerContainer() noexcept;
    FixedIdBerContainer(const FixedIdBerContainer&) = default;
    FixedIdBerContainer(FixedIdBerContainer&&)      = default;
    FixedIdBerContainer(const BerView& input_view) noexcept { assign_ber(input_view); }
    FixedIdBerContainer(absl::Span<const uint8_t> input_data, ConstructionMethod method) noexcept;
    FixedIdBerContainer(Construction input_construction, Class input_class, Tag input_tag,
                        absl::Span<const uint8_t> input_content) noexcept;

    FixedIdBerContainer& operator=(const BerView& input_view) noexcept;
    FixedIdBerContainer& operator=(const FixedIdBerContainer& input_container) noexcept;
    size_t               assign_ber(const BerView& input_view) noexcept;
    size_t               assign_ber(const FixedIdBerContainer& container) noexcept;
    size_t               assign_ber(const absl::Span<const uint8_t> input_data) noexcept;
    void                 assign_content(const absl::Span<const uint8_t> input_content) noexcept;
    void                 assign_content(Construction input_construction, Class input_class, Tag input_tag,
                                        absl::Span<const uint8_t> input_content) noexcept;
    void                 resize_content(size_t size);

    constexpr static Class class_() noexcept { return Identifier::class_(); }
    constexpr static Tag   tag() noexcept { return Identifier::tag(); }

    absl::Span<uint8_t>       content() noexcept { return absl::MakeSpan(content_data(), m_content_length); }
    absl::Span<const uint8_t> content() const noexcept { return absl::MakeSpan(content_data(), m_content_length); }
    uint8_t*                  content_data() noexcept { return m_data[m_data.size() - m_content_length]; }
    const uint8_t*            content_data() const noexcept { return m_data[m_data.size() - m_content_length]; }
    size_t                    content_length() const noexcept { return m_view.content_length(); }

    absl::Span<const uint8_t> encoded() const noexcept { return absl::MakeSpan(m_data.begin(), m_data.end()); }
    const uint8_t*            ber_data() const noexcept { return m_data.data(); }
    size_t                    ber_length() const noexcept { return m_data.length(); }

    BerView view() const noexcept { return BerView(m_data.begin(), m_data.end()); }

    size_t encode(absl::Span<uint8_t> buffer) const noexcept { return m_view.encode(buffer); }
    size_t encode_content_and_length(absl::Span<uint8_t> buffer) const noexcept;

  private:
    absl::InlinedVector<uint8_t, 100u> m_data;
    size_t                             m_content_length;
};

template <typename Identifier>
FixedIdBerContainer<Identifier>::FixedIdBerContainer() noexcept
    : m_data{0x80, 0x00}, m_view(absl::MakeSpan(m_data.begin(), m_data.size()), 0, 1, 2, 0)
{
}

template <typename Identifier>
FixedIdBerContainer<Identifier>::FixedIdBerContainer(absl::Span<const uint8_t> input_data,
                                                     ConstructionMethod        method) noexcept
    : m_data(input_data.begin(), input_data.end()), m_view(absl::MakeSpan(m_data.begin(), m_data.size()))
{
    if (method == ConstructionMethod::construct_with_provided_content)
    {
        assign_content(input_data);
    }
    else if (method == ConstructionMethod::construct_from_ber_packet)
    {
        assign_ber(input_data);
    }
    else
    {
        assert(0);
    }
}

template <typename Identifier>
FixedIdBerContainer<Identifier>::FixedIdBerContainer(Construction input_construction, Class input_class, Tag input_tag,
                                                     absl::Span<const uint8_t> input_content) noexcept
{
    assign_content(input_construction, input_class, input_tag, input_content);
}

template <typename Identifier>
FixedIdBerContainer& FixedIdBerContainer<Identifier>::operator=(const BerView& input_view) noexcept
{
    assign_ber(input_view);
    return *this;
}

template <typename Identifier>
FixedIdBerContainer& FixedIdBerContainer<Identifier>::operator=(const FixedIdBerContainer& container) noexcept
{
    assign_ber(container);
    return *this;
}

template <typename Identifier>
size_t FixedIdBerContainer<Identifier>::assign_ber(const BerView& input_view) noexcept
{
    if (!input_view.is_valid())
    {
        return false;
    }

    m_data.assign(input_view.ber().begin(), input_view.ber().end());
    m_view.assign(absl::MakeSpan(m_data.data(), m_data.size()), input_view.tag(), input_view.identifier_length(),
                  input_view.header_length(), input_view.content_length());

    return input_view.ber_length();
}

template <typename Identifier>
size_t FixedIdBerContainer<Identifier>::assign_ber(const FixedIdBerContainer& input_container) noexcept
{
    return assign_ber(input_container.view());
}
template <typename Identifier>
size_t FixedIdBerContainer<Identifier>::assign_ber(const absl::Span<const uint8_t> input_data) noexcept
{
    m_data.assign(input_data.begin(), input_data.end());
    m_view.assign(absl::MakeSpan(m_data.begin(), m_data.size()));

    if (!m_view.is_valid())
    {
        return 0;
    }
    return ber_length();
}

template <typename Identifier>
void FixedIdBerContainer<Identifier>::assign_content(const absl::Span<const uint8_t> input_content) noexcept
{
    m_data.resize(15 + input_content.size());
    m_data[0] = 0x80; // No identifier provided, use a tag of 0
    size_t new_header_length =
        1 + encode_length(absl::Span<uint8_t>(m_data.data() + 1, m_data.size() - 1), input_content.size());

    m_data.resize(new_header_length + input_content.length());
    std::copy(input_content.data(), input_content.end(), m_data.data() + new_header_length);
    m_view.assign(m_data, 0, 1, new_header_length, input_content.size());

    assert(m_view.is_valid());
}

template <typename Identifier>
void FixedIdBerContainer<Identifier>::assign_content(Construction input_construction, Class input_class, Tag input_tag,
                                                     absl::Span<const uint8_t> input_content) noexcept
{
    m_data.resize(30 + input_content.size());
    size_t input_header_length = encode_header(absl::MakeSpan(m_data.data(), m_data.size()), input_construction,
                                               input_class, input_tag, input_content.size());

    m_data.resize(input_header_length + input_content.length());
    std::copy(input_content.data(), input_content.end(), m_data.data() + input_header_length);
    m_view.assign(m_data, input_header_length, input_content.size());

    assert(m_view.is_valid());
}

template <typename Identifier>
void FixedIdBerContainer<Identifier>::resize_content(size_t size)
{
    std::array<uint8_t, 20> length_buffer;

    size_t old_header_length   = view().header_length();
    size_t old_size            = view().content_length();
    size_t new_length_offset   = view().identifier_length();
    size_t new_length_length   = encode_length(absl::MakeSpan(length_buffer), size);
    size_t new_header_length   = new_length_offset + new_length_length;
    size_t new_complete_length = new_header_length + size;

    m_data.resize(new_complete_length);
    std::memmove(m_data.data() + new_header_length, m_data.data() + old_header_length, std::min(old_size, size));
    std::copy(length_buffer.data(), length_buffer.data() + new_length_length, m_data.data() + new_length_offset);
    m_view.assign(absl::MakeSpan(m_data.data(), m_data.size()), new_header_length, size);

    assert(new_length_length != 0);
    assert(m_view.is_valid());
}

template <typename Identifier>
size_t FixedIdBerContainer<Identifier>::encode_content_and_length(absl::Span<uint8_t> buffer) const noexcept
{
    return m_view.encode_content_and_length(buffer);
}

} // namespace fast_ber
