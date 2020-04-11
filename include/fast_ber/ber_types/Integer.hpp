#pragma once

#include "absl/types/span.h"
#include "fast_ber/ber_types/Class.hpp"
#include "fast_ber/ber_types/Construction.hpp"
#include "fast_ber/util/BerView.hpp"
#include "fast_ber/util/DecodeHelpers.hpp"
#include "fast_ber/util/EncodeHelpers.hpp"
#include "fast_ber/util/EncodeIdentifiers.hpp"
#include "fast_ber/util/Extract.hpp"
#include "fast_ber/util/SmallFixedIdBerContainer.hpp"

#include <algorithm>
#include <array>
#include <cstdint>
#include <cstring>
#include <iosfwd>
#include <limits>

namespace fast_ber
{

inline bool   decode_integer(absl::Span<const uint8_t> input, int64_t& output) noexcept;
inline size_t encode_integer(absl::Span<uint8_t> output, int64_t input) noexcept;

template <typename Identifier = ExplicitId<UniversalTag::integer>>
class Integer
{
  public:
    Integer() noexcept : m_contents{{0x00}, ConstructionMethod::construct_with_provided_content} {}
    Integer(int64_t num) noexcept { assign(num); }
    Integer(BerView view) noexcept { decode(view); }
    template <typename Identifier2>
    Integer(const Integer<Identifier2>& rhs) noexcept;
    Integer(const Integer& rhs) noexcept = default;
    Integer(Integer&& rhs) noexcept      = default;
    ~Integer() noexcept                  = default;

    int64_t value() const noexcept;

    Integer& operator=(int64_t rhs) noexcept;
    Integer& operator=(Integer&& rhs) noexcept = default;
    Integer& operator=(const Integer& rhs) noexcept = default;
    template <typename Identifier2>
    Integer<Identifier>& operator=(const Integer<Identifier2>& rhs) noexcept;

    template <typename Identifier2>
    bool operator==(const Integer<Identifier2>& rhs) const noexcept
    {
        return this->value() == rhs.value();
    }

    template <typename Identifier2>
    bool operator!=(const Integer<Identifier2>& rhs) const noexcept
    {
        return !(*this == rhs);
    }

    bool operator==(int64_t rhs) const { return this->value() == rhs; }
    bool operator!=(int64_t rhs) const { return !(*this == rhs); }

    using AsnId = Identifier;

    void assign(int64_t val) noexcept;

    template <typename Identifier2>
    void assign(const Integer<Identifier2>& rhs) noexcept;

    size_t       encoded_length() const noexcept { return m_contents.ber_length(); }
    EncodeResult encode(absl::Span<uint8_t> buffer) const noexcept { return m_contents.encode(buffer); }
    DecodeResult decode(BerView rhs) noexcept { return m_contents.decode(rhs); }

    template <typename Identifier2>
    friend class Integer;

  private:
    SmallFixedIdBerContainer<Identifier, sizeof(int64_t)> m_contents;
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

    std::array<uint8_t, sizeof(int64_t)> buffer{};
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
    std::array<uint8_t, sizeof(int64_t)> buffer{};
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

    size_t encoded_len = sizeof(int64_t) - to_skip;
    if (output.size() < encoded_len)
    {
        return 0;
    }

    std::memcpy(output.data(), buffer.data() + to_skip, encoded_len);
    return encoded_len;
}

template <typename Identifier>
template <typename Identifier2>
Integer<Identifier>::Integer(const Integer<Identifier2>& rhs) noexcept : m_contents(rhs.m_contents)
{
}

template <typename Identifier>
inline int64_t Integer<Identifier>::value() const noexcept
{
    int64_t ret = 0;
    decode_integer(m_contents.content(), ret);
    return ret;
}

template <typename Identifier>
inline Integer<Identifier>& Integer<Identifier>::operator=(int64_t rhs) noexcept
{
    assign(rhs);
    return *this;
}

template <typename Identifier>
template <typename Identifier2>
inline Integer<Identifier>& Integer<Identifier>::operator=(const Integer<Identifier2>& rhs) noexcept
{
    assign(rhs);
    return *this;
}

template <typename Identifier>
inline void Integer<Identifier>::assign(int64_t val) noexcept
{
    m_contents.resize_content(encode_integer(absl::Span<uint8_t>(m_contents.content_data(), sizeof(int64_t)), val));
}

template <typename Identifier>
template <typename Identifier2>
inline void Integer<Identifier>::assign(const Integer<Identifier2>& rhs) noexcept
{
    m_contents = rhs.m_contents;
}

template <typename Identifier>
DecodeResult decode(BerViewIterator& input, Integer<Identifier>& output) noexcept
{
    DecodeResult res = output.decode(*input);
    ++input;
    return res;
}

template <typename Identifier>
std::ostream& operator<<(std::ostream& os, const Integer<Identifier>& object) noexcept
{
    return os << object.value();
}

} // namespace fast_ber
