#pragma once

#include "fast_ber/util/BerView.hpp"
#include "fast_ber/util/DecodeHelpers.hpp"
#include "fast_ber/util/EncodeHelpers.hpp"

namespace fast_ber
{

template <typename Identifier = ExplicitId<UniversalTag::real>>
class Real
{
  public:
    Real() noexcept : m_data{} {}
    Real(double num) noexcept { assign(num); }
    Real(BerView& view) noexcept { assign_ber(view); }

    explicit Real(absl::Span<const uint8_t> ber_data) noexcept { assign_ber(ber_data); }

    explicit operator double() const noexcept { return value(); }
    double   value() const noexcept;

    Real&  operator=(double rhs) noexcept;
    Real&  operator=(const Real& rhs) noexcept;
    Real&  operator=(const BerView& rhs) noexcept;
    void   assign(double val) noexcept;
    void   assign(const Real& rhs) noexcept;
    size_t assign_ber(const BerView& rhs) noexcept;
    size_t assign_ber(absl::Span<const uint8_t> buffer) noexcept;

    bool operator==(const Real&) const
    {
        // TODO: fuzzy equality here
        return true;
    }
    bool operator!=(const Real& rhs) const { return !(*this == rhs); }

    EncodeResult encode_content_and_length(absl::Span<uint8_t> buffer) const noexcept;

    constexpr static size_t encoded_length() noexcept;
    EncodeResult            encode(absl::Span<uint8_t> buffer) const noexcept;
    DecodeResult            decode(BerView input) noexcept;

    using AsnId = Identifier;

  private:
    void set_content_length(uint64_t length) noexcept
    {
        assert(length <= std::numeric_limits<uint8_t>::max());
        m_data[0] = static_cast<uint8_t>(length);
    }
    uint8_t content_length() const noexcept { return m_data[0]; }

    std::array<uint8_t, sizeof(int64_t) + sizeof(uint8_t)> m_data;
};

} // namespace fast_ber
