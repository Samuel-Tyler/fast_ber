﻿#pragma once

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
    constexpr static bool check_id_match(Class c, Tag t) { return is_an_identifier_choice(c, t, Identifiers{}...); }
};

template <typename T>
struct IsChoiceId : std::false_type
{
};

template <typename... Types>
struct IsChoiceId<ChoiceId<Types...>> : std::true_type
{
};

template <typename... Identifiers>
struct OuterIdentifierT<ChoiceId<Identifiers...>>
{
    using Type = ChoiceId<Identifiers...>;
};

template <typename... Types>
struct Choices
{
    using DefaultIdentifier = ChoiceId<OuterIdentifier<Identifier<Types>>...>;
};

template <typename Choices, typename Identifier = typename Choices::DefaultIdentifier,
          StorageMode storage = StorageMode::static_>
struct Choice;

template <typename Types, StorageMode storage>
struct ChoiceImplementation
{
};

template <typename... Types>
struct ChoiceImplementation<Choices<Types...>, StorageMode::static_>
{
    using Type = absl::variant<Types...>;

    template <size_t i>
    using InPlaceIndex = absl::in_place_index_t<i>;

    template <typename T>
    using InPlaceType = absl::in_place_type_t<T>;
};

template <typename... Types>
struct ChoiceImplementation<Choices<Types...>, StorageMode::dynamic>
{
    using Type = dynamic::DynamicVariant<Types...>;

    template <size_t i>
    using InPlaceIndex = dynamic::in_place_index_t<i>;

    template <typename T>
    using InPlaceType = dynamic::in_place_type_t<T>;
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
    os << "[";
    print(os, Identifiers{}...);
    return os << "]";
}

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

template <class T>
struct variant_size;

template <typename Identifier, typename... Types, StorageMode storage>
struct variant_size<Choice<Choices<Types...>, Identifier, storage>>
    : std::integral_constant<std::size_t, sizeof...(Types)>
{
};

template <std::size_t I, typename Choice>
struct variant_alternative;

template <std::size_t I, typename... Types, typename Identifier, StorageMode storage>
struct variant_alternative<I, Choice<Choices<Types...>, Identifier, storage>>
{
    using type = dynamic::detail::TypeAtIndex<I, Types...>;
};

template <std::size_t I, class T>
using variant_alternative_t = typename variant_alternative<I, T>::type;

template <class T, typename... Types, typename Identifier, StorageMode storage>
constexpr bool holds_alternative(const Choice<Choices<Types...>, Identifier, storage>& v) noexcept
{
    return v.index() == dynamic::detail::IndexOf<T, Types...>::value;
}

template <class T, typename... Types, typename Identifier>
T& get(Choice<Choices<Types...>, Identifier, StorageMode::static_>& v)
{
    return absl::get<T>(v.base());
}

template <class T, typename... Types, typename Identifier>
T&& get(Choice<Choices<Types...>, Identifier, StorageMode::static_>&& v)
{
    return absl::get<T>(v.base());
}

template <class T, typename... Types, typename Identifier>
const T& get(const Choice<Choices<Types...>, Identifier, StorageMode::static_>& v)
{
    return absl::get<T>(v.base());
}

template <class T, typename... Types, typename Identifier>
const T&& get(const Choice<Choices<Types...>, Identifier, StorageMode::static_>&& v)
{
    return absl::get<T>(v.base());
}

template <std::size_t I, typename... Types, typename Identifier>
variant_alternative_t<I, Choice<Choices<Types...>, Identifier, StorageMode::static_>>&
get(Choice<Choices<Types...>, Identifier, StorageMode::static_>& v)
{
    return absl::get<I>(v.base());
}

template <std::size_t I, typename... Types, typename Identifier>
variant_alternative_t<I, Choice<Choices<Types...>, Identifier, StorageMode::static_>>&&
get(Choice<Choices<Types...>, Identifier, StorageMode::static_>&& v)
{
    return absl::get<I>(v.base());
}

