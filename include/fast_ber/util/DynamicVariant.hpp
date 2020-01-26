#pragma once

#include "fast_ber/util/DynamicVariantHelpers.hpp"

#include "absl/meta/type_traits.h"

#include <cassert>
#include <cstddef>
#include <exception>
#include <initializer_list>
#include <type_traits>

namespace fast_ber
{

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

class BadVariantAccess : public std::exception
{
  public:
    BadVariantAccess() noexcept  = default;
    ~BadVariantAccess() override = default;
    const char* what() const noexcept override { return "Bad variant access;"; }
};

template <typename... Ts, absl::enable_if_t<absl::conjunction<std::is_move_constructible<Ts>...>::value, int> = 0>
void swap(DynamicVariant<Ts...>& v, DynamicVariant<Ts...>& w) noexcept(noexcept(v.swap(w)))
{
    v.swap(w);
}

template <class T>
struct variant_size;

template <class... Ts>
struct variant_size<DynamicVariant<Ts...>> : std::integral_constant<std::size_t, sizeof...(Ts)>
{
};

template <class T>
struct variant_size<const T> : variant_size<T>
{
};

template <class T>
struct variant_size<volatile T> : variant_size<T>
{
};

template <class T>
struct variant_size<const volatile T> : variant_size<T>
{
};

template <std::size_t I, class T>
struct variant_alternative;

template <std::size_t I, class... Types>
struct variant_alternative<I, DynamicVariant<Types...>>
{
    using type = detail::TypeAtIndex<I, Types...>;
};

template <std::size_t I, class T>
struct variant_alternative<I, const T>
{
    using type = const typename variant_alternative<I, T>::type;
};

template <std::size_t I, class T>
struct variant_alternative<I, volatile T>
{
    using type = volatile typename variant_alternative<I, T>::type;
};

template <std::size_t I, class T>
struct variant_alternative<I, const volatile T>
{
    using type = const volatile typename variant_alternative<I, T>::type;
};

template <std::size_t I, class T>
using variant_alternative_t = typename variant_alternative<I, T>::type;

template <class T, class... Types>
constexpr bool holds_alternative(const DynamicVariant<Types...>& v) noexcept
{
    return v.index() == detail::IndexOf<T, Types...>::value;
}

template <class T, class... Types>
T& get(DynamicVariant<Types...>& v)
{
    if (v.index() != detail::IndexOf<T, Types...>::value)
    {
        throw BadVariantAccess();
    }
    return v.template value<T>();
}

template <class T, class... Types>
T&& get(DynamicVariant<Types...>&& v)
{
    if (v.index() != detail::IndexOf<T, Types...>::value)
    {
        throw BadVariantAccess();
    }
    return v.template value<T>();
}

template <class T, class... Types>
const T& get(const DynamicVariant<Types...>& v)
{
    if (v.index() != detail::IndexOf<T, Types...>::value)
    {
        throw BadVariantAccess();
    }
    return v.template value<T>();
}

template <class T, class... Types>
const T&& get(const DynamicVariant<Types...>&& v)
{
    if (v.index() != detail::IndexOf<T, Types...>::value)
    {
        throw BadVariantAccess();
    }
    return v.template value<T>();
}

template <std::size_t I, class... Types>
variant_alternative_t<I, DynamicVariant<Types...>>& get(DynamicVariant<Types...>& v)
{
    if (v.index() != I)
    {
        throw BadVariantAccess();
    }
    return v.template value<variant_alternative_t<I, DynamicVariant<Types...>>>();
}

template <std::size_t I, class... Types>
variant_alternative_t<I, DynamicVariant<Types...>>&& get(DynamicVariant<Types...>&& v)
{
    if (v.index() != I)
    {
        throw BadVariantAccess();
    }
    return v.template value<variant_alternative_t<I, DynamicVariant<Types...>>>();
}

template <std::size_t I, class... Types>
const variant_alternative_t<I, DynamicVariant<Types...>>& get(const DynamicVariant<Types...>& v)
{
    if (v.index() != I)
    {
        throw BadVariantAccess();
    }
    return v.template value<variant_alternative_t<I, DynamicVariant<Types...>>>();
}

template <std::size_t I, class... Types>
const variant_alternative_t<I, DynamicVariant<Types...>>&& get(const DynamicVariant<Types...>&& v)
{
    if (v.index() != I)
    {
        throw BadVariantAccess();
    }
    return v.template value<variant_alternative_t<I, DynamicVariant<Types...>>>();
}

template <std::size_t I, class... Types>
constexpr typename std::add_pointer<variant_alternative_t<I, DynamicVariant<Types...>>>::type
get_if(DynamicVariant<Types...>* v) noexcept
{
    return (v != nullptr && v->index() == I) ? std::addressof(fast_ber::get<I>(*v)) : nullptr;
}

template <std::size_t I, class... Types>
constexpr typename std::add_pointer<const variant_alternative_t<I, DynamicVariant<Types...>>>::type
get_if(const DynamicVariant<Types...>* v) noexcept
{
    return (v != nullptr && v->index() == I) ? std::addressof(fast_ber::get<I>(*v)) : nullptr;
}

template <class T, class... Types>
constexpr typename std::add_pointer<T>::type get_if(DynamicVariant<Types...>* v) noexcept
{
    return fast_ber::get_if<detail::IndexOf<T, Types...>::value>(v);
}

template <class T, class... Types>
constexpr typename std::add_pointer<const T>::type get_if(const DynamicVariant<Types...>* v) noexcept
{
    return fast_ber::get_if<detail::IndexOf<T, Types...>::value>(v);
}

template <size_t index, typename Visitor, typename... VariantOptions,
          typename std::enable_if<(index == sizeof...(VariantOptions)), int>::type = 0>
[[noreturn]] auto visit_impl(Visitor&& vis, const DynamicVariant<VariantOptions...>& variant)
    -> decltype(vis(get<0>(variant)))
{
    assert("Not reachable");
    throw 0;
}

template <size_t index, typename Visitor, typename... VariantOptions,
          typename std::enable_if<(index < sizeof...(VariantOptions)), int>::type = 0>
auto visit_impl(Visitor&& vis, DynamicVariant<VariantOptions...>& variant) -> decltype(vis(get<0>(variant)))
{
    if (index == variant.index())
    {
        return vis(get<index>(variant));
    }
    return visit_impl<index + 1>(vis, variant);
}

template <size_t index, typename Visitor, typename... VariantOptions,
          typename std::enable_if<(index < sizeof...(VariantOptions)), int>::type = 0>
auto visit_impl(Visitor&& vis, const DynamicVariant<VariantOptions...>& variant) -> decltype(vis(get<0>(variant)))
{
    if (index == variant.index())
    {
        return vis(get<index>(variant));
    }
    return visit_impl<index + 1>(vis, variant);
}

template <typename Visitor, typename... Variants>
auto visit(Visitor&& vis, Variants&&... vars) -> decltype(visit_impl<0>(vis, vars...))
{
    return visit_impl<0>(vis, vars...);
}

template <size_t index, typename Visitor, typename... VariantOptions,
          typename std::enable_if<(index == sizeof...(VariantOptions)), int>::type = 0>
[[noreturn]] auto visit_binary_impl(Visitor&& vis, const DynamicVariant<VariantOptions...>& a,
                                    const DynamicVariant<VariantOptions...>& b) -> decltype(vis(get<0>(a), get<0>(b)))
{
    assert("Not reachable");
    throw 0;
}

template <size_t index, typename Visitor, typename... VariantOptions,
          typename std::enable_if<(index < sizeof...(VariantOptions)), int>::type = 0>
auto visit_binary_impl(Visitor&& vis, const DynamicVariant<VariantOptions...>& a,
                       const DynamicVariant<VariantOptions...>& b) -> decltype(vis(get<0>(a), get<0>(b)))
{
    if (index == a.index())
    {
        return vis(get<index>(a), get<index>(b));
    }
    return visit_binary_impl<index + 1>(vis, a, b);
}

template <typename Visitor, typename... Types>
auto visit_binary(Visitor&& vis, const DynamicVariant<Types...>& a, const DynamicVariant<Types...>& b)
    -> decltype(visit_binary_impl<0>(vis, a, b))
{
    assert(a.index() == b.index());
    return visit_binary_impl<0>(vis, a, b);
}

struct CopyVisitor
{
    template <typename T>
    void* operator()(const T& t) const
    {
        return new T(t);
    }
};

struct DeleteVisitor
{
    template <typename T>
    void operator()(T& t) const
    {
        delete &t;
    }
};

template <typename... Types>
class DynamicVariant
{
    static_assert(absl::conjunction<std::is_object<Types>...>::value,
                  "Attempted to instantiate a variant containing a non-object "
                  "type.");
    static_assert(absl::conjunction<absl::negation<std::is_array<Types>>...>::value,
                  "Attempted to instantiate a variant containing an array type.");

