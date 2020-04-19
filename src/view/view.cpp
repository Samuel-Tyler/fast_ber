#include "fast_ber/util/BerView.hpp"

#include <fstream>
#include <iostream>
#include <vector>

void usage()
{
    std::cout << "Tool to inspect the contents of a BER PDU. Does not require schema.\n";
    std::cout << "  usage: ./ber_view [pdu_to_decode.ber]\n";
}

int main(int argc, const char** argv)
{
    if (argc != 2)
    {
        usage();
        return -1;
    }

    auto          input_path = std::string(argv[1]);
    std::ifstream input(input_path, std::ios::binary);
    if (!input.good())
    {
        std::cout << "Failed to open input: " << input_path << std::endl;
        return -1;
    }

    const auto buffer = std::vector<uint8_t>{std::istreambuf_iterator<char>(input), std::istreambuf_iterator<char>()};
    auto       view   = fast_ber::BerView(buffer);

    std::cout << view;
    return view.is_valid() ? 0 : -1;
}
