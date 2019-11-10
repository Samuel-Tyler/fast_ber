#pragma once

#define FAST_BER_ALIAS(X, ...)                                                                                         \
    struct X : __VA_ARGS__                                                                                             \
    {                                                                                                                  \
        using AliasedType = __VA_ARGS__;                                                                               \
        using AliasedType::AliasedType;                                                                                \
        X() = default;                                                                                                 \
        X(const AliasedType& y) : AliasedType(y) {}                                                                    \
        X(AliasedType&& y) : AliasedType(std::move(y)) {}                                                              \
        using Id = fast_ber::Identifier<AliasedType>;                                                                  \
        AliasedType&       get_base() { return *this; }                                                                \
        const AliasedType& get_base() const { return *this; }                                                          \
    }
