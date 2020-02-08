#pragma once

#include "fast_ber/util/BerView.hpp"
#include "fast_ber/util/DecodeHelpers.hpp"
#include "fast_ber/util/EncodeHelpers.hpp"

#include <cmath>
#include <limits>

namespace fast_ber
{

namespace detail
{
constexpr size_t MaxEncodedLength = 100;

enum class RealEncoding
{
    binary,
    decimal,
    special_real_value
};

enum class SpecialValues : uint8_t
{
    infinity       = 0b01000000,
    minus_infinity = 0b01000001,
    not_a_number   = 0b01000010,
    minus_zero     = 0b01000011
};

inline DecodeResult decode_real(absl::Span<const uint8_t> input, double& output) noexcept;
inline EncodeResult encode_real(absl::Span<uint8_t> output, double input) noexcept;
} // namespace detail

template <typename Identifier = ExplicitId<UniversalTag::real>>
class Real
{
  public:
    Real() noexcept : m_data{} {}
    Real(double num) noexcept { assign(num); }
    Real(BerView& view) noexcept { assign_ber(view); }

    template <typename Identifier2>
    Real(Real<Identifier2>);

    explicit Real(absl::Span<const uint8_t> ber_data) noexcept { assign_ber(ber_data); }

    // Implicit conversion to double
           operator double() const noexcept { return value(); }
    double value() const noexcept;

    Real& operator=(double rhs) noexcept;
    Real& operator=(const Real& rhs) noexcept;
    Real& operator=(const BerView& rhs) noexcept;
    template <typename Identifier2>
    Real& operator=(const Real<Identifier2>& rhs) noexcept;

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

    using AsnId = Identifier;

  private:
    void set_content_length(uint64_t length) noexcept
    {
        assert(length <= std::numeric_limits<uint8_t>::max());
        m_data[0] = static_cast<uint8_t>(length);
    }
    uint8_t content_length() const noexcept { return m_data[0]; }
    size_t  encoded_length() const noexcept { return 1 + content_length(); }

    std::array<uint8_t, detail::MaxEncodedLength> m_data;
};

namespace detail
{

inline EncodeResult encode_real(absl::Span<uint8_t> output, double input) noexcept
{
    if (input == 0)
    {
        return EncodeResult{true, 0};
    }

    if (output.length() < 1)
    {
        return EncodeResult{false, 0};
    }

    if (std::isnan(input))
    {
        if (input > 0)
        {
            output[0] = static_cast<uint8_t>(SpecialValues::minus_infinity);
        }
        else
        {
            output[0] = static_cast<uint8_t>(SpecialValues::infinity);
        }
        return EncodeResult{true, 1};
    }

    // Some more code
}

inline DecodeResult decode_real(absl::Span<const uint8_t> input, double& output) noexcept
{
    if (input.length() == 0)
    {
        output = 0;
        return DecodeResult{true};
    }

    RealEncoding encoding = (input[0] & 0x80)
                                ? RealEncoding::binary
                                : (input[0] & 0x40) ? RealEncoding::decimal : RealEncoding ::special_real_value;

    if (encoding == RealEncoding::binary)
    {
        // some code
    }
    else if (encoding == RealEncoding::decimal)
    {
        // some code
    }
    else if (encoding == RealEncoding::special_real_value)
    {
        if (input[0] == static_cast<uint8_t>(SpecialValues::infinity))
        {
            output = std::numeric_limits<double>::infinity();
            return DecodeResult{true};
        }
        if (input[0] == static_cast<uint8_t>(SpecialValues::minus_infinity))
        {
            output = -std::numeric_limits<double>::infinity();
            return DecodeResult{true};
        }
        if (input[0] == static_cast<uint8_t>(SpecialValues::not_a_number))
        {
            output = std::numeric_limits<double>::quiet_NaN();
            return DecodeResult{true};
        }
        if (input[0] == static_cast<uint8_t>(SpecialValues::minus_zero))
        {
            output = -0;
            return DecodeResult{true};
        }
        return DecodeResult{false};
    }

    return DecodeResult{false};
}

} // namespace detail
} // namespace fast_ber
