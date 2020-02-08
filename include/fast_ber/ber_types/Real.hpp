#pragma once

#include "absl/types/span.h"
#include "absl/strings/numbers.h"
#include "fast_ber/util/BerView.hpp"
#include "fast_ber/util/EncodeHelpers.hpp"

#include <limits>
#include <regex>
#include <algorithm>

namespace fast_ber
{

inline bool   decode_real(absl::Span<const uint8_t> input, double& output) noexcept;
inline bool   decode_real_special(absl::Span<const uint8_t> input, double& output) noexcept;
inline bool   decode_real_binary(absl::Span<const uint8_t> input, double& output) noexcept;
inline bool   decode_real_decimal(absl::Span<const uint8_t> input, double& output) noexcept;
inline bool   check_real_binary(absl::Span<const uint8_t> input) noexcept;
inline size_t encode_real(absl::Span<uint8_t> output, double input) noexcept;

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

    using AsnId = Identifier;

  private:
    void set_content_length(uint64_t length) noexcept
    {
        assert(length <= std::numeric_limits<uint8_t>::max());
        m_data[0] = static_cast<uint8_t>(length);
    }
    uint8_t content_length() const noexcept { return m_data[0]; }
    size_t  encoded_length() const noexcept { return 1 + content_length(); }

    std::array<uint8_t, sizeof(int64_t) + sizeof(uint8_t)> m_data;
};


/**
 * Checks that real binary value conforms to Real ASN.1 specification
 * @param input BER encoded real binary value to check
 * @return true on success, false on failure
 */
inline bool check_real_binary(absl::Span<const uint8_t> input) noexcept
{
    /* Minimal binary construction is: | control | exponent | number |  => length is 3 bytes minimum
     * Check that base bits are not set to the reserved value, see X690 Clause 8.5.7.4
     * */
    if (input.length() > 3  && (input[0] & 0x30) != 0x30)
    {
        /* X690 Clause 8.5.7.4 */
        size_t exponent_len = (input[0] & 0x03) + 1;
        size_t exponent_offset = 1;

        /* X690 Clause 8.5.7.4.d: we should get something like this:
         * | control | exponent_len | exponent_1 | ... | exponent_N | number |
         * */
        if (exponent_len == 4)
        {
            exponent_len = static_cast<size_t>(input[1]);
            exponent_offset = 2;
        }

        /* Check that exponent length is valid or does not overflow input */
        if (exponent_len == 0 || (exponent_len + exponent_offset) >= input.length())
        {
            return false;
        }

        /* Check that the exponent does not start with 9 identical bits */
        if (exponent_len > 1)
        {
            uint8_t first_byte = input[exponent_offset];
            uint8_t second_byte = input[exponent_offset + 1];
            if ((first_byte == 0xFF && (second_byte & 0x80) != 0) ||
                (first_byte == 0x00 && (second_byte & 0x80) == 0))
            {
                /* The exponent begins with 9 identical bits */
                return false;
            }
        }

        /* Check that the mantissa is not zero */
        for (size_t i = exponent_len + exponent_offset ; i < input.length() ; ++i)
        {
            if (input[i] != 0) return true;
        }
    }

    return false;
}


/**
 * Decodes a real value into a double
 * @param input BER encoded real value
 * @param output decoded special value
 * @return true on success, false on failure
 */
inline bool decode_real(absl::Span<const uint8_t> input, double& output) noexcept
{
    bool res = false;
    std::array<uint8_t, sizeof(double)> buffer;

    /* X690 Clause 8.5.2: it is the value Plus Zero */
    if (input.length() == 0)
    {
        output = 0.0;
        res = true;
    }
    /* X690 Clause 8.5.9: it is a Special Real Value */
    else if (input[0] & 0x40)
    {
        res = decode_real_special(input, output);
    }
    /* X690 Clause 8.5.7: it is a Binary value */
    else if (input[0] & 0x80)
    {
        res = decode_real_binary(input, output);
    }
    /* X690 Clause 8.5.8: it is a Decimal value */
    else if ((input[0] & 0xC0) == 0)
    {
        res = decode_real_decimal(input, output);
    }

    /* Swap bytes */
    if (res)
    {
        std::memcpy(buffer.data(), &output, sizeof(double));
        std::reverse(buffer.begin(), buffer.end());
        std::memcpy(&output, buffer.data(), sizeof(double));
    }

    return res;
}

