#include "fast_ber/ber_types/All.hpp"
#include "clock.hpp"

#include <fstream>

int main()
{
    std::array<uint8_t, 2000> buffer;
    std::ofstream output("test_out.ber");

    fast_ber::Clock::Collection collection;
    size_t encode_size = fast_ber::Clock::encode(absl::Span(buffer.data(), buffer.size()), collection);
    output.write(reinterpret_cast<const char*>(buffer.data()), encode_size);
}
