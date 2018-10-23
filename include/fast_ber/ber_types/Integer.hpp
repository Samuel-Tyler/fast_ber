#pragma once

#include "absl/types/span.h"

#include <cstdint>

namespace fast_ber
{

class Integer
{
  public:
    Integer(absl::Span<const uint8_t> ber_data)
    {
        //  decode();
    }

    Integer(int64_t num) {}

    Integer() : Integer(0) {}

    int64_t value() const;
    void    set(int64_t val);

    size_t encode(absl::Span<const uint8_t> buffer)
    {
        if (buffer.length() <= m_data.size())
        {
            std::copy(buffer.begin(), buffer.end(), m_data.data());
        }
        else
        {
            return 0;
        }
    }

  private:
    size_t                  m_size;
    std::array<uint8_t, 12> m_data;
};

inline bool decode(absl::Span<const uint8_t> input, int64_t& output)
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

constexpr inline size_t encode(absl::Span<uint8_t> output, int64_t input)
{
    /*    for (int i = 0; i < 8; i++)
        {
            uint8_t top_byte = (input & 0xFF00000000000000) >> (64 - 8);
            if (!= 0)
            {
            }
        }   */
}

} // namespace fast_ber
