#pragma once

#include "absl/container/inlined_vector.h"
#include "absl/types/span.h"
#include "absl/types/variant.h"

#include "fast_ber/ber_types/Class.hpp"
#include "fast_ber/ber_types/Tag.hpp"
#include "fast_ber/util/DecodeHelpers.hpp"
#include "fast_ber/util/EncodeHelpers.hpp"

#include <iosfwd>
#include <tuple>

namespace fast_ber
{

template <typename T0, typename... Args>
struct Choice : public absl::variant<T0, Args...>
{
    using Base = absl::variant<T0, Args...>;
    using Base::Base;
    using Base::operator=;

    Base&       base() { return *static_cast<Base*>(this); }
    const Base& base() const { return *static_cast<const Base*>(this); }

    Choice() : Base() {}
    Choice(const Choice& rhs) = default;
    Choice(Choice&& rhs)      = default;

    Choice& operator=(const Choice& rhs) = default;

    bool operator==(const Choice& rhs) const { return this->base() == rhs.base(); }
    bool operator!=(const Choice& rhs) const { return this->base() != rhs.base(); }

    template <typename T>
    bool operator==(const T& rhs) const
    {
        return base() == rhs;
    }
    template <typename T>
    bool operator!=(const T& rhs) const
    {
        return base() != rhs;
    }

    using Id = ExplicitId<UniversalTag::choice>;
};

template <typename T>
using variant_size = absl::variant_size<typename T::Base>;

template <std::size_t n, typename T>
using variant_alternative = absl::variant_alternative<n, typename T::Base>;

template <typename Visitor, typename... Variants>
auto visit(Visitor&& vis, const Choice<Variants...>& variant) -> decltype(absl::visit(vis, variant.base()))
{
    return absl::visit(vis, variant.base());
}

template <size_t index, size_t max_depth, typename... Variants,
          typename std::enable_if<(!(index < max_depth)), int>::type = 0>
EncodeResult encode_if(const absl::Span<uint8_t>&, const Choice<Variants...>&) noexcept
{
    // No substitutions found, fail
    return EncodeResult{false, 0};
}

template <size_t index, size_t max_depth, typename... Variants,
          typename std::enable_if<(index < max_depth), int>::type = 0>
EncodeResult encode_if(const absl::Span<uint8_t>& buffer, const Choice<Variants...>& choice) noexcept
{
    if (choice.index() == index)
    {
        const auto* child = absl::get_if<index>(&choice);
        assert(child);

        return encode(buffer, *child);
    }
    else
    {
        return encode_if<index + 1, max_depth>(buffer, choice);
    }
}

template <typename... Variants, typename ID = ExplicitId<UniversalTag::choice>>
EncodeResult encode_content_and_length(const absl::Span<uint8_t>& buffer, const Choice<Variants...>& choice) noexcept
{
    constexpr auto depth = static_cast<int>(fast_ber::variant_size<Choice<Variants...>>::value);
    return encode_if<0, depth>(buffer, choice);
}

template <typename... Variants, typename ID = ExplicitId<UniversalTag::choice>>
EncodeResult encode(const absl::Span<uint8_t>& buffer, const Choice<Variants...>& choice, ID id = ID{}) noexcept
{
    const auto header_length_guess = 2;
    auto       child_buffer        = buffer;
    child_buffer.remove_prefix(header_length_guess);

    const EncodeResult& inner_encode_result = encode_content_and_length(child_buffer, choice);
    if (!inner_encode_result.success)
    {
        return inner_encode_result;
    }
    return wrap_with_ber_header(buffer, inner_encode_result.length, id, header_length_guess);
}

template <int index, int max_depth, typename... Variants, typename ID,
          typename std::enable_if<(!(index < max_depth)), int>::type = 0>
DecodeResult decode_if(BerViewIterator&, Choice<Variants...>&, ID) noexcept
{
    // No substitutions found, fail
    return DecodeResult{false};
}

template <size_t index, size_t max_depth, typename... Variants, typename ID,
          typename std::enable_if<(index < max_depth), int>::type = 0>
DecodeResult decode_if(BerViewIterator& input, Choice<Variants...>& output, ID id) noexcept
{
    using T                 = typename fast_ber::variant_alternative<index, Choice<Variants...>>::type;
    constexpr auto child_id = Identifier<T>{};
    if (input->tag() == val(child_id.tag()))
    {
        T* child = &output.template emplace<index>();
        return decode(input, *child);
    }
    else
    {
        return decode_if<index + 1, max_depth>(input, output, id);
    }
}

template <typename... Variants, typename ID = ExplicitId<UniversalTag::choice>>
DecodeResult decode(BerViewIterator& input, Choice<Variants...>& output, ID id = ID{}) noexcept
{
    if (!input->is_valid() || input->tag() != val(id.tag()))
    {
        return DecodeResult{false};
    }

    auto child = input->begin();
    if (!child->is_valid())
    {
        return DecodeResult{false};
    }

    constexpr auto     depth  = fast_ber::variant_size<typename std::remove_reference<decltype(output)>::type>::value;
    const DecodeResult result = decode_if<0, depth>(child, output, id);
    ++input;
    return result;
}

template <typename... Variants>
std::ostream& operator<<(std::ostream& os, const Choice<Variants...>& variant);

struct OsVisitor
{
    template <typename T>
    void operator()(const T& t)
    {
        os << t;
    }

    std::ostream& os;
};

template <typename... Variants>
std::ostream& operator<<(std::ostream& os, const Choice<Variants...>& variant)
{
    OsVisitor visitor{os};

    fast_ber::visit(visitor, variant);
    return os;
}

} // namespace fast_ber