template <std::size_t I, typename... Types, typename Identifier>
const variant_alternative_t<I, Choice<Choices<Types...>, Identifier, StorageMode::static_>>&
get(const Choice<Choices<Types...>, Identifier, StorageMode::static_>& v)
{
    return absl::get<I>(v.base());
}

template <std::size_t I, typename... Types, typename Identifier>
const variant_alternative_t<I, Choice<Choices<Types...>, Identifier, StorageMode::static_>>&&
get(const Choice<Choices<Types...>, Identifier, StorageMode::static_>&& v)
{
    return absl::get<I>(v.base());
}

template <class T, typename... Types, typename Identifier>
T&& get(Choice<Choices<Types...>, Identifier, StorageMode::dynamic>&& v)
{
    return fast_ber::dynamic::get<T>(v.base());
}

template <class T, typename... Types, typename Identifier>
const T& get(const Choice<Choices<Types...>, Identifier, StorageMode::dynamic>& v)
{
    return fast_ber::dynamic::get<T>(v.base());
}

template <class T, typename... Types, typename Identifier>
const T&& get(const Choice<Choices<Types...>, Identifier, StorageMode::dynamic>&& v)
{
    return fast_ber::dynamic::get<T>(v.base());
}

template <std::size_t I, typename... Types, typename Identifier>
variant_alternative_t<I, Choice<Choices<Types...>, Identifier, StorageMode::dynamic>>&
get(Choice<Choices<Types...>, Identifier, StorageMode::dynamic>& v)
{
    return fast_ber::dynamic::get<I>(v.base());
}

template <std::size_t I, typename... Types, typename Identifier>
variant_alternative_t<I, Choice<Choices<Types...>, Identifier, StorageMode::dynamic>>&&
get(Choice<Choices<Types...>, Identifier, StorageMode::dynamic>&& v)
{
    return fast_ber::dynamic::get<I>(v.base());
}

template <std::size_t I, typename... Types, typename Identifier>
const variant_alternative_t<I, Choice<Choices<Types...>, Identifier, StorageMode::dynamic>>&
get(const Choice<Choices<Types...>, Identifier, StorageMode::dynamic>& v)
{
    return fast_ber::dynamic::get<I>(v.base());
}

template <std::size_t I, typename... Types, typename Identifier>
const variant_alternative_t<I, Choice<Choices<Types...>, Identifier, StorageMode::dynamic>>&&
get(const Choice<Choices<Types...>, Identifier, StorageMode::dynamic>&& v)
{
    return fast_ber::dynamic::get<I>(v.base());
}

template <std::size_t I, typename... Types, typename Identifier>
constexpr typename std::add_pointer<
    variant_alternative_t<I, Choice<Choices<Types...>, Identifier, StorageMode::static_>>>::type
get_if(Choice<Choices<Types...>, Identifier, StorageMode::static_>* v) noexcept
{
    return v ? absl::get_if<I>(&(v->base())) : nullptr;
}

template <std::size_t I, typename... Types, typename Identifier>
constexpr typename std::add_pointer<
    const variant_alternative_t<I, Choice<Choices<Types...>, Identifier, StorageMode::static_>>>::type
get_if(const Choice<Choices<Types...>, Identifier, StorageMode::static_>* v) noexcept
{
    return v ? absl::get_if<I>(&(v->base())) : nullptr;
}

template <class T, typename... Types, typename Identifier>
constexpr typename std::add_pointer<T>::type
get_if(Choice<Choices<Types...>, Identifier, StorageMode::static_>* v) noexcept
{
    return v ? absl::get_if<T>(&(v->base())) : nullptr;
}

template <class T, typename... Types, typename Identifier>
constexpr typename std::add_pointer<const T>::type
get_if(const Choice<Choices<Types...>, Identifier, StorageMode::static_>* v) noexcept
{
    return v ? absl::get_if<T>(&(v->base())) : nullptr;
}

template <std::size_t I, typename... Types, typename Identifier>
constexpr typename std::add_pointer<
    variant_alternative_t<I, Choice<Choices<Types...>, Identifier, StorageMode::dynamic>>>::type