    template <typename T>
    using ExactlyOnce = detail::ExactlyOnce<T, Types...>;
    template <typename T>
    using AcceptedIndex = detail::AcceptedIndex<T&&, DynamicVariant>;

    template <size_t i, bool = (i < sizeof...(Types))>
    struct ToTypeImpl
    {
    };

    template <size_t i>
    struct ToTypeImpl<i, true>
    {
        using type = variant_alternative_t<i, DynamicVariant>;
    };

    template <size_t i>
    using ToType = typename ToTypeImpl<i>::type;

    template <typename T>
    using AcceptedType = ToType<AcceptedIndex<T>::value>;

  public:
    DynamicVariant() : m_index(0), m_data(new ToType<0>()) {}
    DynamicVariant(const DynamicVariant& other) : m_index(other.m_index), m_data(fast_ber::visit(CopyVisitor{}, other))
    {
    }
    DynamicVariant(DynamicVariant&& other) noexcept : m_index(std::move(other.m_index)), m_data(std::move(other.m_data))
    {
        other.m_data = nullptr;
    }

    template <typename T, typename = absl::enable_if_t<!std::is_same<absl::decay_t<T>, DynamicVariant>::value>,
              typename = absl::enable_if_t<ExactlyOnce<AcceptedType<T&&>>::value &&
                                           std::is_constructible<AcceptedType<T&&>, T&&>::value>>
    DynamicVariant(T&& t) noexcept(std::is_nothrow_constructible<AcceptedType<T&&>, T&&>::value)
        : m_index(detail::IndexOf<AcceptedType<T&&>, Types...>::value), m_data(new AcceptedType<T&&>(t))
    {
        assert(holds_alternative<AcceptedType<T&&>>(*this));
    }

