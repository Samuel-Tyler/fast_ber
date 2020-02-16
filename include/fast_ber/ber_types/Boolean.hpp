#pragma once

#include "absl/types/span.h"

#include "fast_ber/ber_types/Class.hpp"
#include "fast_ber/ber_types/Construction.hpp"
#include "fast_ber/util/BerView.hpp"
#include "fast_ber/util/DecodeHelpers.hpp"
#include "fast_ber/util/EncodeHelpers.hpp"
#include "fast_ber/util/EncodeIdentifiers.hpp"
#include "fast_ber/util/Extract.hpp"

#include <algorithm>
#include <array>
#include <cstdint>
#include <cstring>

namespace fast_ber
{

template <typename Identifier = ExplicitId<UniversalTag::boolean>>
class Boolean
{
  public:
    Boolean() noexcept = default;
    Boolean(bool val) noexcept { assign(val); }
    template <typename Identifier2>
    Boolean(const Boolean<Identifier2>& rhs) noexcept;
    explicit Boolean(const BerView& view) noexcept { assign_ber(view); }
    explicit Boolean(absl::Span<const uint8_t> ber_data) noexcept { assign_ber(ber_data); }

    // Implicit conversion to bool
                              operator bool() const noexcept { return value(); }
    bool                      value() const noexcept { return static_cast<bool>(m_data.back()); }
    absl::Span<const uint8_t> ber() const { return absl::Span<const uint8_t>(m_data); }

    Boolean& operator=(bool rhs) noexcept;
    template <typename Identifier2>
    Boolean& operator=(const Boolean<Identifier2>& rhs) noexcept;
    Boolean& operator=(const BerView& rhs) noexcept;
    void     assign(bool val) noexcept;
    template <typename Identifier2>
    void   assign(const Boolean<Identifier2>& rhs) noexcept;
    size_t assign_ber(const BerView& rhs) noexcept;
    size_t assign_ber(absl::Span<const uint8_t> buffer) noexcept;

    using AsnId = Identifier;

    template <typename Identifier2>
    friend class Boolean;

  private:
    std::array<uint8_t, encoded_length(1, Identifier{})> m_data = encoded_header<Identifier, 1>();
}; // namespace fast_ber

template <typename Identifier>
template <typename Identifier2>
Boolean<Identifier>::Boolean(const Boolean<Identifier2>& rhs) noexcept
{
    m_data.back() = rhs.m_data.back();
}

template <typename Identifier>
inline Boolean<Identifier>& Boolean<Identifier>::operator=(bool rhs) noexcept
{
    assign(rhs);
    return *this;
}

template <typename Identifier>
template <typename Identifier2>
inline Boolean<Identifier>& Boolean<Identifier>::operator=(const Boolean<Identifier2>& rhs) noexcept
{
    m_data.back() = rhs.m_data.back();
    return *this;
}

template <typename Identifier>
inline Boolean<Identifier>& Boolean<Identifier>::operator=(const BerView& rhs) noexcept
{
    assign_ber(rhs);
    return *this;
}

template <typename Identifier>
inline void Boolean<Identifier>::assign(bool val) noexcept
{
    m_data.back() = static_cast<uint8_t>(val ? 0xFF : 0x00);
}

template <typename Identifier>
template <typename Identifier2>
inline void Boolean<Identifier>::assign(const Boolean<Identifier2>& rhs) noexcept
{
    m_data.back() = rhs.m_data.back();
}

template <typename Identifier>
inline size_t Boolean<Identifier>::assign_ber(const BerView& input) noexcept
{
    if (!has_correct_header(input, Identifier{}, Construction::primitive))
    {
        return 0;
    }

    if (Identifier::depth() == 1 && input.content_length() == 1)
    {
        m_data.back() = *input.content_data();
        return input.ber_length();
    }
    if (Identifier::depth() == 2 && input.begin()->content_length() == 1)
    {
        m_data.back() = *input.begin()->content_data();
        return input.ber_length();
    }
    return 0;
}

template <typename Identifier>
inline size_t Boolean<Identifier>::assign_ber(absl::Span<const uint8_t> buffer) noexcept
{
    return assign_ber(BerView(buffer));
}

template <typename Identifier>
constexpr size_t encoded_length(const Boolean<Identifier>&) noexcept
{
    return encoded_length(1, Identifier{});
}

template <typename Identifier>
EncodeResult encode(absl::Span<uint8_t> output, const Boolean<Identifier>& object) noexcept
{
    if (output.size() < object.ber().size())
    {
        return EncodeResult{false, 0};
    }

    std::memcpy(output.data(), object.ber().data(), object.ber().size());
    return EncodeResult{true, object.ber().size()};
}

template <typename Identifier>
DecodeResult decode(BerViewIterator& input, Boolean<Identifier>& output) noexcept
{
    bool success = output.assign_ber(*input) > 0;
    if (success)
    {
        ++input;
    }
    return DecodeResult{success};
}

template <typename Identifier>
std::ostream& operator<<(std::ostream& os, const Boolean<Identifier>& object)
{
    if (object)
    {
        return os << "true";
    }
    return os << "false";
}

} // namespace fast_ber
