#pragma once

#define FAST_BER_ALIAS(X, ...)                                                                                         \
    struct X : __VA_ARGS__                                                                                             \
    {                                                                                                                  \
        using AliasedType = __VA_ARGS__;                                                                               \
        using AliasedType::AliasedType;                                                                                \
        using AliasedType::operator=;                                                                                  \
        X()                        = default;                                                                          \
        X(const X&)                = default;                                                                          \
        X(X&&)                     = default;                                                                          \
        X(const AliasedType& a) : AliasedType(a) {}                                                                    \
        X(AliasedType&& a) : AliasedType(std::move(a)) {}                                                              \
        X&                 operator=(const X&) = default;                                                              \
        AliasedType&       get_base() { return *this; }                                                                \
        const AliasedType& get_base() const { return *this; }                                                          \
    }
