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
// vector. Can be constructed directly from encoded ber memory (decode()) or by specifiyng the desired contents
// (assign_contents)

template <typename Identifier, size_t max_content_length>
class SmallFixedIdBerContainer
{
  public:
    constexpr static size_t max_possible_content_length =
        (Identifier::depth() == 2) ? 128 - encoded_header_length(0, inner_identifier(Identifier{})) : 128;

    static_assert(max_content_length <= max_possible_content_length, "Content length must fit in one byte");

    SmallFixedIdBerContainer() noexcept;
    SmallFixedIdBerContainer(const SmallFixedIdBerContainer&) = default;
    SmallFixedIdBerContainer(SmallFixedIdBerContainer&&)      = default;
    SmallFixedIdBerContainer(const BerView input_view) noexcept { decode(input_view); }
    SmallFixedIdBerContainer(absl::Span<const uint8_t> input_data, ConstructionMethod method) noexcept;
    SmallFixedIdBerContainer(Construction input_construction, Class input_class, Tag input_tag,
                             absl::Span<const uint8_t> input_content) noexcept;
    template <typename Identifier2, size_t max_content_length_2>
    SmallFixedIdBerContainer(const SmallFixedIdBerContainer<Identifier2, max_content_length_2>& rhs) noexcept
    {
        assign_content(rhs.content());
    }

    SmallFixedIdBerContainer& operator=(const BerView input_view) noexcept;
    SmallFixedIdBerContainer& operator=(const SmallFixedIdBerContainer& input_container) = default;
    SmallFixedIdBerContainer& operator=(SmallFixedIdBerContainer&& input_container) = default;
    template <typename Identifier2, size_t max_content_length_2>
    SmallFixedIdBerContainer&
    operator=(const SmallFixedIdBerContainer<Identifier2, max_content_length_2>& rhs) noexcept;

    void assign_content(const absl::Span<const uint8_t> input_content) noexcept;
    void resize_content(size_t size);

    constexpr static Class class_() noexcept { return Identifier::class_(); }
    constexpr static Tag   tag() noexcept { return Identifier::tag(); }

    absl::Span<uint8_t>       content() noexcept { return absl::MakeSpan(content_data(), content_length()); }
    absl::Span<const uint8_t> content() const noexcept { return absl::MakeSpan(content_data(), content_length()); }
    uint8_t*                  content_data() noexcept { return m_data.data() + m_header_length; }
    const uint8_t*            content_data() const noexcept { return m_data.data() + m_header_length; }
    size_t                    content_length() const noexcept { return m_data[m_header_length - 1]; }

    absl::Span<const uint8_t> ber() const noexcept { return absl::MakeSpan(ber_data(), ber_length()); }
    const uint8_t*            ber_data() const noexcept { return m_data.data(); }
    size_t                    ber_length() const noexcept { return m_header_length + content_length(); }

    bool operator==(const SmallFixedIdBerContainer& rhs) const { return content() == rhs.content(); }
    bool operator!=(const SmallFixedIdBerContainer& rhs) const { return !(*this == rhs); }

    BerView view() const noexcept { return BerView(m_data); }

    EncodeResult encode(absl::Span<uint8_t> buffer) const noexcept;
    DecodeResult decode(BerView view) noexcept;

  private:
    template <Class class_1, Tag tag_1>
    DecodeResult decode_impl(const BerView input_view, Id<class_1, tag_1>) noexcept;
    template <typename Identifier1, typename Identifier2>
    DecodeResult decode_impl(const BerView input_view, DoubleId<Identifier1, Identifier2>) noexcept;
    void         set_content_length(size_t length) noexcept;

    constexpr static size_t                                   m_header_length = encoded_header_length(0, Identifier{});
    std::array<uint8_t, m_header_length + max_content_length> m_data          = [] {
        std::array<uint8_t, m_header_length + max_content_length> data;
        encode_header(absl::Span<uint8_t>(data), 0, Identifier{}, Construction::primitive);
        return data;
    }();
};

template <typename Identifier, size_t max_content_length>
SmallFixedIdBerContainer<Identifier, max_content_length>::SmallFixedIdBerContainer() noexcept
{
}

template <typename Identifier, size_t max_content_length>
SmallFixedIdBerContainer<Identifier, max_content_length>::SmallFixedIdBerContainer(absl::Span<const uint8_t> input_data,
                                                                                   ConstructionMethod method) noexcept
{
    if (method == ConstructionMethod::construct_with_provided_content)
    {
        assign_content(input_data);
    }
    else if (method == ConstructionMethod::construct_from_ber_packet)
    {
        decode(input_data);
    }
    else
    {
        assert(0);
    }
}

