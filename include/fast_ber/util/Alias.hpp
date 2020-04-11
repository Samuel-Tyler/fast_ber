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
        X& operator=(const X&) = default;                                                                              \
        using AsnId            = fast_ber::Identifier<AliasedType>;                                                    \
        AliasedType&       get_base() { return *this; }                                                                \
        const AliasedType& get_base() const { return *this; }                                                          \
    }
