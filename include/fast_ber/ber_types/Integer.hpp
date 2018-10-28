#pragma once

#include "absl/types/span.h"

#include <algorithm>
#include <cstdint>
#include <iostream>

namespace fast_ber
{

inline bool   decode_ber_content(absl::Span<const uint8_t> input, int64_t& output) noexcept;
inline size_t encode_ber_content(absl::Span<uint8_t> output, int64_t input) noexcept;

class Integer
{
  public:
    Integer(absl::Span<const uint8_t> ber_data) noexcept { assign(ber_data); }
    Integer(int64_t num) noexcept { assign(num); }
    Integer() noexcept : Integer(0) {}

    int64_t value() const noexcept;

    void assign(int64_t val) noexcept
    {
        m_size = encode_ber_content(absl::Span<uint8_t>(m_data.data(), m_data.size()), val);
    }
    bool assign(absl::Span<const uint8_t> buffer) noexcept;

    size_t encode(absl::Span<uint8_t> buffer) const noexcept;

  private:
    size_t                  m_size;
    std::array<uint8_t, 12> m_data;
};

inline bool decode_ber_content(absl::Span<const uint8_t> input, int64_t& output) noexcept
{
    if (input.size() == 0 || input.size() > 8)
    {
        return false;
    }

    if (input[0] & 0x80)
    {
        output = -1;
    }
    else
    {
        output = 0;
    }

    const auto offset = 7 - input.size();
    for (size_t i = 0; i < input.size(); i++)
    {
        reinterpret_cast<uint8_t*>(&output)[offset - i] = input[i];
    }

    return true;
}

inline size_t encode_ber_content(absl::Span<uint8_t> output, int64_t input) noexcept
{
    std::cout << "encoding!\n";
    size_t i = 0;
    for (; i < sizeof(int64_t); i++)
    {
        output[i] = static_cast<uint8_t>(input % 0xFF);
        input /= 0xFF;

        if (input == 0)
        {
            std::reverse(output.begin(), output.begin() + i);
            return i;
        }
    }

    return 0;
}

inline int64_t Integer::value() const noexcept
{
    int64_t ret = 0;
    decode_ber_content(absl::Span<const uint8_t>(m_data.data(), m_size), ret);
    return ret;
}

inline bool Integer::assign(absl::Span<const uint8_t> buffer) noexcept
{
    if (buffer.length() > m_data.size())
    {
        return false;
    }

    std::copy(buffer.begin(), buffer.end(), m_data.data());
    return true;
}

inline size_t Integer::encode(absl::Span<uint8_t> buffer) const noexcept
{
    if (buffer.size() < m_size)
    {
        return 0;
    }

    std::copy(m_data.begin(), m_data.begin() + m_size, buffer.data());
    return m_size;
}

} // namespace fast_ber
