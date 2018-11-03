#pragma once

#include "absl/types/span.h"

#include "fast_ber/ber_types/Class.hpp"
#include "fast_ber/ber_types/Construction.hpp"
#include "fast_ber/util/BerView.hpp"
#include "fast_ber/util/Create.hpp"
#include "fast_ber/util/EncodeResult.hpp"
#include "fast_ber/util/Extract.hpp"

#include <algorithm>
#include <cstdint>
#include <cstring>

namespace fast_ber
{

class Boolean
{
  public:
    Boolean() noexcept : Boolean(false) {}
    explicit Boolean(bool val) noexcept { assign(val); }
    explicit Boolean(BerView& view) noexcept { assign_ber(view); }
    explicit Boolean(absl::Span<const uint8_t> ber_data) noexcept { assign_ber(ber_data); }

    // Implicit conversion to bool
         operator bool() const noexcept { return value(); }
    bool value() const noexcept { return static_cast<bool>(m_data[1]); }

    Boolean& operator=(bool rhs) noexcept;
    Boolean& operator=(const Boolean& rhs) noexcept;
    Boolean& operator=(const BerView& rhs) noexcept;
    void     assign(bool val) noexcept;
    void     assign(const Boolean& rhs) noexcept;
    size_t   assign_ber(const BerView& rhs) noexcept;
    size_t   assign_ber(absl::Span<const uint8_t> buffer) noexcept;

    size_t encode_with_new_id(absl::Span<uint8_t> buffer, Class, Tag tag) const noexcept;

  private:
    std::array<uint8_t, 2> m_data;
}; // namespace fast_ber

inline EncodeResult encode_with_new_id(absl::Span<uint8_t>& output, const Boolean& object, Class class_, int tag)
{
    size_t encode_length = object.encode_with_new_id(output, class_, tag);
    return EncodeResult{encode_length > 0, encode_length};
}

inline Boolean& Boolean::operator=(bool rhs) noexcept
{
    assign(rhs);
    return *this;
}
inline Boolean& Boolean::operator=(const Boolean& rhs) noexcept
{
    assign(rhs);
    return *this;
}
inline Boolean& Boolean::operator=(const BerView& rhs) noexcept
{
    assign_ber(rhs);
    return *this;
}

inline void   Boolean::assign(bool val) noexcept { m_data = {0x1, static_cast<uint8_t>(val)}; }
inline void   Boolean::assign(const Boolean& rhs) noexcept { m_data = rhs.m_data; }
inline size_t Boolean::assign_ber(const BerView& rhs) noexcept
{
    if (!rhs.is_valid() || rhs.construction() != Construction::primitive || rhs.content_length() != 1 ||
        *(rhs.content_data()) > 1)
    {
        return 0;
    }

    m_data = {0x1, *(rhs.content_data())};
    return 2 + rhs.identifier_length();
}

inline size_t Boolean::assign_ber(absl::Span<const uint8_t> buffer) noexcept { return assign_ber(BerView(buffer)); }

inline size_t Boolean::encode_with_new_id(absl::Span<uint8_t> buffer, Class class_, Tag tag) const noexcept
{
    size_t tag_length = create_identifier(buffer, Construction::primitive, class_, tag);
    if (tag_length == 0)
    {
        return 0;
    }

    buffer.remove_prefix(tag_length);

    if (buffer.size() < m_data.size())
    {
        return 0;
    }

    buffer[0] = m_data[0];
    buffer[1] = m_data[1];
    return tag_length + m_data.size();
}

} // namespace fast_ber
