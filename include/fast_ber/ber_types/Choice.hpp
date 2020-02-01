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

template <typename... Types>
struct Choices;

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
    os << "[";
    print(os, Identifiers{}...);
    return os << "]";
}

struct BadVariantAccess : dynamic::BadVariantAccess
{
    using dynamic::BadVariantAccess::BadVariantAccess;
};

struct in_place_t
{
    explicit in_place_t() = default;
};

static constexpr in_place_t in_place{};

template <typename T>
struct in_place_type_t
{
    explicit in_place_type_t() = default;
};

template <size_t _Idx>
struct in_place_index_t
{
    explicit in_place_index_t() = default;
};

template <typename Choices, typename Identifier>
struct TaggedChoice;

template <class T>
struct variant_size;

template <typename Identifier, typename... Types>
struct variant_size<TaggedChoice<Choices<Types...>, Identifier>> : std::integral_constant<std::size_t, sizeof...(Types)>
{
};

template <std::size_t I, typename... Types>
struct variant_alternative;

template <std::size_t I, typename... Types, typename Identifier>
struct variant_alternative<I, TaggedChoice<Choices<Types...>, Identifier>>
{
    using type = dynamic::detail::TypeAtIndex<I, Types...>;
};

template <std::size_t I, class T>
using variant_alternative_t = typename variant_alternative<I, T>::type;

template <class T, typename... Types, typename Identifier>
constexpr bool holds_alternative(const TaggedChoice<Choices<Types...>, Identifier>& v) noexcept
{
    return v.index() == dynamic::detail::IndexOf<T, Types...>::value;
}

template <class T, typename... Types, typename Identifier>
T& get(TaggedChoice<Choices<Types...>, Identifier>& v)
{
    if (v.index() != dynamic::detail::IndexOf<T, Types...>::value)
    {
        throw BadVariantAccess();
    }
    return v.template value<T>();
}

template <class T, typename... Types, typename Identifier>
T&& get(TaggedChoice<Choices<Types...>, Identifier>&& v)
{
    if (v.index() != dynamic::detail::IndexOf<T, Types...>::value)
    {
        throw BadVariantAccess();
    }
    return v.template value<T>();
}

template <class T, typename... Types, typename Identifier>
const T& get(const TaggedChoice<Choices<Types...>, Identifier>& v)
{
    if (v.index() != dynamic::detail::IndexOf<T, Types...>::value)
    {
        throw BadVariantAccess();
    }
    return v.template value<T>();
}

template <class T, typename... Types, typename Identifier>
const T&& get(const TaggedChoice<Choices<Types...>, Identifier>&& v)
{
    if (v.index() != dynamic::detail::IndexOf<T, Types...>::value)
    {
        throw BadVariantAccess();
    }
    return v.template value<T>();
}

template <std::size_t I, typename... Types, typename Identifier>
variant_alternative_t<I, TaggedChoice<Choices<Types...>, Identifier>>&
get(TaggedChoice<Choices<Types...>, Identifier>& v)
{
    if (v.index() != I)
    {
        throw BadVariantAccess();
    }
    return v.template value<variant_alternative_t<I, TaggedChoice<Choices<Types...>, Identifier>>>();
}

template <std::size_t I, typename... Types, typename Identifier>
variant_alternative_t<I, TaggedChoice<Choices<Types...>, Identifier>>&&
get(TaggedChoice<Choices<Types...>, Identifier>&& v)
{
    if (v.index() != I)
    {
        throw BadVariantAccess();
    }
    return v.template value<variant_alternative_t<I, TaggedChoice<Choices<Types...>, Identifier>>>();
}

template <std::size_t I, typename... Types, typename Identifier>
const variant_alternative_t<I, TaggedChoice<Choices<Types...>, Identifier>>&
get(const TaggedChoice<Choices<Types...>, Identifier>& v)
{
    if (v.index() != I)
    {
        throw BadVariantAccess();
    }
    return v.template value<variant_alternative_t<I, TaggedChoice<Choices<Types...>, Identifier>>>();
}

template <std::size_t I, typename... Types, typename Identifier>
const variant_alternative_t<I, TaggedChoice<Choices<Types...>, Identifier>>&&
get(const TaggedChoice<Choices<Types...>, Identifier>&& v)
{
    if (v.index() != I)
    {
        throw BadVariantAccess();
    }
    return v.template value<variant_alternative_t<I, TaggedChoice<Choices<Types...>, Identifier>>>();
}

