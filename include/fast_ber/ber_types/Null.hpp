#pragma once

#include "absl/types/span.h"

#include "fast_ber/util/BerView.hpp"
#include "fast_ber/util/DecodeHelpers.hpp"
#include "fast_ber/util/EncodeHelpers.hpp"

#include <algorithm>
#include <array>
#include <cstdint>

namespace fast_ber
{

template <typename Identifier = ExplicitId<UniversalTag::null>>
class Null
{
  public:
    Null() noexcept            = default;
    Null(const Null&) noexcept = default;
    Null(Null&&) noexcept      = default;
    explicit Null(std::nullptr_t) noexcept {}
    explicit Null(BerView view) { assign_ber(view); }
    template <typename Identifier2>
    Null(const Null<Identifier2>&) noexcept
    {
    }

    Null& operator=(const Null&) = default;
    Null& operator=(Null&&) = default;
    template <typename Identifier2>
    Null& operator=(const Null<Identifier2>&) noexcept
    {
        return *this;
    }

    template <typename Identifier2>
    bool operator==(const Null<Identifier2>&) const noexcept
    {
        return true;
    }
    template <typename Identifier2>
    bool operator!=(const Null<Identifier2>&) const noexcept
    {
        return false;
    }
    std::nullptr_t            value() { return nullptr; }
    absl::Span<const uint8_t> ber() const { return absl::Span<const uint8_t>(m_data); }

    size_t assign_ber(const BerView& rhs) noexcept;
    size_t assign_ber(absl::Span<const uint8_t> buffer) noexcept { return assign_ber(BerView(buffer)); }

    constexpr static size_t encoded_length() noexcept;
    EncodeResult            encode(absl::Span<uint8_t> buffer) const noexcept;
    DecodeResult            decode(absl::Span<const uint8_t> buffer) noexcept;

    using AsnId = Identifier;

  private:
    std::array<uint8_t, fast_ber::encoded_length(0, Identifier{})> m_data = encoded_header<Identifier>();
};

template <typename Identifier>
size_t Null<Identifier>::assign_ber(const BerView& input) noexcept
{
    if (!has_correct_header(input, Identifier{}, Construction::primitive))
    {
        return 0;
    }

    if (Identifier::depth() == 1)
    {
        if (input.content_length() == 0)
        {
            return input.ber_length();
        }
    }
    if (Identifier::depth() == 2)
    {
        if (input.begin()->content_length() == 0)
        {
            return input.ber_length();
        }
    }
    return 0;
}

template <typename Identifier>
constexpr size_t Null<Identifier>::encoded_length() noexcept
{
    return fast_ber::encoded_length(0, Identifier{});
}

template <typename Identifier>
EncodeResult Null<Identifier>::encode(absl::Span<uint8_t> output) const noexcept
{
    if (output.size() < this->m_data.size())
    {
        return EncodeResult{false, 0};
    }

    std::memcpy(output.data(), m_data.data(), this->m_data.size());
    return EncodeResult{true, this->m_data.size()};
}

template <typename Identifier>
DecodeResult decode(BerViewIterator& input, Null<Identifier>&) noexcept
{
    if (!has_correct_header(*input, Identifier{}, Construction::primitive))
    {
        return DecodeResult{false};
    }

    if (Identifier::depth() == 1 && input->content_length() == 0)
    {
        ++input;
        return DecodeResult{true};
    }
    if (Identifier::depth() == 2 && input->begin()->content_length() == 0)
    {
        ++input;
        return DecodeResult{true};
    }
    return DecodeResult{false};
}

template <typename Identifier>
std::ostream& operator<<(std::ostream& os, const Null<Identifier>)
{
    return os << "null";
}

} // namespace fast_ber
