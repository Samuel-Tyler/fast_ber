#pragma once

#include "absl/memory/memory.h"
#include "absl/types/internal/variant.h"
#include "absl/types/variant.h"

namespace fast_ber
{

template <size_t I, typename... Tn>
struct TypeAtIndexImpl;

template <typename T0, typename... Tn>
struct TypeAtIndexImpl<0, T0, Tn...>
{
    using Type = typename std::remove_reference<T0>::type;
};

template <size_t I, typename T0, typename... Tn>
struct TypeAtIndexImpl<I, T0, Tn...>
{
    using Type = typename TypeAtIndexImpl<I - 1, Tn...>::Type;
};

template <size_t I, typename... Args>
using TypeAtIndex = typename TypeAtIndexImpl<I, Args...>::Type;

static_assert(std::is_same<TypeAtIndex<0, int, char>, int>::value, "");
static_assert(std::is_same<TypeAtIndex<1, int, char>, char>::value, "");

template <typename T, typename... Args>
using VariantIndex = absl::variant_internal::UnambiguousIndexOf<absl::variant<Args...>, T>;

template <typename... Ts>
class DynamicVariant;

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
    using type = TypeAtIndex<I, Types...>;
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
    static_assert(absl::variant_internal::UnambiguousIndexOfImpl<absl::variant<Types...>, T, 0>::value !=
                      sizeof...(Types),
                  "The type T must occur exactly once in Types...");
    return v.index() == VariantIndex<T, Types...>::value;
}

template <class T, class... Types>
T& get(DynamicVariant<Types...>& v)
{
    if (v.index() != absl::variant_internal::IndexOf<T, Types...>::value)
    {
        throw BadVariantAccess();
    }
    return v.template value<T>();
}

template <class T, class... Types>
T&& get(DynamicVariant<Types...>&& v)
{
    if (v.index() != absl::variant_internal::IndexOf<T, Types...>::value)
    {
        throw BadVariantAccess();
    }
    return v.template value<T>();
}

template <class T, class... Types>
const T& get(const DynamicVariant<Types...>& v)
{
    if (v.index() != absl::variant_internal::IndexOf<T, Types...>::value)
    {
        throw BadVariantAccess();
    }
    return v.template value<T>();
}

template <class T, class... Types>
const T&& get(const DynamicVariant<Types...>&& v)
{
    if (v.index() != absl::variant_internal::IndexOf<T, Types...>::value)
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
    return fast_ber::get_if<absl::variant_internal::IndexOf<T, Types...>::value>(v);
}

