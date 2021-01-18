#pragma once

#include <iostream>

#define FAST_BER_ERROR(...) ::fast_ber::handle_error(__VA_ARGS__)

namespace fast_ber
{

inline void handle_error_impl() { std::cerr << std::endl; }

template <typename T, typename... Args>
void handle_error_impl(const T& t, Args&&... args)
{
    std::cerr << t;
    handle_error_impl(args...);
}

template <typename... Args>
void handle_error(Args&&... args)
{
    handle_error_impl(args...);
}

} // namespace fast_ber