/**
 * Decodes a real special value
 * @param input BER encoded real special value
 * @param output decoded special value
 * @return true on success, false on failure
 */
inline bool decode_real_special(absl::Span<const uint8_t> input, double& output) noexcept
{
    bool res = false;

    if (input.length() == 1)
    {
        switch (input[0])
        {
            /* Plus infinity value*/
            case 0x40:
                output = std::numeric_limits<double>::infinity();
                res = true;
                break;
            /* Minus infinity value */
            case 0x41:
                output = -std::numeric_limits<double>::infinity();
                res = true;
                break;
            /* Not A Number value */
            case 0x42:
                output = std::numeric_limits<double>::quiet_NaN();
                res = true;
                break;
            /* Minus Zero value */
            case 0x43:
                output = -0.0;
                res = true;
                break;
            /* Unknown value */
            default:
                break;
        }
    }

    return res;
}

/**
 * Decodes a real binary value
 * @param input BER encoded real binary value
 * @param output decoded binary value
 * @return true on success, false on failure
 */
inline bool decode_real_binary(absl::Span<const uint8_t> input, double& output) noexcept
{
    if (!check_real_binary(input))
    {
        return false;
    }

    size_t exponent_len = (input[0] & 0x3) + 1;
    size_t exponent_offset = 1;

    /* X690 Clause 8.5.7.4.d */
    if (exponent_len == 4)
    {
        exponent_len = static_cast<size_t>(input[1]);
        exponent_offset = 2;
    }

    /* Verify that the exponent fits into an int */
    if (exponent_len > sizeof(int))
    {
        return false;
    }

    /* Get exponent sign */
    int exponent = (input[exponent_offset] & 0x80) ? -1 : 0;

    /* X690 Clause 8.5.7.1 */
    int mantissa_sign = (input[0] & 0x40) ? -1 : 1;

    /* X690 Clause 8.5.7.2 */
    int base = (input[0] & 0x30) >> 4;

    /* X690 Clause 8.5.7.3 */
    int factor = (input[0] & 0x0C) >> 2;

    /* Read exponent */
    for (size_t i = 0 ; i < exponent_offset ; ++i)
    {
        exponent = (exponent << 8) | input[exponent_offset + i];
    }

    /* Convert base 8 exponent to base 2 */
    if (base == 1)
    {
        if (exponent > 0 ? exponent <= std::numeric_limits<int>::max() / 3 : exponent >= std::numeric_limits<int>::min() / 3)
        {
            exponent *= 3;
        }
        else
        {
            return false;
        }
    }
    /* Convert base 16 exponent to base 2 */
    if (base == 2)
    {
        if (exponent > 0 ? exponent <= std::numeric_limits<int>::max() / 4 : exponent >= std::numeric_limits<int>::min() / 4)
        {
            exponent *= 4;
        }
        else
        {
            return false;
        }
    }

    /* Apply binary factor */
    if (exponent <= (std::numeric_limits<int>::max() - factor))
    {
        exponent += factor;
    }
    else {
        return false;
    }

    /* If exponent overflows, check */
    if (exponent < -0xFFFFFF || exponent > 0xFFFFFF)
    {
        /* Exponent too large for a double */
        if (exponent >= 0)
            output = mantissa_sign >= 0 ? std::numeric_limits<double>::infinity() : -std::numeric_limits<double>::infinity();
        /* Exponent too small for a double */
        else
            output = mantissa_sign >= 0 ? 0.0 : -0.0;

        return true;
    }

    /* Get Mantissa into a buffer
     * One more byte is read because there could be a 3 or 4 bits shift, see X690 Clause 8.5.7
     * */
    std::array<uint8_t, 9> double_bytes = {};
    size_t                 mantissa_offset = exponent_len + exponent_offset;
    size_t                 mantissa_len = input.length() - mantissa_offset;

    /* Skip leading zeros */
    while (mantissa_len > 1 && input[mantissa_offset] == 0)
    {
        mantissa_offset++;
        mantissa_len--;
    }

    /* Put mantissa into bytes 1 to 7 and adapt the exponent */
    for (size_t i = 0 ; i < 8 ; i++)
    {
        if (i < mantissa_len)
        {
            double_bytes[i + 1] = input[mantissa_offset + i];
        }
    }

    /* IEEE 754 double format is: | 1 bit Sign | 11 bits exponent | 52 bits mantissa |
     * Exponent bias is 2^(k-1) - 1 where k = exponent size in bits. Here bias = 1023
     * Add mantissa size to the bias to get an exponent corresponding to the mantissa
     * bias = 1023 + 52 = 1075
     * */
    exponent += ((mantissa_len - 7) << 3) + 1075;

    /* For a normalized value, the mantissa size is 52 bits with an implicit hidden bit which equals 1
     * Set double_bytes to have b0001 in the first 4 bits of the first double byte
     * See X690 Clause 8.5.7 */
    if ((double_bytes[1] & 0xE0) != 0 && exponent < 0x7FF)
    {
        /* Mantissa is too big - shift right up to 3 bits */
        int shift = 0;
        unsigned int first_byte = double_bytes[1];

        /* Adapt exponent */
        while ((first_byte & 0xE0) != 0 && exponent < (0x7FF - shift))
        {
            first_byte >>= 1;
            shift++;
        }
        exponent += shift;

        /* Adapt double bytes */
        for (int j = 7 ; j > 0 ; --j)
        {
            unsigned int next_byte_shifted = static_cast<unsigned int>(double_bytes[j - 1]) << 8;
            double_bytes[j] = (next_byte_shifted | double_bytes[j]) >> shift;
        }

    }
    else if ((double_bytes[1] & 0xF0u) == 0  && exponent > 0)
    {
        /* Mantissa is too small - shift left up to 4 bits */
        int shift = 8;
        unsigned int first_byte = double_bytes[1];

        /* Adapt exponent */
        while ((first_byte & 0xF0) == 0 && exponent > (8 - shift))
        {
            exponent -= 8 - shift;
        }

        for (int j = 1 ; j < 8 ; ++j)
        {
            unsigned int first_byte_shifted = static_cast<unsigned int>(double_bytes[j]) << 8;
            double_bytes[j] = (first_byte_shifted | double_bytes[j + 1]) >> shift;
        }
    }

    /* Verify that exponent does not overflow after modification */
    if (exponent >= 0x7FF || exponent <= -55)
    {
        /* Exponent too large for a double */
        if (exponent >= 0)
            output = mantissa_sign >= 0 ? std::numeric_limits<double>::infinity() : -std::numeric_limits<double>::infinity();
            /* Exponent too small for a double */
        else
            output = mantissa_sign >= 0 ? 0.0 : -0.0;

        return true;
    }

    /* Normalize the value if necessary */
    if (exponent <= 0)
    {
        /* correct the exponent to shift one bit more right */
        --exponent;

        /* Shift the mantissa right until exponent is 0 */
        int shift = (-exponent) >> 3;
        for (int j = 7 ; j > shift ; --j)
        {
            double_bytes[j] = double_bytes[j - shift];
        }
        for (int j = shift ; j > 0; --j)
        {
            double_bytes[j] = 0;
        }
        /* shift right bit by bit */
        shift = -exponent & 0x07;
        for (int j = 7 ; j > 0 ; --j)
        {
            unsigned int next_byte_shifted = static_cast<unsigned int>(double_bytes[j - 1]) << 8;
            double_bytes[j] = (next_byte_shifted | double_bytes[j]) >> shift;
        }

        /* exponent for a denormalized value */
        exponent = 0;
    }

    /* Finally add the mantissa sign and the exponent to the double bytes */
    double_bytes[0] = (mantissa_sign << 7) | (exponent >> 4);
    double_bytes[1] = (exponent << 4) | (double_bytes[1] & 0x0F);

    memcpy(&output, double_bytes.data(), sizeof(double));
    return true;
}

