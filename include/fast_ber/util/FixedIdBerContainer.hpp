#pragma once

#include "fast_ber/ber_types/Identifier.hpp"
#include "fast_ber/util/BerContainer.hpp"
#include "fast_ber/util/BerView.hpp"
#include "fast_ber/util/DecodeHelpers.hpp"
#include "fast_ber/util/EncodeHelpers.hpp"

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
    template <typename Identifier2>
    FixedIdBerContainer(const FixedIdBerContainer<Identifier2>& rhs) noexcept
    {
        assign_content(rhs.content());
    }

    FixedIdBerContainer& operator=(const BerView& input_view) noexcept;
    FixedIdBerContainer& operator=(const FixedIdBerContainer& input_container) = default;
    FixedIdBerContainer& operator=(FixedIdBerContainer&& input_container) = default;
    template <typename Identifier2>
    FixedIdBerContainer& operator=(const FixedIdBerContainer<Identifier2>& rhs) noexcept;

    size_t assign_ber(const BerView& input_view) noexcept;
    size_t assign_ber(const absl::Span<const uint8_t> input_data) noexcept;
    void   assign_content(const absl::Span<const uint8_t> input_content) noexcept;
    void   resize_content(size_t size);

    constexpr static Class class_() noexcept { return Identifier::class_(); }
    constexpr static Tag   tag() noexcept { return Identifier::tag(); }

    absl::Span<uint8_t>       content() noexcept { return absl::MakeSpan(content_data(), m_content_length); }
    absl::Span<const uint8_t> content() const noexcept { return absl::MakeSpan(content_data(), m_content_length); }
    uint8_t*                  content_data() noexcept { return m_data.data() + m_data.size() - m_content_length; }
    const uint8_t*            content_data() const noexcept { return m_data.data() + m_data.size() - m_content_length; }
    size_t                    content_length() const noexcept { return m_content_length; }

    absl::Span<const uint8_t> ber() const noexcept { return absl::MakeSpan(m_data.begin(), m_data.end()); }
    const uint8_t*            ber_data() const noexcept { return m_data.data(); }
    size_t                    ber_length() const noexcept { return m_data.size(); }

    bool operator==(const FixedIdBerContainer& rhs) const { return content() == rhs.content(); }
    bool operator!=(const FixedIdBerContainer& rhs) const { return !(*this == rhs); }

    BerView view() const noexcept { return BerView(m_data); }

    size_t       encoded_length() const noexcept { return m_data.size(); }
    EncodeResult encode(absl::Span<uint8_t> buffer) const noexcept;
    DecodeResult decode(BerView view) noexcept;
    DecodeResult decode(BerViewIterator& iter) noexcept;

    EncodeResult encode_content_and_length(absl::Span<uint8_t> buffer) const noexcept;

  private:
    template <Class class_1, Tag tag_1>
    size_t assign_ber_impl(const BerView& input_view, Id<class_1, tag_1>) noexcept;
    template <typename Identifier1, typename Identifier2>
    size_t assign_ber_impl(const BerView& input_view, DoubleId<Identifier1, Identifier2>) noexcept;

    absl::InlinedVector<uint8_t, 100u> m_data;
    size_t                             m_content_length;
};

template <typename Identifier>
FixedIdBerContainer<Identifier>::FixedIdBerContainer() noexcept : m_data(), m_content_length(0)
{
    m_data.resize(fast_ber::encoded_length(0, Identifier{}));
    encode_header(absl::Span<uint8_t>(m_data), 0, Identifier{}, Construction::primitive);
}

template <typename Identifier>
FixedIdBerContainer<Identifier>::FixedIdBerContainer(absl::Span<const uint8_t> input_data,
                                                     ConstructionMethod        method) noexcept
    : m_data()
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
FixedIdBerContainer<Identifier>& FixedIdBerContainer<Identifier>::operator=(const BerView& input_view) noexcept
{
    assign_ber(input_view);
    return *this;
}