template <std::size_t I, typename... Types, typename Identifier>
constexpr typename std::add_pointer<variant_alternative_t<I, TaggedChoice<Choices<Types...>, Identifier>>>::type
get_if(TaggedChoice<Choices<Types...>, Identifier>* v) noexcept
{
    return (v != nullptr && v->index() == I) ? std::addressof(fast_ber::get<I>(*v)) : nullptr;
}

template <std::size_t I, typename... Types, typename Identifier>
constexpr typename std::add_pointer<const variant_alternative_t<I, TaggedChoice<Choices<Types...>, Identifier>>>::type
get_if(const TaggedChoice<Choices<Types...>, Identifier>* v) noexcept
{
    return (v != nullptr && v->index() == I) ? std::addressof(fast_ber::get<I>(*v)) : nullptr;
}

template <class T, typename... Types, typename Identifier>
constexpr typename std::add_pointer<T>::type get_if(TaggedChoice<Choices<Types...>, Identifier>* v) noexcept
{
    return fast_ber::get_if<dynamic::detail::IndexOf<T, Types...>::value>(v);
}

template <class T, typename... Types, typename Identifier>
constexpr typename std::add_pointer<const T>::type get_if(const TaggedChoice<Choices<Types...>, Identifier>* v) noexcept
{
    return fast_ber::get_if<dynamic::detail::IndexOf<T, Types...>::value>(v);
}

template <typename... Types, typename Identifier>
struct TaggedChoice<Choices<Types...>, Identifier>
{
    using Base = fast_ber::dynamic::DynamicVariant<Types...>;
    Base&       base() { return m_base; }
    const Base& base() const { return m_base; }

    template <size_t i>
    using ToType = typename dynamic::DynamicVariant<Types...>::template ToType<i>;
    template <typename T>
    using ExactlyOnce = typename dynamic::DynamicVariant<Types...>::template ExactlyOnce<T>;
    template <typename T>
    using AcceptedIndex = typename dynamic::DynamicVariant<Types...>::template AcceptedIndex<T>;
    template <typename T>
    using AcceptedType = typename dynamic::DynamicVariant<Types...>::template AcceptedType<T>;

  public:
    TaggedChoice()                          = default;
    ~TaggedChoice()                         = default;
    TaggedChoice(const TaggedChoice& other) = default;
    TaggedChoice(TaggedChoice&& other)      = default;
    TaggedChoice& operator=(const TaggedChoice& other) = default;
    TaggedChoice& operator=(TaggedChoice&& other) = default;

    template <typename T, typename = absl::enable_if_t<!std::is_same<absl::decay_t<T>, TaggedChoice>::value &&
                                                       ExactlyOnce<AcceptedType<T&&>>::value &&
                                                       std::is_constructible<AcceptedType<T&&>, T&&>::value>>
    TaggedChoice(T&& t) noexcept(std::is_nothrow_constructible<AcceptedType<T&&>, T&&>::value) : m_base(t)
    {
        assert(holds_alternative<AcceptedType<T&&>>(*this));
    }

    template <typename T, typename... Args,
              typename = absl::enable_if_t<ExactlyOnce<T>::value && std::is_constructible<T, Args&&...>::value>>
    explicit TaggedChoice(in_place_type_t<T>, Args&&... args)
        : m_base(dynamic::in_place_t{}, std::forward<Args>(args)...)
    {
        assert(holds_alternative<T>(*this));
    }

    template <typename T, typename U, typename... Args,
              typename = absl::enable_if_t<ExactlyOnce<T>::value &&
                                           std::is_constructible<T, std::initializer_list<U>&, Args&&...>::value>>
    explicit TaggedChoice(in_place_type_t<T>, std::initializer_list<U> il, Args&&... args)
        : m_base(dynamic::in_place_t{}, il, std::forward<Args>(args)...)
    {
        assert(holds_alternative<T>(*this));
    }

    template <size_t i, typename... Args,
              typename = absl::enable_if_t<std::is_constructible<ToType<i>, Args&&...>::value>>
    explicit TaggedChoice(in_place_index_t<i>, Args&&... args)
        : m_base(dynamic::in_place_index_t<i>{}, std::forward<Args>(args)...)
    {
        assert(index() == i);
    }

    template <
        size_t i, typename U, typename... Args,
        typename = absl::enable_if_t<std::is_constructible<ToType<i>, std::initializer_list<U>&, Args&&...>::value>>
    explicit TaggedChoice(in_place_index_t<i>, std::initializer_list<U> il, Args&&... args)
        : m_base(dynamic::in_place_index_t<i>{}, il, std::forward<Args>(args)...)

    {
        assert(index() == i);
    }