    template <typename T, typename... Args,
              typename = absl::enable_if_t<ExactlyOnce<T>::value && std::is_constructible<T, Args&&...>::value>>
    explicit DynamicVariant(in_place_type_t<T>, Args&&... args)
        : m_index(detail::IndexOf<T, Types...>::value), m_data(new T(std::forward<Args>(args)...))

    {
        assert(holds_alternative<T>(*this));
    }

    template <typename T, typename U, typename... Args,
              typename = absl::enable_if_t<ExactlyOnce<T>::value &&
                                           std::is_constructible<T, std::initializer_list<U>&, Args&&...>::value>>
    explicit DynamicVariant(in_place_type_t<T>, std::initializer_list<U> il, Args&&... args)
        : m_index(detail::IndexOf<T, Types...>::value), m_data(new T(il, args...))
    {
        assert(holds_alternative<T>(*this));
    }

    template <size_t i, typename... Args,
              typename = absl::enable_if_t<std::is_constructible<ToType<i>, Args&&...>::value>>
    explicit DynamicVariant(in_place_index_t<i>, Args&&... args)
        : m_index(i), m_data(new ToType<i>(std::forward<Args>(args)...))
    {
        assert(index() == i);
    }

    template <
        size_t i, typename U, typename... Args,
        typename = absl::enable_if_t<std::is_constructible<ToType<i>, std::initializer_list<U>&, Args&&...>::value>>
    explicit DynamicVariant(in_place_index_t<i>, std::initializer_list<U> il, Args&&... args)
        : m_index(i), m_data(new ToType<i>(il, std::forward<Args>(args)...))

