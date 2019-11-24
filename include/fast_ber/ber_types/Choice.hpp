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

inline bool is_an_identifier_choice(Class, Tag) { return false; }

template <typename Identifier, typename... Identifiers>
bool is_an_identifier_choice(Class class_, Tag tag, Identifier id, Identifiers... ids)
{
    if (id.check_id_match(class_, tag))
    {
        return true;
    }

    return is_an_identifier_choice(class_, tag, ids...);
}

template <typename... Identifiers>
struct ChoiceId
{
    static std::tuple<Identifiers...> choice_ids() { return {}; };
    constexpr static bool check_id_match(Class c, Tag t) { return is_an_identifier_choice(c, t, Identifiers{}...); }
};

inline void print(std::ostream&) noexcept {}

template <typename Identifier, typename... Identifiers>
void print(std::ostream& os, Identifier id, Identifiers... ids) noexcept
{
    os << id;

    if (sizeof...(ids) > 0)
    {
        os << ", ";
    }
    print(os, ids...);
}

template <typename... Identifiers>
inline std::ostream& operator<<(std::ostream& os, const ChoiceId<Identifiers...>&) noexcept
{
    os << "ChoiceId(";
    print(os, Identifiers{}...);
    return os << ")";
}

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

    template <typename... Variants>
    static constexpr auto identifier(Choice<Variants...>*) -> ChoiceId<Identifier<Variants>...>
    {
        return {};
    }

    using AsnId = decltype(identifier(static_cast<Choice<T0, Args...>*>(nullptr)));
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

template <typename... Variants>
EncodeResult encode_choice(const absl::Span<uint8_t>& buffer, const Choice<Variants...>& choice) noexcept
{
    constexpr auto depth = static_cast<int>(fast_ber::variant_size<Choice<Variants...>>::value);
    return encode_if<0, depth>(buffer, choice);
}

template <typename... Variants, typename ID>
EncodeResult encode(const absl::Span<uint8_t>& buffer, const Choice<Variants...>& choice, ID id) noexcept
{
    const auto header_length_guess = 2;
    auto       child_buffer        = buffer;
    child_buffer.remove_prefix(header_length_guess);

    const EncodeResult& inner_encode_result = encode_choice(child_buffer, choice);
    if (!inner_encode_result.success)
    {
        return inner_encode_result;
    }
    return wrap_with_ber_header(buffer, inner_encode_result.length, id, header_length_guess);
}

template <typename... Variants>
EncodeResult encode(const absl::Span<uint8_t>& buffer, const Choice<Variants...>& choice,
                    ChoiceId<Identifier<Variants>...> = {}) noexcept
{
    return encode_choice(buffer, choice);
}

template <int index, int max_depth, typename... Variants,
          typename std::enable_if<(!(index < max_depth)), int>::type = 0>
DecodeResult decode_if(BerViewIterator&, Choice<Variants...>&) noexcept
{
    // No substitutions found, fail
    return DecodeResult{false};
}

template <size_t index, size_t max_depth, typename... Variants,
          typename std::enable_if<(index < max_depth), int>::type = 0>
DecodeResult decode_if(BerViewIterator& input, Choice<Variants...>& output) noexcept
{
    using T = typename fast_ber::variant_alternative<index, Choice<Variants...>>::type;

    if (Identifier<T>::check_id_match(input->class_(), input->tag()))
    {
        T* child = &output.template emplace<index>();
        return decode(input, *child);
    }
    else
    {
        return decode_if<index + 1, max_depth>(input, output);
    }
}

template <typename... Variants>
DecodeResult decode(BerViewIterator& input, Choice<Variants...>& output,
                    ChoiceId<Identifier<Variants>...> = {}) noexcept
{
    constexpr auto     depth  = fast_ber::variant_size<typename std::remove_reference<decltype(output)>::type>::value;
    const DecodeResult result = decode_if<0, depth>(input, output);
    ++input;
    return result;
}

template <typename... Variants, typename ID>
DecodeResult decode(BerViewIterator& input, Choice<Variants...>& output, ID id) noexcept
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
    const DecodeResult result = decode_if<0, depth>(child, output);
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