    template <typename T, typename = absl::enable_if_t<!std::is_same<absl::decay_t<T>, TaggedChoice>::value>>
    absl::enable_if_t<ExactlyOnce<AcceptedType<T&&>>::value && std::is_constructible<AcceptedType<T&&>, T&&>::value &&
                          std::is_assignable<AcceptedType<T&&>&, T&&>::value,
                      TaggedChoice&>
    operator=(T&& t) noexcept(std::is_nothrow_assignable<AcceptedType<T&&>&, T&&>::value&&
                                  std::is_nothrow_constructible<AcceptedType<T&&>, T&&>::value)
    {
        m_base = t;
        return *this;
    }

    template <typename T, typename... Args,
              typename = absl::enable_if_t<std::is_constructible<T, Args...>::value && ExactlyOnce<T>::value>>
    T& emplace(Args&&... args)
    {
        return m_base.emplace(std::forward<Args>(args)...);
    }

    template <typename T, typename U, typename... Args,
              typename = absl::enable_if_t<std::is_constructible<T, std::initializer_list<U>&, Args...>::value &&
                                           ExactlyOnce<T>::value>>
    T& emplace(std::initializer_list<U> il, Args&&... args)
    {
        return m_base.emplace(il, std::forward<Args>(args)...);
    }

    template <size_t i, typename... Args>
    absl::enable_if_t<std::is_constructible<variant_alternative_t<i, TaggedChoice>, Args...>::value,
                      variant_alternative_t<i, TaggedChoice>&>
    emplace(Args&&... args)
    {
        return m_base.template emplace<i>(std::forward<Args>(args)...);
    }

    template <size_t i, typename U, typename... Args>
    absl::enable_if_t<
        std::is_constructible<variant_alternative_t<i, TaggedChoice>, std::initializer_list<U>&, Args...>::value,
        variant_alternative_t<i, TaggedChoice>&>
    emplace(std::initializer_list<U> il, Args&&... args)
    {
        return m_base.template emplace<i>(il, std::forward<Args>(args)...);
    }

    constexpr bool valueless_by_exception() const noexcept { return m_base.valueless_by_exception(); }

    constexpr std::size_t index() const noexcept { return m_base.index(); }

    void swap(TaggedChoice& rhs) noexcept { return m_base.swap(rhs.base()); }

    template <typename T>
    T& value()
    {
        return m_base.template value<T>();
    }

    template <typename T>
    const T& value() const
    {
        return m_base.template value<T>();
    }

    using AsnId = Identifier;

  private:
    fast_ber::dynamic::DynamicVariant<Types...> m_base;
};

template <typename... Variants, typename Identifier>
bool operator==(const TaggedChoice<Choices<Variants...>, Identifier>& a,
                const TaggedChoice<Choices<Variants...>, Identifier>& b)
{
    return a.base() == b.base();
}

template <typename... Variants, typename Identifier>
bool operator!=(const TaggedChoice<Choices<Variants...>, Identifier>& a,
                const TaggedChoice<Choices<Variants...>, Identifier>& b)
{
    return a.base() != b.base();
}

template <typename... Variants, typename Identifier>
bool operator<(const TaggedChoice<Choices<Variants...>, Identifier>& a,
               const TaggedChoice<Choices<Variants...>, Identifier>& b)
{
    return a.base() < b.base();
}

template <typename... Variants, typename Identifier>
bool operator>(const TaggedChoice<Choices<Variants...>, Identifier>& a,
               const TaggedChoice<Choices<Variants...>, Identifier>& b)
{
    return a.base() > b.base();
}

template <typename... Variants, typename Identifier>
bool operator<=(const TaggedChoice<Choices<Variants...>, Identifier>& a,
                const TaggedChoice<Choices<Variants...>, Identifier>& b)
{
    return a.base() <= b.base();
}

template <typename... Variants, typename Identifier>
bool operator>=(const TaggedChoice<Choices<Variants...>, Identifier>& a,
                const TaggedChoice<Choices<Variants...>, Identifier>& b)
{
    return a.base() >= b.base();
}

template <typename... Types>
using Choice = TaggedChoice<Choices<Types...>, ChoiceId<Identifier<Types>...>>;

template <typename Visitor, typename... Variants, typename Identifier>
auto visit(Visitor&& vis, const TaggedChoice<Choices<Variants...>, Identifier>& variant)
    -> decltype(fast_ber::dynamic::visit(vis, variant.base()))
{
    return fast_ber::dynamic::visit(vis, variant.base());
}