    {
        assert(index() == i);
    }

    ~DynamicVariant()
    {
        if (!valueless_by_exception() && m_data != nullptr)
        {
            fast_ber::visit(DeleteVisitor(), *this);
        }
    }

    DynamicVariant& operator=(const DynamicVariant& other)
    {
        if (!valueless_by_exception() && m_data != nullptr)
        {
            fast_ber::visit(DeleteVisitor(), *this);
        }
        m_index = other.m_index;
        m_data  = fast_ber::visit(CopyVisitor(), other);

        return *this;
    }

    DynamicVariant& operator=(DynamicVariant&& other) noexcept
    {
        if (!valueless_by_exception() && m_data != nullptr)
        {
            fast_ber::visit(DeleteVisitor(), *this);
        }
        m_index = other.m_index;
        m_data  = other.m_data;

        other.m_data = nullptr;
        return *this;
    }

    template <typename T, typename = absl::enable_if_t<!std::is_same<absl::decay_t<T>, DynamicVariant>::value>>
    absl::enable_if_t<ExactlyOnce<AcceptedType<T&&>>::value && std::is_constructible<AcceptedType<T&&>, T&&>::value &&
                          std::is_assignable<AcceptedType<T&&>&, T&&>::value,
                      DynamicVariant&>
    operator=(T&& t) noexcept(std::is_nothrow_assignable<AcceptedType<T&&>&, T&&>::value&&
                                  std::is_nothrow_constructible<AcceptedType<T&&>, T&&>::value)
    {
        if (!valueless_by_exception() && m_data != nullptr)
        {
            fast_ber::visit(DeleteVisitor(), *this);
        }
        m_index = AcceptedIndex<T>::value;
        m_data  = new AcceptedType<T>(t);
        return *this;
    }

    template <typename T, typename... Args,
              typename = absl::enable_if_t<std::is_constructible<T, Args...>::value && ExactlyOnce<T>::value>>
    T& emplace(Args&&... args)
    {
        if (!valueless_by_exception() && m_data != nullptr)
        {
            fast_ber::visit(DeleteVisitor(), *this);
        }
        m_index = AcceptedIndex<T>::value;
        m_data  = new AcceptedType<T>(std::forward<Args>(args)...);

        assert(holds_alternative<T>(*this));
        return *static_cast<T*>(m_data);
    }

    template <typename T, typename U, typename... Args,
              typename = absl::enable_if_t<std::is_constructible<T, std::initializer_list<U>&, Args...>::value &&
                                           ExactlyOnce<T>::value>>
    T& emplace(std::initializer_list<U> il, Args&&... args)
    {
        if (!valueless_by_exception() && m_data != nullptr)
        {
            fast_ber::visit(DeleteVisitor(), *this);
        }
        m_index = AcceptedIndex<T>::value;
        m_data  = new T(std::forward<Args>(il, args)...);

        assert(holds_alternative<T>(*this));
        return *static_cast<T*>(m_data);
    }

    template <size_t i, typename... Args>
    absl::enable_if_t<std::is_constructible<variant_alternative_t<i, DynamicVariant>, Args...>::value,
                      variant_alternative_t<i, DynamicVariant>&>
    emplace(Args&&... args)
    {
        static_assert(i < sizeof...(Types), "The index should be in [0, number of alternatives)");

        if (!valueless_by_exception() && m_data != nullptr)
        {
            fast_ber::visit(DeleteVisitor(), *this);
        }

        try
        {
            m_index = i;
            m_data  = new ToType<i>(std::forward<Args>(args)...);
        }
        catch (...)
        {
            m_index = variant_npos;
            throw;
        }
        assert(index() == i);
        return get<i>(*this);
    }