get_if(Choice<Choices<Types...>, Identifier, StorageMode::dynamic>* v) noexcept
{
    return v ? fast_ber::dynamic::get_if<I>(&(v->base())) : nullptr;
}

template <std::size_t I, typename... Types, typename Identifier>
constexpr typename std::add_pointer<
    const variant_alternative_t<I, Choice<Choices<Types...>, Identifier, StorageMode::dynamic>>>::type
get_if(const Choice<Choices<Types...>, Identifier, StorageMode::dynamic>* v) noexcept
{
    return v ? fast_ber::dynamic::get_if<I>(&(v->base())) : nullptr;
}

template <class T, typename... Types, typename Identifier>
constexpr typename std::add_pointer<T>::type
get_if(Choice<Choices<Types...>, Identifier, StorageMode::dynamic>* v) noexcept
{
    return v ? fast_ber::dynamic::get_if<T>(&(v->base())) : nullptr;
}

template <class T, typename... Types, typename Identifier>
constexpr typename std::add_pointer<const T>::type
get_if(const Choice<Choices<Types...>, Identifier, StorageMode::dynamic>* v) noexcept
{
    return v ? fast_ber::dynamic::get_if<T>(&(v->base())) : nullptr;
}

template <typename... Types, typename Identifier, StorageMode storage>
struct Choice<Choices<Types...>, Identifier, storage>
{
    using Implementation = ChoiceImplementation<Choices<Types...>, storage>;
    typename Implementation::Type&       base() { return m_base; }
    const typename Implementation::Type& base() const { return m_base; }

    template <size_t i>
    using ToType = typename dynamic::DynamicVariant<Types...>::template ToType<i>;
    template <typename T>
    using ExactlyOnce = typename dynamic::DynamicVariant<Types...>::template ExactlyOnce<T>;
    template <typename T>
    using AcceptedIndex = typename dynamic::DynamicVariant<Types...>::template AcceptedIndex<T>;
    template <typename T>
    using AcceptedType = typename dynamic::DynamicVariant<Types...>::template AcceptedType<T>;

  public:
    Choice()                    = default;
    ~Choice()                   = default;
    Choice(const Choice& other) = default;
    Choice(Choice&& other) noexcept : m_base(std::move(other.m_base)) {}
    Choice& operator=(const Choice& other) = default;
    Choice& operator                       =(Choice&& other) noexcept
    {
        m_base = std::move(other.m_base);
        return *this;
    };

    template <typename T, typename = absl::enable_if_t<!std::is_same<absl::decay_t<T>, Choice>::value &&
                                                       ExactlyOnce<AcceptedType<T&&>>::value &&
                                                       std::is_constructible<AcceptedType<T&&>, T&&>::value>>
    Choice(T&& t) noexcept(std::is_nothrow_constructible<AcceptedType<T&&>, T&&>::value)
        : m_base(typename Implementation::template InPlaceType<AcceptedType<T&&>>{}, t)
    {
        assert(holds_alternative<AcceptedType<T&&>>(*this));
    }

    template <typename T, typename... Args,
              typename = absl::enable_if_t<ExactlyOnce<T>::value && std::is_constructible<T, Args&&...>::value>>
    explicit Choice(in_place_type_t<T>, Args&&... args)
        : m_base(typename Implementation::template InPlaceType<T>{}, std::forward<Args>(args)...)
    {
        assert(holds_alternative<T>(*this));
    }

    template <typename T, typename U, typename... Args,
              typename = absl::enable_if_t<ExactlyOnce<T>::value &&
                                           std::is_constructible<T, std::initializer_list<U>&, Args&&...>::value>>
    explicit Choice(in_place_type_t<T>, std::initializer_list<U> il, Args&&... args)
        : m_base(typename Implementation::template InPlaceType<T>{}, il, std::forward<Args>(args)...)
    {
        assert(holds_alternative<T>(*this));
    }

