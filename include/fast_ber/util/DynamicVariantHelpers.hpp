#pragma once

#include <cstddef>
#include <tuple>
#include <type_traits>

#include <absl/types/variant.h>

namespace fast_ber
{

namespace dynamic
{

template <typename... Ts>
class DynamicVariant;

static constexpr size_t variant_npos = size_t(-1);

namespace detail
{

template <class Variant, std::size_t i = 0>
struct ImaginaryFun;

template <std::size_t i>
struct ImaginaryFun<DynamicVariant<>, i>
{
    static void Run() = delete;
};

template <class H, class... T, std::size_t i>
struct ImaginaryFun<DynamicVariant<H, T...>, i> : ImaginaryFun<DynamicVariant<T...>, i + 1>
{
    using ImaginaryFun<DynamicVariant<T...>, i + 1>::Run;

    static std::integral_constant<std::size_t, i> Run(const H&, std::integral_constant<std::size_t, i>);
    static std::integral_constant<std::size_t, i> Run(H&&, std::integral_constant<std::size_t, i>);
};

template <typename T, typename Variant>
using FunType = decltype(ImaginaryFun<Variant>::Run(std::declval<T>(), {}));

template <typename T, typename Variant, typename = void>
struct AcceptedIndex : std::integral_constant<size_t, variant_npos>
{
};

template <typename T, typename Variant>
struct AcceptedIndex<T, Variant, absl::void_t<decltype(ImaginaryFun<Variant>::Run(std::declval<T>(), {}))>>
    : decltype(ImaginaryFun<Variant>::Run(std::declval<T>(), {}))
{
};

static_assert(AcceptedIndex<std::string, DynamicVariant<int>>::value == variant_npos, "");
static_assert(AcceptedIndex<int, DynamicVariant<int>>::value != variant_npos, "");
static_assert(AcceptedIndex<int, DynamicVariant<int>>::value == 0, "");
static_assert(AcceptedIndex<bool, DynamicVariant<char, int, bool>>::value == 2, "");

template <typename T, typename Tuple>
struct TupleCount;

template <typename T, typename... Types>
struct TupleCount<T, std::tuple<Types...>> : std::integral_constant<size_t, 0>
{
};

template <typename T, typename First, typename... Rest>
struct TupleCount<T, std::tuple<First, Rest...>>
    : std::integral_constant<size_t, TupleCount<T, std::tuple<Rest...>>::value + std::is_same<T, First>::value>
{
};

template <typename T, typename... Types>
struct ExactlyOnce : std::integral_constant<bool, TupleCount<T, std::tuple<Types...>>::value == 1>
{
};

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

template <typename T, typename... Types>
struct IndexOf : std::integral_constant<size_t, 0>
{
};

template <typename T, typename First, typename... Rest>
struct IndexOf<T, First, Rest...>
    : std::integral_constant<size_t, std::is_same<T, First>::value ? 0 : IndexOf<T, Rest...>::value + 1>
{
};

} // namespace detail
} // namespace dynamic
} // namespace fast_ber
