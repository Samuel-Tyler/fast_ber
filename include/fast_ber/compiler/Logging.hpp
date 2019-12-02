#pragma once

#include "fast_ber/compiler/CompilerTypes.hpp"

#include <iostream>
#include <string>

const static bool debug = true;

inline void log_debug(const Asn1Tree&, const std::string& message)
{
    if (debug)
    {
        std::cout << message << "\n";
    }
}