/**
 * Decodes a real decimal value
 * @param input BER encoded real decimal value
 * @param output decoded decimal value
 * @return true on success, false on failure
 */
inline bool decode_real_decimal(absl::Span<const uint8_t> input, double& output) noexcept
{
    std::string iso_6093_value(reinterpret_cast<const char*>(&input[1]), input.length() - 1);

    /* Check that string matches the iso 6093 NR1/NR2/NR3 forms */
    std::regex iso_6093_nr1 { "^[ ]*[+-]?[0-9]+$" };
    std::regex iso_6093_nr2 { "^[ ]*[+-]?([0-9]+[\\.\\,]{1}[0-9]*|[0-9]*[\\.\\,]{1}[0-9]+)$" };
    std::regex iso_6093_nr3 { "^[ ]*[+-]?([0-9]+[\\.\\,]{1}[0-9]*|[0-9]*[\\.\\,]{1}[0-9]+)[Ee][+-]?[0-9]+$" };

    if (std::regex_match(iso_6093_value, iso_6093_nr1) || std::regex_match(iso_6093_value, iso_6093_nr2) || std::regex_match(iso_6093_value, iso_6093_nr3))
    {
        /* Replace potential comma by a dot */
        std::replace(iso_6093_value.begin(), iso_6093_value.end(), ',', '.');

        return absl::SimpleAtod(iso_6093_value, &output);
    }
    else
    {
        return false;
    }
}


