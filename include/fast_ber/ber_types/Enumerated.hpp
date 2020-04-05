#pragma once

#include "fast_ber/ber_types/Integer.hpp"
#include "fast_ber/util/DecodeHelpers.hpp"
#include "fast_ber/util/EncodeHelpers.hpp"

namespace fast_ber
{

template <typename EnumeratedType, typename Identifier = ExplicitId<UniversalTag::enumerated>>
class Enumerated
{
  public:
    static_assert(std::is_enum<EnumeratedType>::value, "Must be enum");

    Enumerated() noexcept : m_val() {}
    Enumerated(EnumeratedType val) noexcept { assign(val); }
    template <typename Identifier2>
    Enumerated(const Enumerated<EnumeratedType, Identifier2>& rhs) noexcept
    {
        assign(rhs);
    }
    explicit Enumerated(absl::Span<const uint8_t> ber_data) noexcept { assign_ber(ber_data); }

    using Values = EnumeratedType;

    EnumeratedType value() const noexcept { return static_cast<EnumeratedType>(m_val.value()); }

    template <typename Identifier2>
    Enumerated& operator=(const Enumerated<EnumeratedType, Identifier2>& rhs) noexcept;
    Enumerated& operator=(EnumeratedType rhs) noexcept
    {
        assign(rhs);
        return *this;
    }

    template <typename Identifier2>
    bool operator==(const Enumerated<EnumeratedType, Identifier2>& rhs) const
    {
        return this->value() == rhs.value();
    }

    template <typename Identifier2>
    bool operator!=(const Enumerated<EnumeratedType, Identifier2>& rhs) const
    {
        return !(*this == rhs);
    }

    bool operator==(EnumeratedType rhs) const { return this->value() == rhs; }
    bool operator!=(EnumeratedType rhs) const { return !(*this == rhs); }

    template <typename Identifier2>
    void assign(const Enumerated<EnumeratedType, Identifier2>& rhs) noexcept
    {
        m_val.assign(rhs.m_val);
    }
    void   assign(EnumeratedType val) noexcept { m_val.assign(static_cast<int64_t>(val)); }
    size_t assign_ber(const BerView& rhs) noexcept { return m_val.assign_ber(rhs); }

    using AsnId = Identifier;

    template <typename EnumeratedType2, typename Identifier2>
    friend class Enumerated;

    const Integer<Identifier>& container() const noexcept { return m_val; }
    Integer<Identifier>&       container() noexcept { return m_val; }

    size_t       encoded_length() const noexcept;
    EncodeResult encode(absl::Span<uint8_t> output) const noexcept;
    DecodeResult decode(absl::Span<const uint8_t> input) noexcept;

  private:
    Integer<Identifier> m_val;
};

template <typename EnumeratedType, typename Identifier>
size_t Enumerated<EnumeratedType, Identifier>::encoded_length() const noexcept
{
    return this->m_val.encoded_length();
}

template <typename EnumeratedType, typename Identifier>
EncodeResult Enumerated<EnumeratedType, Identifier>::encode(absl::Span<uint8_t> output) const noexcept
{
    return this->m_val.encode(output);
}

template <typename EnumeratedType, typename Identifier>
DecodeResult decode(BerViewIterator& input, Enumerated<EnumeratedType, Identifier>& output) noexcept
{
    return decode(input, output.container());
}

template <typename EnumeratedType, typename Identifier>
const char* to_string(const Enumerated<EnumeratedType, Identifier>& obj)
{
    return to_string(obj.value());
}

template <typename EnumeratedType, typename Identifier>
std::ostream& operator<<(std::ostream& os, const Enumerated<EnumeratedType, Identifier>& obj) noexcept
{
    return os << '"' << obj.value() << '"';
}

} // namespace fast_ber
