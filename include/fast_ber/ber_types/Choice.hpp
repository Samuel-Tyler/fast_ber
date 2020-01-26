#pragma once

#include "absl/container/inlined_vector.h"
#include "absl/types/span.h"
#include "absl/types/variant.h"

#include "fast_ber/ber_types/Class.hpp"
#include "fast_ber/ber_types/Tag.hpp"
#include "fast_ber/util/DecodeHelpers.hpp"
#include "fast_ber/util/DynamicVariant.hpp"
#include "fast_ber/util/EncodeHelpers.hpp"

#include <iosfwd>
#include <tuple>

namespace fast_ber
{

inline constexpr bool is_an_identifier_choice(Class, Tag) { return false; }

template <typename Identifier, typename... Identifiers>
constexpr bool is_an_identifier_choice(Class class_, Tag tag, Identifier id, Identifiers... ids)
{
    return id.check_id_match(class_, tag) ? true : is_an_identifier_choice(class_, tag, ids...);
}

template <typename... Identifiers>
struct ChoiceId
{
    static std::tuple<Identifiers...> choice_ids() { return {}; };
    constexpr static bool check_id_match(Class c, Tag t) { return is_an_identifier_choice(c, t, Identifiers{}...); }
};

template <typename T>
struct IsChoiceId : std::false_type
{
};

template <typename... Identifiers>
struct IsChoiceId<ChoiceId<Identifiers...>> : std::true_type
{
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

template <typename Identifier, typename T0, typename... Args>
struct TaggedChoice : public fast_ber::DynamicVariant<T0, Args...>
{
    using Base = fast_ber::DynamicVariant<T0, Args...>;
    using Base::Base;
    using Base::operator=;

    Base&       base() { return *this; }
    const Base& base() const { return *this; }

    TaggedChoice() : Base() {}
    TaggedChoice(const TaggedChoice& rhs) = default;
    TaggedChoice(TaggedChoice&& rhs)      = default;

    TaggedChoice& operator=(const TaggedChoice& rhs) = default;

    bool operator==(const TaggedChoice& rhs) const { return this->base() == rhs.base(); }
    bool operator!=(const TaggedChoice& rhs) const { return this->base() != rhs.base(); }

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

