#pragma once

#include "absl/types/span.h"

#include "fast_ber/util/BerView.hpp"
#include "fast_ber/util/DecodeHelpers.hpp"
#include "fast_ber/util/EncodeHelpers.hpp"

#include <algorithm>
#include <array>
#include <cstdint>

namespace fast_ber
{

template <typename Identifier = ExplicitIdentifier<UniversalTag::null>>
class Null
{
  public:
    Null() noexcept            = default;
    Null(const Null&) noexcept = default;
    Null(Null&&) noexcept      = default;
    explicit Null(std::nullptr_t) noexcept {}

    bool           operator==(const Null&) const noexcept { return true; }
    bool           operator!=(const Null&) const noexcept { return false; }
    std::nullptr_t value() { return nullptr; }

    size_t       assign_ber(const BerView& rhs) noexcept;
    size_t       assign_ber(absl::Span<const uint8_t> buffer) noexcept { return assign_ber(BerView(buffer)); }
    EncodeResult encode_content_and_length(absl::Span<uint8_t> buffer) const noexcept;
};

template <typename Identifier>
constexpr Identifier identifier(const Null<Identifier>*) noexcept
{
    return {};
}

template <typename Identifier>
size_t Null<Identifier>::assign_ber(const BerView& rhs) noexcept
{
    if (rhs.is_valid())
    {
        return rhs.ber_length();
    }
    else
    {
        return 0;
    }
}

template <typename Identifier>
EncodeResult Null<Identifier>::encode_content_and_length(absl::Span<uint8_t> buffer) const noexcept
{
    if (buffer.length() > 0)
    {
        buffer[0] = 0x00;
        return EncodeResult{true, 1};
    }
    else
    {
        return EncodeResult{false, 0};
    }
}

template <typename DefaultIdentifier, typename ID = DefaultIdentifier>
EncodeResult encode(absl::Span<uint8_t> output, const Null<DefaultIdentifier>& object, const ID& id = ID{})
{
    return encode_impl(output, object, id);
}

template <typename DefaultIdentifier, typename ID = DefaultIdentifier>
DecodeResult decode(BerViewIterator& input, Null<DefaultIdentifier>& output, const ID& id = {}) noexcept
{
    return decode_impl(input, output, id);
}

} // namespace fast_ber
