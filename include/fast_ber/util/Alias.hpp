#pragma once

#define FAST_BER_ALIAS(X, Y)                                                                                           \
    struct X : Y                                                                                                       \
    {                                                                                                                  \
        using Y::Y;                                                                                                    \
        X() = default;                                                                                                 \
        X(const Y& y) : Y(y) {}                                                                                        \
        X(Y&& y) : Y(std::move(y)) {}                                                                                  \
        using Id          = fast_ber::Identifier<Y>;                                                                   \
        using AliasedType = Y;                                                                                         \
        Y&       get_base() { return *this; }                                                                          \
        const Y& get_base() const { return *this; }                                                                    \
    }
