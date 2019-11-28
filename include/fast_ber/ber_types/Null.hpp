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

template <typename Identifier = ExplicitId<UniversalTag::null>>
class Null
{
  public:
    Null() noexcept            = default;
    Null(const Null&) noexcept = default;
    Null(Null&&) noexcept      = default;
    explicit Null(std::nullptr_t) noexcept {}

    Null& operator=(const Null&) = default;
    Null& operator=(Null&&) = default;

    template <typename Identifier2>
    bool operator==(const Null<Identifier2>&) const noexcept
    {
        return true;
    }
    template <typename Identifier2>
    bool operator!=(const Null<Identifier2>&) const noexcept
    {
        return false;
    }
    std::nullptr_t value() { return nullptr; }

    size_t       assign_ber(const BerView& rhs) noexcept;
    size_t       assign_ber(absl::Span<const uint8_t> buffer) noexcept { return assign_ber(BerView(buffer)); }
    EncodeResult encode_content_and_length(absl::Span<uint8_t> buffer) const noexcept;

    using AsnId = Identifier;
};

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

template <typename Identifier>
constexpr size_t encoded_length(const Null<Identifier>&)
{
    return encoded_length(1, Identifier{});
}

template <typename Identifier>
EncodeResult encode(absl::Span<uint8_t> output, const Null<Identifier>& object)
{
    return encode_impl(output, object, Identifier{});
}

template <typename Identifier>
DecodeResult decode(BerViewIterator& input, Null<Identifier>& output) noexcept
{
    return decode_impl(input, output, Identifier{});
}

template <typename Identifier>
EncodeResult encode_content_and_length(absl::Span<uint8_t> output, const Null<Identifier>& object) noexcept
{
    return object.encode_content_and_length(output);
}

template <typename Identifier>
DecodeResult decode_content_and_length(BerViewIterator& input, Null<Identifier>& output) noexcept
{
    (void)input;
    (void)output;

    return {};
}

template <typename Identifier>
std::ostream& operator<<(std::ostream& os, const Null<Identifier>)
{
    return os << "null";
}

} // namespace fast_ber
