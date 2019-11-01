#pragma once

#include "fast_ber/ber_types/Integer.hpp"
#include "fast_ber/util/DecodeHelpers.hpp"
#include "fast_ber/util/EncodeHelpers.hpp"

namespace fast_ber
{

template <typename T, typename T2 = std::enable_if<std::is_enum<T>::type>>
ExplicitIdentifier<UniversalTag::enumerated> identifier(const T*, IdentifierAdlToken = IdentifierAdlToken{}) noexcept
{
    return {};
}

template <typename Enumerated, typename ID = ExplicitIdentifier<UniversalTag::enumerated>,
          typename std::enable_if<std::is_enum<Enumerated>{}, int>::type = 0>
EncodeResult encode(absl::Span<uint8_t> output, const Enumerated& input, const ID& id = ID{})
{
    fast_ber::Integer<> i(static_cast<int64_t>(input));
    return encode(output, i, id);
}

template <typename Enumerated, typename ID = ExplicitIdentifier<UniversalTag::enumerated>,
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
constexpr inline ExplicitIdentifier<UniversalTag::enumerated>
identifier(const Enumerated*, IdentifierAdlToken = IdentifierAdlToken{}) noexcept
{
    return {};
}

} // namespace fast_ber
