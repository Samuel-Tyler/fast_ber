#pragma once

#include "fast_ber/compiler/CompilerTypes.hpp"

#include <iostream>
#include <string>

const static bool debug = false;

inline void log_debug(const Asn1Tree&, const std::string& message)
{
    if (debug)
    {
        std::cout << message << "\n";
    }
}

template <typename Location>
inline void feature_not_implemented(const Location& location, const Asn1Tree&, const std::string& feature,
                                    const std::string& message = "")
{
    std::cerr << location << ": WARNING: The feature " << feature << " has not yet been implemented. " << message
              << "Please contact fast_ber developer.\n";
}
