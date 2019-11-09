#pragma once

#include "fast_ber/ber_types/Integer.hpp"
#include "fast_ber/util/DecodeHelpers.hpp"
#include "fast_ber/util/EncodeHelpers.hpp"

namespace fast_ber
{

template <typename T, typename T2 = std::enable_if<std::is_enum<T>::type>>
ExplicitId<UniversalTag::enumerated> identifier(const T*, IdentifierAdlToken = IdentifierAdlToken{}) noexcept
{
    return {};
}

template <typename T>
struct IdentifierType<std::enable_if<std::is_enum<T>::value, T>>
{
    using type = ExplicitId<UniversalTag::enumerated>;
};

template <typename Enumerated, typename ID = ExplicitId<UniversalTag::enumerated>,
          typename std::enable_if<std::is_enum<Enumerated>{}, int>::type = 0>
EncodeResult encode(absl::Span<uint8_t> output, const Enumerated& input, const ID& id = ID{})
{
    fast_ber::Integer<> i(static_cast<int64_t>(input));
    return encode(output, i, id);
}

template <typename Enumerated, typename ID = ExplicitId<UniversalTag::enumerated>,
          typename std::enable_if<std::is_enum<Enumerated>{}, int>::type = 0>
DecodeResult decode(BerViewIterator& input, Enumerated& output, const ID& id = ID{}) noexcept
{
    fast_ber::Integer<> i;
    DecodeResult        result = decode(input, i, id);
    if (result.success)
    {
        output = static_cast<Enumerated>(i.value());
        return DecodeResult{true};
    }
    else
    {
        return DecodeResult{false};
    }
}

template <typename Enumerated, typename std::enable_if<std::is_enum<Enumerated>{}, int>::type = 0>
EncodeResult encode_content_and_length(absl::Span<uint8_t> output, Enumerated& input) noexcept
{
    fast_ber::Integer<> i(static_cast<int64_t>(input));
    return encode_content_and_length(output, i);
}

template <typename Enumerated, typename std::enable_if<std::is_enum<Enumerated>{}, int>::type = 0>
DecodeResult decode_content_and_length(BerViewIterator& input, Enumerated& output) noexcept
{
    (void)input;
    (void)output;

    /* fast_ber::Integer<> i;
     DecodeResult        result = decode(input, i, id);
     if (result.success)
     {
         output = static_cast<Enumerated>(i.value());
         return DecodeResult{true};
     }
     else
     {
         return DecodeResult{false};
     }*/
    return {};
}

template <typename Enumerated, typename std::enable_if<std::is_enum<Enumerated>{}, int>::type = 0>
constexpr inline ExplicitId<UniversalTag::enumerated> identifier(const Enumerated*,
                                                                 IdentifierAdlToken = IdentifierAdlToken{}) noexcept
{
    return {};
}

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
        assign(static_cast<int64_t>(rhs));
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

    EncodeResult encode_content_and_length(absl::Span<uint8_t> buffer) const noexcept
    {
        return m_val.encode_content_and_length(buffer);
    }

    using Id = Identifier;

    template <typename EnumeratedType2, typename Identifier2>
    friend class Enumerated;

  private:
    Integer<Identifier> m_val;
};

template <typename EnumeratedType, typename Identifier, typename ID = Identifier>
EncodeResult encode(absl::Span<uint8_t> output, const Enumerated<EnumeratedType, Identifier>& object,
                    const ID& id = ID{})
{
    return encode_impl(output, object, id);
}

template <typename EnumeratedType, typename Identifier, typename ID = Identifier>
DecodeResult decode(BerViewIterator& input, Enumerated<EnumeratedType, Identifier>& output,
                    const ID& id = ID{}) noexcept
{
    return decode_impl(input, output, id);
}

template <typename EnumeratedType, typename Identifier>
std::ostream& operator<<(std::ostream& os, const Enumerated<EnumeratedType, Identifier>& obj) noexcept
{
    return os << '"' << obj.value() << '"';
}

} // namespace fast_ber