template <typename Identifier, size_t max_content_length>
SmallFixedIdBerContainer<Identifier, max_content_length>::SmallFixedIdBerContainer(
    Construction input_construction, Class input_class, Tag input_tag, absl::Span<const uint8_t> input_content) noexcept
{
    assign_content(input_construction, input_class, input_tag, input_content);
}

template <typename Identifier, size_t max_content_length>
SmallFixedIdBerContainer<Identifier, max_content_length>& SmallFixedIdBerContainer<Identifier, max_content_length>::
                                                          operator=(const BerView input_view) noexcept
{
    decode(input_view);
    return *this;
}

template <typename Identifier1, size_t max_content_length>
template <typename Identifier2, size_t max_content_length_2>
SmallFixedIdBerContainer<Identifier1, max_content_length>& SmallFixedIdBerContainer<Identifier1, max_content_length>::
                                                           operator=(const SmallFixedIdBerContainer<Identifier2, max_content_length_2>& rhs) noexcept
{
    assign_content(rhs.content());
    return *this;
}

template <typename Identifier, size_t max_content_length>
template <Class class_1, Tag tag_1>
DecodeResult SmallFixedIdBerContainer<Identifier, max_content_length>::decode_impl(const BerView      input_view,
                                                                                   Id<class_1, tag_1> id) noexcept
{
    if (!(input_view.is_valid() && input_view.identifier() == id &&
          input_view.construction() == Construction::primitive && input_view.content_length() <= max_content_length))
    {
        return DecodeResult{false};
    }

    std::memcpy(m_data.data() + m_header_length, input_view.content_data(), input_view.content_length());
    set_content_length(input_view.content_length());

    return DecodeResult{true};
}

template <typename Identifier, size_t max_content_length>
template <typename Identifier1, typename Identifier2>
DecodeResult
SmallFixedIdBerContainer<Identifier, max_content_length>::decode_impl(const BerView input_view,
                                                                      DoubleId<Identifier1, Identifier2>) noexcept
{
    if (!(input_view.is_valid() && input_view.identifier() == Identifier1{} &&
          input_view.construction() == Construction::constructed && input_view.begin()->is_valid() &&
          input_view.begin()->identifier() == Identifier2{} &&
          input_view.begin()->construction() == Construction::primitive &&
          input_view.begin()->content_length() <= max_content_length))
    {

        return DecodeResult{false};
    }

    std::memcpy(m_data.data() + m_header_length, input_view.begin()->content_data(),
                input_view.begin()->content_length());
    set_content_length(input_view.begin()->content_length());

    return DecodeResult{true};
}

template <typename Identifier, size_t max_content_length>
void SmallFixedIdBerContainer<Identifier, max_content_length>::assign_content(
    const absl::Span<const uint8_t> input_content) noexcept
{
    assert(input_content.length() <= max_content_length);

    std::memcpy(m_data.data() + m_header_length, input_content.data(), input_content.length());
    set_content_length(input_content.length());

    assert(view().is_valid());
}

template <typename Identifier, size_t max_content_length>
void SmallFixedIdBerContainer<Identifier, max_content_length>::resize_content(size_t size)
{
    assert(size <= max_content_length);

    set_content_length(size);

    assert(view().is_valid());
}

template <typename Identifier, size_t max_content_length>
void SmallFixedIdBerContainer<Identifier, max_content_length>::set_content_length(size_t length) noexcept
{
    assert(length <= max_content_length);

    m_data[m_header_length - 1] = static_cast<uint8_t>(length);

    if (Identifier::depth() == 2)
    {
        m_data[encoded_header_length(0, outer_identifier(Identifier{})) - 1] =
            static_cast<uint8_t>(encoded_length(length, inner_identifier(Identifier{})));
    }

    assert(content_length() == length);
}

template <typename Identifier, size_t max_content_length>
EncodeResult SmallFixedIdBerContainer<Identifier, max_content_length>::encode(absl::Span<uint8_t> buffer) const noexcept
{
    if (buffer.size() < ber_length())
    {
        return EncodeResult{false, 0};
    }

    memcpy(buffer.data(), ber_data(), ber_length());
    return EncodeResult{true, ber_length()};
}

template <typename Identifier, size_t max_content_length>
DecodeResult SmallFixedIdBerContainer<Identifier, max_content_length>::decode(BerView view) noexcept
{
    return decode_impl(view, Identifier{});
}

} // namespace fast_ber