    template <size_t i, typename... Args,
              typename = absl::enable_if_t<std::is_constructible<ToType<i>, Args&&...>::value>>
    explicit Choice(in_place_index_t<i>, Args&&... args)
        : m_base(typename Implementation::template InPlaceIndex<i>{}, std::forward<Args>(args)...)
    {
        assert(index() == i);
    }

    template <
        size_t i, typename U, typename... Args,
        typename = absl::enable_if_t<std::is_constructible<ToType<i>, std::initializer_list<U>&, Args&&...>::value>>
    explicit Choice(in_place_index_t<i>, std::initializer_list<U> il, Args&&... args)
        : m_base(typename Implementation::template InPlaceIndex<i>{}, il, std::forward<Args>(args)...)

    {
        assert(index() == i);
    }

    template <typename T, typename = absl::enable_if_t<!std::is_same<absl::decay_t<T>, Choice>::value>>
    absl::enable_if_t<ExactlyOnce<AcceptedType<T&&>>::value && std::is_constructible<AcceptedType<T&&>, T&&>::value &&
                          std::is_assignable<AcceptedType<T&&>&, T&&>::value,
                      Choice&>
    operator=(T&& t) noexcept(std::is_nothrow_assignable<AcceptedType<T&&>&, T&&>::value&&
                                  std::is_nothrow_constructible<AcceptedType<T&&>, T&&>::value)
    {
        m_base.template emplace<AcceptedType<T&&>>(std::forward<T&&>(t));
        return *this;
    }

    template <typename T, typename... Args,
              typename = absl::enable_if_t<std::is_constructible<T, Args...>::value && ExactlyOnce<T>::value>>
    T& emplace(Args&&... args)
    {
        return m_base.template emplace<T, Args...>(std::forward<Args>(args)...);
    }

    template <typename T, typename U, typename... Args,
              typename = absl::enable_if_t<std::is_constructible<T, std::initializer_list<U>&, Args...>::value &&
                                           ExactlyOnce<T>::value>>
    T& emplace(std::initializer_list<U> il, Args&&... args)
    {
        return m_base.template emplace<T, U, Args...>(il, std::forward<Args>(args)...);
    }

    template <size_t i, typename... Args>
    absl::enable_if_t<std::is_constructible<variant_alternative_t<i, Choice>, Args...>::value,
                      variant_alternative_t<i, Choice>&>
    emplace(Args&&... args)
    {
        return m_base.template emplace<i>(std::forward<Args>(args)...);
    }

    template <size_t i, typename U, typename... Args>
    absl::enable_if_t<
        std::is_constructible<variant_alternative_t<i, Choice>, std::initializer_list<U>&, Args...>::value,
        variant_alternative_t<i, Choice>&>
    emplace(std::initializer_list<U> il, Args&&... args)
    {
        return m_base.template emplace<i>(il, std::forward<Args>(args)...);
    }

    constexpr bool valueless_by_exception() const noexcept { return m_base.valueless_by_exception(); }

    constexpr std::size_t index() const noexcept { return m_base.index(); }

    void swap(Choice& rhs) noexcept { return m_base.swap(rhs.base()); }

    size_t       encoded_length() const noexcept;
    EncodeResult encode(absl::Span<uint8_t> buffer) const noexcept;
    DecodeResult decode(BerView input) noexcept;

    using AsnId = Identifier;

  private:
    typename Implementation::Type m_base;
};

template <typename... Variants, typename Identifier, StorageMode storage>
bool operator==(const Choice<Choices<Variants...>, Identifier, storage>& a,
                const Choice<Choices<Variants...>, Identifier, storage>& b)
{
    return a.base() == b.base();
}

template <typename... Variants, typename Identifier, StorageMode storage>
bool operator!=(const Choice<Choices<Variants...>, Identifier, storage>& a,
                const Choice<Choices<Variants...>, Identifier, storage>& b)
{
    return a.base() != b.base();
}

template <typename... Variants, typename Identifier, StorageMode storage>
bool operator<(const Choice<Choices<Variants...>, Identifier, storage>& a,
               const Choice<Choices<Variants...>, Identifier, storage>& b)
{
    return a.base() < b.base();
}

