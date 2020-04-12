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
    explicit Null(BerView view) { decode(view); }
    ~Null() = default;

    template <typename Identifier2>
    Null(const Null<Identifier2>&) noexcept
    {
    }

    Null& operator=(const Null&) = default;
    Null& operator=(Null&&) noexcept = default;
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

    constexpr static size_t encoded_length() noexcept;
    EncodeResult            encode(absl::Span<uint8_t> buffer) const noexcept;
    DecodeResult            decode(BerView buffer) noexcept;

    using AsnId = Identifier;

  private:
    std::array<uint8_t, fast_ber::encoded_length(0, Identifier{})> m_data = encoded_header<Identifier>();
};

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
DecodeResult Null<Identifier>::decode(BerView input) noexcept
{
    if (!has_correct_header(input, Identifier{}, Construction::primitive))
    {
        return DecodeResult{false};
    }

    if (Identifier::depth() == 1 && input.content_length() == 0)
    {
        return DecodeResult{true};
    }
    if (Identifier::depth() == 2 && input.begin()->content_length() == 0)
    {
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