inline size_t encode_real(absl::Span<uint8_t> output, double input) noexcept
{
    /* Reverse bytes */
    std::array<uint8_t, sizeof(double)> buffer;
    std::memcpy(buffer.data(), &input, sizeof(double));
    std::reverse(buffer.begin(), buffer.end());

    unsigned int sign = buffer[0] >> 7;
    size_t       encoded_length = 0;
    int          exponent = (buffer[0] & 0x7F) << 4 | (buffer[1] >> 4);

    /* Remove exponent */
    buffer[1] &= 0x0F;

    if (exponent == 0x7FF)
    {
        if (std::all_of(buffer.begin() + 1, buffer.end(), [](uint8_t elem){ return elem == 0; }))
        {
            if (sign == 0)
                /* Plus infinity */
                output[0] = 0x40;
            else
                /* Minus infinity */
                output[0] = 0x41;
        }
        else
        {
            /* Not a number value */
            output[0] = 0x42;
        }

        return 1;
    }
    else if (exponent == 0)
    {
        if (std::all_of(buffer.begin() + 1, buffer.end(), [](uint8_t elem){ return elem == 0; }))
        {
            if (sign == 0)
                /* Zero value - nothing to do */
                return 0;
            else
                /* Minus zero */
                output[0] = 0x43;
                return 1;
        }
        else
        {
            /* Denormalized value*/
            buffer[8] = 0;
            for (size_t i = 1 ; i < buffer.size() ; i++)
            {
                unsigned int current_byte_shifted = static_cast<unsigned int>(buffer[i]) << 8;
                buffer[i] = (current_byte_shifted | buffer[i + 1]) >> 7;
            }
            exponent -= 1075;
        }
    }
    else
    {
        /* Normalized value, implicit first bit */
        buffer[1] |= 0x10;
        exponent -= 1075;
    }

    /* Encode a binary real value into BER format */
    if (-128 <= exponent && exponent <= 127)
    {
        /* exponent on 1 byte */
        encoded_length = 1 + 1 + 7;

        /* Control byte bits: | 1 | S | BB | FF | EE |
         * S  = sign
         * BB = base 2 = 00
         * FF = 0      = 00
         * EE = 1 byte = 00
         * */
        output[0] = static_cast<uint8_t>(0x80 | (sign << 6));

        /* Assign exponent */
        output[1] = static_cast<uint8_t>(exponent);
    }
    else
    {
        /* exponent on two signed bytes */
        encoded_length = 1 + 2 + 7;

        /* Control byte bits: | 1 | S | BB | FF | EE |
         * S  = sign
         * BB = base 2  = 00
         * FF = 0       = 00
         * EE = 2 bytes = 01
         * */
        output[0] = static_cast<uint8_t>(0x81 | (sign << 6));

        /* Assign exponent */
        output[1] = static_cast<uint8_t>(exponent >> 8);
        output[2] = static_cast<uint8_t>(exponent & 0xFF);
    }

    /* Mantissa on 7 bytes */
    for (size_t i = 1 ; i < 8 ; ++i)
    {
        output[encoded_length + i - 8] = buffer[i];
    }

    return encoded_length;
}


} // namespace fast_ber
