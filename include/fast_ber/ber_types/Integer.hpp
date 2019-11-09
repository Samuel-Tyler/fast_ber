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
    Integer() noexcept : m_data{0x01, 0x00} {}
    Integer(int64_t num) noexcept { assign(num); }
    Integer(BerView& view) noexcept { assign_ber(view); }
    template <typename Identifier2>
    Integer(const Integer<Identifier2>& rhs) noexcept;

    explicit Integer(absl::Span<const uint8_t> ber_data) noexcept { assign_ber(ber_data); }

    int64_t value() const noexcept;

    Integer<Identifier>& operator=(int64_t rhs) noexcept;
    template <typename Identifier2>
    Integer<Identifier>& operator=(const Integer<Identifier2>& rhs) noexcept;
    Integer<Identifier>& operator=(const BerView& rhs) noexcept;

    template <typename Identifier2>
    bool operator==(const Integer<Identifier2>& rhs) const
    {
        return this->value() == rhs.value();
    }

    template <typename Identifier2>
    bool operator!=(const Integer<Identifier2>& rhs) const
    {
        return !(*this == rhs);
    }

    bool operator==(int64_t rhs) const { return this->value() == rhs; }
    bool operator!=(int64_t rhs) const { return !(*this == rhs); }

    void assign(int64_t val) noexcept;

    template <typename Identifier2>
    void   assign(const Integer<Identifier2>& rhs) noexcept;
    size_t assign_ber(const BerView& rhs) noexcept;
    size_t assign_ber(absl::Span<const uint8_t> buffer) noexcept;

    EncodeResult encode_content_and_length(absl::Span<uint8_t> buffer) const noexcept;

    using Id = Identifier;

    template <typename Identifier2>
    friend class Integer;

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

template <typename Identifier>
template <typename Identifier2>
Integer<Identifier>::Integer(const Integer<Identifier2>& rhs) noexcept : m_data(rhs.m_data)
{
}

template <typename Identifier>
inline int64_t Integer<Identifier>::value() const noexcept
{
    int64_t ret = 0;
    decode_integer(absl::MakeSpan(m_data.data() + 1, content_length()), ret);
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
    set_content_length(encode_integer(absl::Span<uint8_t>(m_data.data() + 1, m_data.size() - 1), val));
}

template <typename Identifier>
template <typename Identifier2>
inline void Integer<Identifier>::assign(const Integer<Identifier2>& rhs) noexcept
{
    m_data = rhs.m_data;
}

template <typename Identifier>
inline size_t Integer<Identifier>::assign_ber(const BerView& view) noexcept
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

template <typename Identifier>
inline size_t Integer<Identifier>::assign_ber(absl::Span<const uint8_t> buffer) noexcept
{
    return assign_ber(BerView(buffer));
}

template <typename Identifier>
inline EncodeResult Integer<Identifier>::encode_content_and_length(absl::Span<uint8_t> buffer) const noexcept
{
    if (buffer.size() < encoded_length())
    {
        return EncodeResult{false, 0};
    }

    std::copy(m_data.begin(), m_data.begin() + encoded_length(), buffer.data());
    return EncodeResult{true, encoded_length()};
}

template <typename DefaultIdentifier, typename ID = DefaultIdentifier>
EncodeResult encode(absl::Span<uint8_t> output, const Integer<DefaultIdentifier>& object, const ID& id = ID{})
{
    return encode_impl(output, object, id);
}

template <typename DefaultIdentifier, typename ID = DefaultIdentifier>
DecodeResult decode(BerViewIterator& input, Integer<DefaultIdentifier>& output, const ID& id = ID{}) noexcept
{
    return decode_impl(input, output, id);
}

template <typename Identifier>
EncodeResult encode_content_and_length(absl::Span<uint8_t> output, const Integer<Identifier>& object) noexcept
{
    return object.encode_content_and_length(output);
}

template <typename Identifier>
DecodeResult decode_content_and_length(BerViewIterator& input, Integer<Identifier>& output) noexcept
{
    (void)input;
    (void)output;

    return {};
}

template <typename Identifier>
std::ostream& operator<<(std::ostream& os, const Integer<Identifier>& object) noexcept
{
    return os << object.value();
}

} // namespace fast_ber