/*
template <typename Visitor, typename... Variants, typename Identifier>
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

template <typename... Variants, typename Identifier,
          absl::enable_if_t<!std::is_same<Identifier, ChoiceId<fast_ber::Identifier<Variants>...>>::value, int> = 0>
size_t encoded_length(const TaggedChoice<Choices<Variants...>, Identifier>& choice) noexcept
{
    LengthVisitor visit;
    return wrap_with_ber_header_length(fast_ber::visit(visit, choice), Identifier{});
}

template <typename... Variants, typename Identifier,
          absl::enable_if_t<std::is_same<Identifier, ChoiceId<fast_ber::Identifier<Variants>...>>::value, int> = 0>
size_t encoded_length(const TaggedChoice<Choices<Variants...>, Identifier>& choice) noexcept
{
    LengthVisitor visit;
    return fast_ber::visit(visit, choice);
}

template <size_t index, size_t max_depth, typename... Variants, typename Identifier,
          typename std::enable_if<(!(index < max_depth)), int>::type = 0>
EncodeResult encode_if(const absl::Span<uint8_t>&, const TaggedChoice<Choices<Variants...>, Identifier>&) noexcept
{
    // No substitutions found, fail
    return EncodeResult{false, 0};
}

template <size_t index, size_t max_depth, typename... Variants, typename Identifier,
          typename std::enable_if<(index < max_depth), int>::type = 0>
EncodeResult encode_if(const absl::Span<uint8_t>&                            buffer,
                       const TaggedChoice<Choices<Variants...>, Identifier>& choice) noexcept
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

template <typename... Variants, typename Identifier>
EncodeResult encode_choice(const absl::Span<uint8_t>&                            buffer,
                           const TaggedChoice<Choices<Variants...>, Identifier>& choice) noexcept
{
    constexpr size_t depth = sizeof...(Variants);
    return encode_if<0, depth>(buffer, choice);
}

template <typename... Variants, typename Identifier,
          absl::enable_if_t<!std::is_same<Identifier, ChoiceId<fast_ber::Identifier<Variants>...>>::value, int> = 0>
EncodeResult encode(const absl::Span<uint8_t>&                            buffer,
                    const TaggedChoice<Choices<Variants...>, Identifier>& choice) noexcept
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

template <typename... Variants, typename Identifier,
          absl::enable_if_t<std::is_same<Identifier, ChoiceId<fast_ber::Identifier<Variants>...>>::value, int> = 0>
EncodeResult encode(const absl::Span<uint8_t>&                            buffer,
                    const TaggedChoice<Choices<Variants...>, Identifier>& choice) noexcept
{
    return encode_choice(buffer, choice);
}

template <int index, int max_depth, typename... Variants, typename Identifier,
          absl::enable_if_t<(!(index < max_depth)), int> = 0>
DecodeResult decode_if(BerViewIterator&, TaggedChoice<Choices<Variants...>, Identifier>&) noexcept
{
    // No substitutions found, fail
    return DecodeResult{false};
}

template <size_t index, size_t max_depth, typename ID, typename... Variants,
          absl::enable_if_t<(index < max_depth), int> = 0>
DecodeResult decode_if(BerViewIterator& input, TaggedChoice<Choices<Variants...>, ID>& output) noexcept
{
    using T = typename fast_ber::variant_alternative<index, TaggedChoice<Choices<Variants...>, ID>>::type;

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

template <typename... Variants, typename Identifier,
          absl::enable_if_t<std::is_same<Identifier, ChoiceId<fast_ber::Identifier<Variants>...>>::value, int> = 0>
DecodeResult decode(BerViewIterator& input, TaggedChoice<Choices<Variants...>, Identifier>& output) noexcept
{
    constexpr auto     depth  = sizeof...(Variants);
    const DecodeResult result = decode_if<0, depth>(input, output);
    ++input;
    return result;
}

template <typename... Variants, typename Identifier,
          absl::enable_if_t<!std::is_same<Identifier, ChoiceId<fast_ber::Identifier<Variants>...>>::value, int> = 0>
DecodeResult decode(BerViewIterator& input, TaggedChoice<Choices<Variants...>, Identifier>& output) noexcept
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

    constexpr auto     depth  = sizeof...(Variants);
    const DecodeResult result = decode_if<0, depth>(child, output);
    ++input;
    return result;
}

template <typename... Variants, typename Identifier>
std::ostream& operator<<(std::ostream& os, const TaggedChoice<Choices<Variants...>, Identifier>& variant);

struct OsVisitor
{
    template <typename T>
    void operator()(const T& t)
    {
        os << t;
    }

    std::ostream& os;
};

template <typename... Variants, typename Identifier>
std::ostream& operator<<(std::ostream& os, const TaggedChoice<Choices<Variants...>, Identifier>& variant)
{
    OsVisitor visitor{os};

    fast_ber::visit(visitor, variant);
    return os;
}

} // namespace fast_ber