template <class T, class... Types>
constexpr typename std::add_pointer<const T>::type get_if(const DynamicVariant<Types...>* v) noexcept
{
    return fast_ber::get_if<absl::variant_internal::IndexOf<T, Types...>::value>(v);
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

template <typename T0, typename... Tn>
class DynamicVariant<T0, Tn...>
{
    static_assert(absl::conjunction<std::is_object<T0>, std::is_object<Tn>...>::value,
                  "Attempted to instantiate a variant containing a non-object "
                  "type.");
    static_assert(absl::conjunction<absl::negation<std::is_array<T0>>, absl::negation<std::is_array<Tn>>...>::value,
                  "Attempted to instantiate a variant containing an array type.");

  public:
    DynamicVariant() : m_index(0), m_data(new T0()) {}
    DynamicVariant(const DynamicVariant& other) : m_index(other.m_index), m_data(visit(CopyVisitor{}, other)) {}
    DynamicVariant(DynamicVariant&& other) noexcept : m_index(std::move(other.m_index)), m_data(std::move(other.m_data))
    {
        other.m_data = nullptr;
    }

    template <class T,
              std::size_t I = std::enable_if<
                  absl::variant_internal::IsNeitherSelfNorInPlace<absl::variant<T0, Tn...>, absl::decay_t<T>>::value,
                  absl::variant_internal::IndexOfConstructedType<absl::variant<T0, Tn...>, T>>::type::value,
              class Tj = variant_alternative_t<I, DynamicVariant>,
              typename std::enable_if<std::is_constructible<Tj, T>::value>::type* = nullptr>
    DynamicVariant(T&& t) : m_index(I), m_data(new Tj(std::forward<T>(t)))
    {
    }

    template <class T, class... Args>
    constexpr explicit DynamicVariant(absl::in_place_type_t<T>, Args&&... args)
        : m_index(absl::variant_internal::UnambiguousIndexOf<absl::variant<T0, Tn...>, T>::value),
          m_data(new T(absl::forward<Args>(args)...))
    {
    }

    template <class T, class U, class... Args>
    constexpr explicit DynamicVariant(absl::in_place_type_t<T>, std::initializer_list<U> il, Args&&... args)
        : m_index(absl::variant_internal::UnambiguousIndexOf<absl::variant<T0, Tn...>, T>::value),
          m_data(new T(il, absl::forward<Args>(args)...))
    {
    }

    template <std::size_t I, class... Args>
    constexpr explicit DynamicVariant(absl::in_place_index_t<I>, Args&&... args)
        : m_index(I), m_data(new TypeAtIndex<I, T0, Tn...>(std::forward<Args>(args)...))
    {
    }

    ~DynamicVariant()
    {
        if (!valueless_by_exception() && m_data != nullptr)
        {
            visit(DeleteVisitor(), *this);
        }
    }

    DynamicVariant& operator=(const DynamicVariant& other)
    {
        if (!valueless_by_exception() && m_data != nullptr)
        {
            visit(DeleteVisitor(), *this);
        }
        m_index = other.m_index;
        m_data  = visit(CopyVisitor(), other);

        return *this;
    }

    DynamicVariant& operator=(DynamicVariant&& other) noexcept
    {
        m_index = other.m_index;
        m_data  = other.m_data;

        other.m_data = nullptr;
        return *this;
    }

    template <class T,
              std::size_t I = std::enable_if<
                  absl::variant_internal::IsNeitherSelfNorInPlace<absl::variant<T0, Tn...>, absl::decay_t<T>>::value,
                  absl::variant_internal::IndexOfConstructedType<absl::variant<T0, Tn...>, T>>::type::value,
              class Tj = variant_alternative_t<I, DynamicVariant>,
              typename std::enable_if<std::is_constructible<Tj, T>::value>::type* = nullptr>
    DynamicVariant&
    operator=(T&& t) noexcept(std::is_nothrow_assignable<Tj&, T>::value&& std::is_nothrow_constructible<Tj, T>::value)
    {
        if (!valueless_by_exception() && m_data != nullptr)
        {
            visit(DeleteVisitor(), *this);
        }
        m_index = I;
        m_data  = new Tj(t);
        return *this;
    }

    template <
        class T, class... Args,
        typename std::enable_if<std::is_constructible<
            variant_alternative_t<VariantIndex<T, T0, Tn...>::value, DynamicVariant>, Args...>::value>::type* = nullptr>
    T& emplace(Args&&... args)
    {
        if (!valueless_by_exception() && m_data != nullptr)
        {
            visit(DeleteVisitor(), *this);
        }
        m_index = VariantIndex<T, T0, Tn...>::value;
        m_data  = new T(args...);

        return *static_cast<T*>(m_data);
    }
    /*
           // Constructs a value of the given alternative type T within the variant using
           // an initializer list.
           //
           // Example:
           //
           //   absl::variant<std::vector<int>, int, std::string> v;
           //   v.emplace<std::vector<int>>({0, 1, 2});
           template <class T, class U, class... Args,
                     typename std::enable_if<std::is_constructible<
                         absl::variant_alternative_t<variant_internal::UnambiguousIndexOf<variant, T>::value, variant>,
                         std::initializer_list<U>&, Args...>::value>::type* = nullptr>
           T& emplace(std::initializer_list<U> il, Args&&... args)
           {
               return variant_internal::VariantCoreAccess::Replace<variant_internal::UnambiguousIndexOf<variant,
       T>::value>( this, il, absl::forward<Args>(args)...);
           }
*/
    template <std::size_t I, class... Args,
              typename std::enable_if<
                  std::is_constructible<variant_alternative_t<I, DynamicVariant>, Args...>::value>::type* = nullptr>
    variant_alternative_t<I, DynamicVariant>& emplace(Args&&... args)
    {
        if (!valueless_by_exception() && m_data != nullptr)
        {
            visit(DeleteVisitor(), *this);
        }
        m_index = I;
        m_data  = new variant_alternative_t<I, DynamicVariant>(args...);

        return *static_cast<variant_alternative_t<I, DynamicVariant>*>(m_data);
    }
    /*
       template <std::size_t I, class U, class... Args,
                 typename std::enable_if<std::is_constructible<
                     absl::variant_alternative_t<I, variant>, std::initializer_list<U>&, Args...>::value>::type* =
       nullptr> absl::variant_alternative_t<I, variant>& emplace(std::initializer_list<U> il, Args&&... args)
       {

           return variant_internal::VariantCoreAccess::Replace<I>(this, il, absl::forward<Args>(args)...);
       }
     */
    constexpr bool valueless_by_exception() const noexcept { return m_index == absl::variant_npos; }

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
};

template <>
class DynamicVariant<>;

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
absl::variant_internal::RequireAllHaveEqualT<Types...> operator==(const DynamicVariant<Types...>& a,
                                                                  const DynamicVariant<Types...>& b)
{
    return (a.index() == b.index()) && visit_binary(Equals(), a, b);
}

template <typename... Types>
absl::variant_internal::RequireAllHaveNotEqualT<Types...> operator!=(const DynamicVariant<Types...>& a,
                                                                     const DynamicVariant<Types...>& b)
{
    return (a.index() != b.index()) || !visit_binary(Equals(), a, b);
}

template <typename... Types>
absl::variant_internal::RequireAllHaveLessThanT<Types...> operator<(const DynamicVariant<Types...>& a,
                                                                    const DynamicVariant<Types...>& b)
{
    return (a.index() != b.index()) ? (a.index() + 1) < (b.index() + 1) : visit_binary(LessThan(), a, b);
}

template <typename... Types>
absl::variant_internal::RequireAllHaveGreaterThanT<Types...> operator>(const DynamicVariant<Types...>& a,
                                                                       const DynamicVariant<Types...>& b)
{
    return (a.index() != b.index()) ? (a.index() + 1) > (b.index() + 1) : visit_binary(GreaterThan(), a, b);
}

template <typename... Types>
absl::variant_internal::RequireAllHaveLessThanOrEqualT<Types...> operator<=(const DynamicVariant<Types...>& a,
                                                                            const DynamicVariant<Types...>& b)
{
    return (a.index() != b.index()) ? (a.index() + 1) < (b.index() + 1) : visit_binary(LessThanOrEquals(), a, b);
}

template <typename... Types>
absl::variant_internal::RequireAllHaveGreaterThanOrEqualT<Types...> operator>=(const DynamicVariant<Types...>& a,
                                                                               const DynamicVariant<Types...>& b)
{
    return (a.index() != b.index()) ? (a.index() + 1) > (b.index() + 1) : visit_binary(GreaterThanOrEquals(), a, b);
}

} // namespace fast_ber