    using AsnId = Identifier;
};

template <typename T0, typename... Args>
using Choice = TaggedChoice<ChoiceId<Identifier<T0>, Identifier<Args>...>, T0, Args...>;

template <typename Identifier, typename T0, class... Types>
struct variant_size<TaggedChoice<Identifier, T0, Types...>>
{
    constexpr static std::size_t value = 1 + sizeof...(Types);
};

template <std::size_t I, typename Identifier, typename T0, class... Types>
struct variant_alternative<I, TaggedChoice<Identifier, T0, Types...>>
{
    using type = typename fast_ber::variant_alternative<I, typename TaggedChoice<Identifier, T0, Types...>::Base>::type;
};

/*
template <typename Visitor, typename Identifier, typename... Variants>
auto visit(Visitor&& vis, const TaggedChoice<Identifier, Variants...>& variant)
    -> decltype(absl::visit(vis, variant.base()))
{
    return absl::visit(vis, variant.base());
}*/

struct LengthVisitor
{
    template <typename T>
    size_t operator()(const T& t) const
    {
        return encoded_length(t);
    }
};

template <typename Identifier, typename... Variants, absl::enable_if_t<!IsChoiceId<Identifier>::value, int> = 0>
size_t encoded_length(const TaggedChoice<Identifier, Variants...>& choice) noexcept
{
    LengthVisitor visit;
    return wrap_with_ber_header_length(fast_ber::visit(visit, choice), Identifier{});
}

template <typename Identifier, typename... Variants, absl::enable_if_t<IsChoiceId<Identifier>::value, int> = 0>
size_t encoded_length(const TaggedChoice<Identifier, Variants...>& choice) noexcept
{
    LengthVisitor visit;
    return fast_ber::visit(visit, choice);
}

template <size_t index, size_t max_depth, typename Identifier, typename... Variants,
          typename std::enable_if<(!(index < max_depth)), int>::type = 0>
EncodeResult encode_if(const absl::Span<uint8_t>&, const TaggedChoice<Identifier, Variants...>&) noexcept
{
    // No substitutions found, fail
    return EncodeResult{false, 0};
}

template <size_t index, size_t max_depth, typename Identifier, typename... Variants,
          typename std::enable_if<(index < max_depth), int>::type = 0>
EncodeResult encode_if(const absl::Span<uint8_t>& buffer, const TaggedChoice<Identifier, Variants...>& choice) noexcept
{
    if (choice.index() == index)
    {
        const auto* child = fast_ber::get_if<index>(&choice);
        assert(child);

        return encode(buffer, *child);
    }
    else
    {
        return encode_if<index + 1, max_depth>(buffer, choice);
    }
}

template <typename Identifier, typename... Variants>
EncodeResult encode_choice(const absl::Span<uint8_t>&                   buffer,
                           const TaggedChoice<Identifier, Variants...>& choice) noexcept
{
    constexpr auto depth = static_cast<int>(fast_ber::variant_size<TaggedChoice<Identifier, Variants...>>::value);
    return encode_if<0, depth>(buffer, choice);
}

template <typename Identifier, typename... Variants, absl::enable_if_t<!IsChoiceId<Identifier>::value, int> = 0>
EncodeResult encode(const absl::Span<uint8_t>& buffer, const TaggedChoice<Identifier, Variants...>& choice) noexcept
{
    const auto header_length_guess = 2;
    auto       child_buffer        = buffer;
    child_buffer.remove_prefix(header_length_guess);

    const EncodeResult& inner_encode_result = encode_choice(child_buffer, choice);
    if (!inner_encode_result.success)
    {
        return inner_encode_result;
    }

    return wrap_with_ber_header(buffer, inner_encode_result.length, Identifier{}, header_length_guess);
}

template <typename Identifier, typename... Variants, absl::enable_if_t<IsChoiceId<Identifier>::value, int> = 0>
EncodeResult encode(const absl::Span<uint8_t>& buffer, const TaggedChoice<Identifier, Variants...>& choice) noexcept
{
    return encode_choice(buffer, choice);
}

template <int index, int max_depth, typename Identifier, typename... Variants,
          typename std::enable_if<(!(index < max_depth)), int>::type = 0>
DecodeResult decode_if(BerViewIterator&, TaggedChoice<Identifier, Variants...>&) noexcept
{
    // No substitutions found, fail
    return DecodeResult{false};
}

template <size_t index, size_t max_depth, typename ID, typename... Variants,
          typename std::enable_if<(index < max_depth), int>::type = 0>
DecodeResult decode_if(BerViewIterator& input, TaggedChoice<ID, Variants...>& output) noexcept
{
    using T = typename fast_ber::variant_alternative<index, TaggedChoice<ID, Variants...>>::type;

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
DecodeResult decode(BerViewIterator&                                              input,
                    TaggedChoice<ChoiceId<Identifier<Variants>...>, Variants...>& output) noexcept
{
    constexpr auto     depth  = fast_ber::variant_size<typename std::remove_reference<decltype(output)>::type>::value;
    const DecodeResult result = decode_if<0, depth>(input, output);
    ++input;
    return result;
}

template <typename Identifier, typename... Variants, absl::enable_if_t<!IsChoiceId<Identifier>::value, int> = 0>
DecodeResult decode(BerViewIterator& input, TaggedChoice<Identifier, Variants...>& output) noexcept
{
    if (!input->is_valid() || input->tag() != Identifier::tag() || input->class_() != Identifier::class_())
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

template <typename Identifier, typename... Variants>
std::ostream& operator<<(std::ostream& os, const TaggedChoice<Identifier, Variants...>& variant);

struct OsVisitor
{
    template <typename T>
    void operator()(const T& t)
    {
        os << t;
    }

    std::ostream& os;
};

template <typename Identifier, typename... Variants>
std::ostream& operator<<(std::ostream& os, const TaggedChoice<Identifier, Variants...>& variant)
{
    OsVisitor visitor{os};

    fast_ber::visit(visitor, variant);
    return os;
}

} // namespace fast_ber