template <typename... Variants, typename Identifier, StorageMode storage>
bool operator>(const Choice<Choices<Variants...>, Identifier, storage>& a,
               const Choice<Choices<Variants...>, Identifier, storage>& b)
{
    return a.base() > b.base();
}

template <typename... Variants, typename Identifier, StorageMode storage>
bool operator<=(const Choice<Choices<Variants...>, Identifier, storage>& a,
                const Choice<Choices<Variants...>, Identifier, storage>& b)
{
    return a.base() <= b.base();
}

template <typename... Variants, typename Identifier, StorageMode storage>
bool operator>=(const Choice<Choices<Variants...>, Identifier, storage>& a,
                const Choice<Choices<Variants...>, Identifier, storage>& b)
{
    return a.base() >= b.base();
}

template <typename Visitor, typename... Variants, typename Identifier>
auto visit(Visitor&& vis, const Choice<Choices<Variants...>, Identifier, StorageMode::dynamic>& variant)
    -> decltype(fast_ber::dynamic::visit(vis, variant.base()))
{
    return fast_ber::dynamic::visit(vis, variant.base());
}

template <typename Visitor, typename... Variants, typename Identifier>
auto visit(Visitor&& vis, const Choice<Choices<Variants...>, Identifier, StorageMode::static_>& variant)
    -> decltype(absl::visit(vis, variant.base()))
{
    return absl::visit(vis, variant.base());
}

struct LengthVisitor
{
    template <typename T>
    size_t operator()(const T& t) const
    {
        return encoded_length(t);
    }
};

template <typename... Variants, typename Identifier, StorageMode storage,
          absl::enable_if_t<!IsChoiceId<Identifier>::value, int> = 0>
size_t encoded_length_impl(const Choice<Choices<Variants...>, Identifier, storage>& choice) noexcept
{
    LengthVisitor visit;
    return encoded_length(fast_ber::visit(visit, choice), Identifier{});
}

template <typename... Variants, typename Identifier, StorageMode storage,
          absl::enable_if_t<IsChoiceId<Identifier>::value, int> = 0>
size_t encoded_length_impl(const Choice<Choices<Variants...>, Identifier, storage>& choice) noexcept
{
    LengthVisitor visit;
    return fast_ber::visit(visit, choice);
}

template <typename... Variants, typename Identifier, StorageMode storage>
size_t Choice<Choices<Variants...>, Identifier, storage>::encoded_length() const noexcept
{
    return encoded_length_impl(*this);
}

template <size_t index, size_t max_depth, typename... Variants, typename Identifier, StorageMode storage,
          typename std::enable_if<(!(index < max_depth)), int>::type = 0>
EncodeResult encode_if(absl::Span<uint8_t>, const Choice<Choices<Variants...>, Identifier, storage>&) noexcept
{
    // No substitutions found, fail
    return EncodeResult{false, 0};
}

template <size_t index, size_t max_depth, typename... Variants, typename Identifier, StorageMode storage,
          typename std::enable_if<(index < max_depth), int>::type = 0>
EncodeResult encode_if(absl::Span<uint8_t>                                      buffer,
                       const Choice<Choices<Variants...>, Identifier, storage>& choice) noexcept
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

template <typename... Variants, typename Identifier, StorageMode storage>
EncodeResult encode_choice(absl::Span<uint8_t>                                      buffer,
                           const Choice<Choices<Variants...>, Identifier, storage>& choice) noexcept
{
    constexpr size_t depth = sizeof...(Variants);
    return encode_if<0, depth>(buffer, choice);
}

template <typename... Variants, typename Identifier, StorageMode storage,
          absl::enable_if_t<!IsChoiceId<Identifier>::value, int> = 0>
