#include "autogen/pokemon.hpp"
#include <fstream>
#include <iostream>

int main()
{
    fast_ber::Pokemon::Team    team;
    fast_ber::Pokemon::Pokemon muchlax = {"Munchlax", "Big Eater", "Normal", "Thick Fat, Pickup", "Fighting", 105};
    fast_ber::Pokemon::Pokemon piplup  = {"Piplup", "Penguin", "Water", "Torrent", "Electric, Grass", 12};
    team.members.push_back(muchlax);
    team.members.push_back(piplup);

    std::array<uint8_t, 2000> buffer;
    const auto                encode_result = fast_ber::encode(absl::MakeSpan(buffer.data(), buffer.size()), team);
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
