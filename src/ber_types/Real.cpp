#include "fast_ber/ber_types/Real.hpp"

#include <regex>

namespace fast_ber
{

const std::regex g_iso_6093_nr1{"^[ ]*[+-]?[0-9]+$"};
const std::regex g_iso_6093_nr2{"^[ ]*[+-]?([0-9]+[\\.\\,]{1}[0-9]*|[0-9]*[\\.\\,]{1}[0-9]+)$"};
const std::regex g_iso_6093_nr3{"^[ ]*[+-]?([0-9]+[\\.\\,]{1}[0-9]*|[0-9]*[\\.\\,]{1}[0-9]+)[Ee][+-]?[0-9]+$"};

/**
 * Decodes a real decimal value
 * @param input BER encoded real decimal value
 * @param output decoded decimal value
 * @return true on success, false on failure
 */
bool decode_real_decimal(absl::Span<const uint8_t> input, double& output) noexcept
{
    std::string iso_6093_value(reinterpret_cast<const char*>(&input[1]), input.length() - 1);

    /* Check that string matches the iso 6093 NR1/NR2/NR3 forms */
    if (std::regex_match(iso_6093_value, g_iso_6093_nr1) || std::regex_match(iso_6093_value, g_iso_6093_nr2) ||
        std::regex_match(iso_6093_value, g_iso_6093_nr3))
    {
        /* Replace potential comma by a dot */
        std::replace(iso_6093_value.begin(), iso_6093_value.end(), ',', '.');

        return absl::SimpleAtod(iso_6093_value, &output);
    }
    else
    {
        return false;
    }
}

} // namespace fast_ber