    template <size_t i, typename U, typename... Args>
    absl::enable_if_t<
        std::is_constructible<variant_alternative_t<i, DynamicVariant>, std::initializer_list<U>&, Args...>::value,
        variant_alternative_t<i, DynamicVariant>&>
    emplace(std::initializer_list<U> il, Args&&... args)
    {
        static_assert(i < sizeof...(Types), "The index should be in [0, number of alternatives)");

        if (!valueless_by_exception() && m_data != nullptr)
        {
            fast_ber::visit(DeleteVisitor(), *this);
        }

        try
        {
            m_index = i;
            m_data  = new ToType<i>(std::forward<Args>(il, args)...);
        }
        catch (...)
        {
            m_index = variant_npos;
            throw;
        }
        assert(index() == i);
        return get<i>(*this);
    }

    constexpr bool valueless_by_exception() const noexcept { return m_index == variant_npos; }

    constexpr std::size_t index() const noexcept { return m_index; }

    void swap(DynamicVariant& rhs) noexcept
    {
        std::swap(m_index, rhs.m_index);
        std::swap(m_data, rhs.m_data);
    }

    template <typename T>
    T& value()
    {
        return *reinterpret_cast<T*>(m_data);
    }

    template <typename T>
    const T& value() const
    {
        return *reinterpret_cast<const T*>(m_data);
    }

  private:
    size_t m_index;
    void*  m_data;
}; // namespace fast_ber

static_assert(std::is_copy_constructible<DynamicVariant<int, char>>::value, "");
static_assert(std::is_move_constructible<DynamicVariant<int, char>>::value, "");
static_assert(std::is_copy_assignable<DynamicVariant<int, char>>::value, "");
static_assert(std::is_move_assignable<DynamicVariant<int, char>>::value, "");

struct Equals
{
    template <typename T>
    bool operator()(const T& lhs, const T& rhs)
    {
        return lhs == rhs;
    }
};

struct LessThan
{
    template <typename T>
    bool operator()(const T& lhs, const T& rhs)
    {
        return lhs == rhs;
    }
};

struct GreaterThan
{
    template <typename T>
    bool operator()(const T& lhs, const T& rhs)
    {
        return lhs > rhs;
    }
};

struct LessThanOrEquals
{
    template <typename T>
    bool operator()(const T& lhs, const T& rhs)
    {
        return lhs <= rhs;
    }
};

struct GreaterThanOrEquals
{
    template <typename T>
    bool operator()(const T& lhs, const T& rhs)
    {
        return lhs >= rhs;
    }
};

template <typename... Types>
bool operator==(const DynamicVariant<Types...>& a, const DynamicVariant<Types...>& b)
{
    return (a.index() == b.index()) && visit_binary(Equals(), a, b);
}

template <typename... Types>
bool operator!=(const DynamicVariant<Types...>& a, const DynamicVariant<Types...>& b)
{
    return (a.index() != b.index()) || !visit_binary(Equals(), a, b);
}

template <typename... Types>
bool operator<(const DynamicVariant<Types...>& a, const DynamicVariant<Types...>& b)
{
    return (a.index() != b.index()) ? (a.index() + 1) < (b.index() + 1) : visit_binary(LessThan(), a, b);
}

template <typename... Types>
bool operator>(const DynamicVariant<Types...>& a, const DynamicVariant<Types...>& b)
{
    return (a.index() != b.index()) ? (a.index() + 1) > (b.index() + 1) : visit_binary(GreaterThan(), a, b);
}

template <typename... Types>
bool operator<=(const DynamicVariant<Types...>& a, const DynamicVariant<Types...>& b)
{
    return (a.index() != b.index()) ? (a.index() + 1) < (b.index() + 1) : visit_binary(LessThanOrEquals(), a, b);
}

template <typename... Types>
bool operator>=(const DynamicVariant<Types...>& a, const DynamicVariant<Types...>& b)
{
    return (a.index() != b.index()) ? (a.index() + 1) > (b.index() + 1) : visit_binary(GreaterThanOrEquals(), a, b);
}

} // namespace fast_ber
