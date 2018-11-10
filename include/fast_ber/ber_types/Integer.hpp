#pragma once

#include "absl/types/span.h"
#include "fast_ber/ber_types/Class.hpp"
#include "fast_ber/ber_types/Construction.hpp"
#include "fast_ber/util/BerView.hpp"
#include "fast_ber/util/Create.hpp"
#include "fast_ber/util/Extract.hpp"

#include <algorithm>
#include <cstdint>
#include <cstring>

namespace fast_ber
{

inline bool   decode_integer(absl::Span<const uint8_t> input, int64_t& output) noexcept;
inline size_t encode_integer(absl::Span<uint8_t> output, int64_t input) noexcept;

class Integer
{
  public:
    Integer() noexcept : Integer(0) {}
    Integer(int64_t num) noexcept { assign(num); }
    Integer(BerView& view) noexcept { assign_ber(view); }

    explicit Integer(absl::Span<const uint8_t> ber_data) noexcept { assign_ber(ber_data); }

    // Implicit conversion to int
            operator int64_t() const noexcept { return value(); }
    int64_t value() const noexcept;

    Integer& operator=(int64_t rhs) noexcept;
    Integer& operator=(const Integer& rhs) noexcept;
    Integer& operator=(const BerView& rhs) noexcept;
    void     assign(int64_t val) noexcept;
    void     assign(const Integer& rhs) noexcept;
    size_t   assign_ber(const BerView& rhs) noexcept;
    size_t   assign_ber(absl::Span<const uint8_t> buffer) noexcept;

    size_t encode_with_specific_id(absl::Span<uint8_t> buffer, Class, Tag tag) const noexcept;

  private:
    void    set_content_length(uint8_t length) noexcept { m_data[0] = length; }
    uint8_t content_length() const noexcept { return m_data[0]; }
    size_t  encoded_length() const noexcept { return 1 + content_length(); }

    std::array<uint8_t, sizeof(int64_t) + sizeof(uint8_t)> m_data;
};

inline bool decode_integer(absl::Span<const uint8_t> input, int64_t& output) noexcept
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

    std::array<uint8_t, sizeof(int64_t)> buffer;
    const auto                           offset = sizeof(int64_t) - input.size();
    memcpy(buffer.data(), &output, sizeof(int64_t));
    for (size_t i = 0; i < input.size(); i++)
    {
        buffer[7 - (offset + i)] = input[i];
    }
    memcpy(&output, buffer.data(), sizeof(int64_t));

    return true;
}

inline size_t encode_integer(absl::Span<uint8_t> output, int64_t input) noexcept
{
    std::array<uint8_t, sizeof(int64_t)> buffer;
    std::memcpy(buffer.data(), &input, sizeof(int64_t));
    std::reverse(buffer.begin(), buffer.end());

    size_t to_skip = 0;
    for (size_t i = 0; i < sizeof(int64_t) - 1; i++)
    {
        if (buffer[i] == 0 && !(buffer[i + 1] & 0x80))
        {
            to_skip++;
        }
        else
        {
            break;
        }
    }

    for (size_t i = 0; i < sizeof(int64_t) - 1; i++)
    {
        if (buffer[i] == 0xFF && (buffer[i + 1] & 0x80))
        {
            to_skip++;
        }
        else
        {
            break;
        }
    }

    size_t encoded_length = sizeof(int64_t) - to_skip;
    if (output.size() < encoded_length)
    {
        return 0;
    }

    std::memcpy(output.data(), buffer.data() + to_skip, encoded_length);
    return encoded_length;
}

inline int64_t Integer::value() const noexcept
{
    int64_t ret = 0;
    decode_integer(absl::MakeSpan(m_data.data() + 1, content_length()), ret);
    return ret;
}

inline Integer& Integer::operator=(int64_t rhs) noexcept
{
    assign(rhs);
    return *this;
}

inline Integer& Integer::operator=(const Integer& rhs) noexcept
{
    assign(rhs);
    return *this;
}

inline void Integer::assign(int64_t val) noexcept
{
    set_content_length(encode_integer(absl::Span<uint8_t>(m_data.data() + 1, m_data.size() - 1), val));
}

inline void Integer::assign(const Integer& rhs) noexcept { m_data = rhs.m_data; }

inline size_t Integer::assign_ber(const BerView& view) noexcept
{
    if (!view.is_valid() || view.construction() != Construction::primitive)
    {
        return false;
    }
    if (view.ber_length() - view.identifier_length() > m_data.size())
    {
        return false;
    }

    std::copy(view.ber_data() + view.identifier_length(), view.ber_data() + view.ber_length(), m_data.begin());
    return view.ber_length();
}

inline size_t Integer::assign_ber(absl::Span<const uint8_t> buffer) noexcept { return assign_ber(BerView(buffer)); }

inline size_t Integer::encode_with_specific_id(absl::Span<uint8_t> buffer, Class class_, Tag tag) const noexcept
{
    size_t tag_length = create_identifier(buffer, Construction::primitive, class_, tag);
    if (tag_length == 0)
    {
        return 0;
    }

    buffer.remove_prefix(tag_length);

    if (buffer.size() < encoded_length())
    {
        return 0;
    }

    std::copy(m_data.begin(), m_data.begin() + encoded_length(), buffer.data());
    return tag_length + encoded_length();
}

} // namespace fast_ber