template <typename Identifier1>
template <typename Identifier2>
FixedIdBerContainer<Identifier1>& FixedIdBerContainer<Identifier1>::
                                  operator=(const FixedIdBerContainer<Identifier2>& rhs) noexcept
{
    assign_content(rhs.content());
    return *this;
}

template <typename Identifier>
template <Class class_1, Tag tag_1>
size_t FixedIdBerContainer<Identifier>::assign_ber_impl(const BerView& input_view, Id<class_1, tag_1> id) noexcept
{
    if (!has_correct_header(input_view, id, Construction::primitive))
    {
        return 0;
    }

    m_data.assign(input_view.ber().begin(), input_view.ber().end());
    m_content_length = input_view.content_length();
    return input_view.ber_length();
}

template <typename Identifier>
template <typename Identifier1, typename Identifier2>
size_t FixedIdBerContainer<Identifier>::assign_ber_impl(const BerView&                     input_view,
                                                        DoubleId<Identifier1, Identifier2> id) noexcept
{
    if (!has_correct_header(input_view, id, Construction::primitive))
    {
        return 0;
    }

    m_data.assign(input_view.ber().begin(), input_view.ber().end());
    m_content_length = input_view.begin()->content_length();
    return input_view.ber_length();
}

template <typename Identifier>
size_t FixedIdBerContainer<Identifier>::assign_ber(const BerView& input_view) noexcept
{
    return assign_ber_impl(input_view, Identifier{});
}

template <typename Identifier>
size_t FixedIdBerContainer<Identifier>::assign_ber(const absl::Span<const uint8_t> input_data) noexcept
{
    return assign_ber(BerView(input_data));
}

template <typename Identifier>
void FixedIdBerContainer<Identifier>::assign_content(const absl::Span<const uint8_t> input_content) noexcept
{
    const size_t encoded_len = fast_ber::encoded_length(input_content.length(), Identifier{});
    const size_t header_len  = encoded_len - input_content.length();

    m_data.resize(encoded_len);
    std::memcpy(m_data.data() + header_len, input_content.data(), input_content.length());
    encode_header(absl::Span<uint8_t>(m_data), input_content.size(), Identifier{}, Construction::primitive);
    m_content_length = input_content.length();

    assert(view().is_valid());
}

template <typename Identifier>
void FixedIdBerContainer<Identifier>::resize_content(size_t size)
{
    size_t old_header_length = m_data.size() - content_length();
    size_t new_header_length = encoded_header_length(size, Identifier{});

    m_data.resize(new_header_length + size);
    std::memmove(m_data.data() + new_header_length, m_data.data() + old_header_length,
                 std::min(m_content_length, size));

    encode_header(absl::Span<uint8_t>(m_data), size, Identifier{}, Construction::primitive);
    m_content_length = size;

    assert(view().is_valid());
}

template <typename Identifier>
EncodeResult FixedIdBerContainer<Identifier>::encode(absl::Span<uint8_t> buffer) const noexcept
{
    if (buffer.size() < m_data.size())
    {
        return EncodeResult{false, 0};
    }

    memcpy(buffer.data(), m_data.data(), m_data.size());
    return EncodeResult{true, m_data.size()};
}

template <typename Identifier>
DecodeResult FixedIdBerContainer<Identifier>::decode(BerView view) noexcept
{
    return DecodeResult{assign_ber(view) > 0};
}

template <typename Identifier>
DecodeResult FixedIdBerContainer<Identifier>::decode(BerViewIterator& iter) noexcept
{
    size_t res = assign_ber(*iter);
    if (res == 0)
    {
        return DecodeResult{false};
    }
    ++iter;
    return DecodeResult{true};
}

template <typename Identifier>
EncodeResult FixedIdBerContainer<Identifier>::encode_content_and_length(absl::Span<uint8_t> buffer) const noexcept
{
    return view().encode_content_and_length(buffer);
}

} // namespace fast_ber