EncodeResult encode_impl(absl::Span<uint8_t>                                      buffer,
                         const Choice<Choices<Variants...>, Identifier, storage>& choice) noexcept
{
    constexpr size_t header_length_guess = encoded_length(0, Identifier{});
    auto             child_buffer        = buffer;
    if (buffer.length() < header_length_guess)
    {
        return EncodeResult{false, 0};
    }
    child_buffer.remove_prefix(header_length_guess);

    EncodeResult inner_encode_result = encode_choice(child_buffer, choice);
    if (!inner_encode_result.success)
    {
        return inner_encode_result;
    }

    return wrap_with_ber_header(buffer, inner_encode_result.length, Identifier{}, header_length_guess);
}

template <typename... Variants, typename Identifier, StorageMode storage,
          absl::enable_if_t<IsChoiceId<Identifier>::value, int> = 0>
EncodeResult encode_impl(absl::Span<uint8_t>                                      buffer,
                         const Choice<Choices<Variants...>, Identifier, storage>& choice) noexcept
{
    return encode_choice(buffer, choice);
}

template <typename... Variants, typename Identifier, StorageMode storage>
EncodeResult Choice<Choices<Variants...>, Identifier, storage>::encode(absl::Span<uint8_t> buffer) const noexcept
{
    return encode_impl(buffer, *this);
}

template <int index, int max_depth, typename... Variants, typename Identifier, StorageMode storage,
          absl::enable_if_t<(!(index < max_depth)), int> = 0>
DecodeResult decode_if(BerView, Choice<Choices<Variants...>, Identifier, storage>&) noexcept
{
    // No substitutions found, fail
    return DecodeResult{false};
}

template <size_t index, size_t max_depth, typename ID, typename... Variants, StorageMode storage,
          absl::enable_if_t<(index < max_depth), int> = 0>
DecodeResult decode_if(BerView input, Choice<Choices<Variants...>, ID, storage>& output) noexcept
{
    using T = typename fast_ber::variant_alternative<index, Choice<Choices<Variants...>, ID>>::type;

    if (Identifier<T>::check_id_match(input.class_(), input.tag()))
    {
        T& child = output.template emplace<index>();
        return child.decode(input);
    }
    else
    {
        return decode_if<index + 1, max_depth>(input, output);
    }
}

template <typename... Variants, typename Identifier, StorageMode storage,
          absl::enable_if_t<IsChoiceId<Identifier>::value, int> = 0>
DecodeResult decode_impl(BerView input, Choice<Choices<Variants...>, Identifier, storage>& output) noexcept
{
    if (!input.is_valid())
    {
        return DecodeResult{false};
    }

    constexpr auto depth = sizeof...(Variants);
    return decode_if<0, depth>(input, output);
}

template <typename... Variants, typename Identifier, StorageMode storage,
          absl::enable_if_t<!IsChoiceId<Identifier>::value, int> = 0>
DecodeResult decode_impl(BerView input, Choice<Choices<Variants...>, Identifier, storage>& output) noexcept
{
    if (!has_correct_header(input, Identifier{}, Construction::constructed))
    {
        return DecodeResult{false};
    }

    BerViewIterator child = (Identifier::depth() == 1) ? input.begin() : input.begin()->begin();
    if (!child->is_valid())
    {
        return DecodeResult{false};
    }

    constexpr auto depth = sizeof...(Variants);
    return decode_if<0, depth>(*child, output);
}

template <typename... Variants, typename Identifier, StorageMode storage>
DecodeResult Choice<Choices<Variants...>, Identifier, storage>::decode(BerView input) noexcept
{
    return decode_impl(input, *this);
}

template <typename... Variants, typename Identifier, StorageMode storage>
std::ostream& operator<<(std::ostream& os, const Choice<Choices<Variants...>, Identifier, storage>& variant);

struct OsVisitor
{
    template <typename T>
    void operator()(const T& t)
    {
        os << t;
    }

    std::ostream& os;
};

template <typename... Variants, typename Identifier, StorageMode storage>
std::ostream& operator<<(std::ostream& os, const Choice<Choices<Variants...>, Identifier, storage>& variant)
{
    OsVisitor visitor{os};

    fast_ber::visit(visitor, variant);
    return os;
}

} // namespace fast_ber
