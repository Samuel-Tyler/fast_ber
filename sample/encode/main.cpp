#include "autogen/pokemon.hpp"
#include <fstream>
#include <iostream>

int main()
{
    fast_ber::Pokemon::Team    team{"Sam's Team"};
    fast_ber::Pokemon::Pokemon muchlax = {"Munchlax",          "Big Eater", fast_ber::Pokemon::Type::normal,
                                          "Thick Fat, Pickup", "Fighting",  105};
    fast_ber::Pokemon::Pokemon piplup  = {"Piplup",  "Penguin",         fast_ber::Pokemon::Type::water,
                                         "Torrent", "Electric, Grass", 12};
    team.members.push_back(muchlax);
    team.members.push_back(piplup);

    std::array<uint8_t, 2000> buffer{};
    const auto                encode_result = fast_ber::encode(absl::Span<uint8_t>(buffer), team);
    if (!encode_result.success)
    {
        std::cout << "Failed to encode data\n";
        return -1;
    }

    std::ofstream output("pokemon.ber");
    if (!output.good())
    {
        std::cout << "Failed to open output file: pokemon.ber\n";
        return -1;
    }
    output.write(reinterpret_cast<const char*>(buffer.data()), static_cast<std::streamsize>(encode_result.length));
    return 0;
}
