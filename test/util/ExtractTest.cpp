#include "fast_ber/util/Extract.hpp"

#include <catch2/catch.hpp>

#include <vector>

struct TestPacket
{
    TestPacket(const std::vector<uint8_t>& d, fast_ber::Tag t, uint64_t l) noexcept : data(d), tag(t), length(l) {}
    std::vector<uint8_t>         data;
    fast_ber::Tag                tag;
    uint64_t                     length;
};

const static std::vector<TestPacket> packets = {TestPacket{std::vector<uint8_t>{0x00, 0x00}, 0, 0},
                                                TestPacket{std::vector<uint8_t>{0x01, 0x01, 0x00}, 1, 1},
                                                TestPacket{std::vector<uint8_t>{0x02, 0x02, 0x00, 0x00}, 2, 2},
                                                TestPacket{std::vector<uint8_t>{0x03, 0x03, 0x00, 0x00, 0x00}, 3, 3}};

TEST_CASE("Extract: Tags")
{
    for (const auto& packet : packets)
    {
        fast_ber::Tag tag = 0xFF;
        REQUIRE(fast_ber::extract_tag(packet.data, tag) == 1);
        REQUIRE(tag == packet.tag);
    }
}

TEST_CASE("Extract: Lengths")
{
    for (const auto& packet : packets)
    {
        uint64_t length = 0xFF;
        REQUIRE(fast_ber::extract_length(packet.data, length, 1) == 1);
        REQUIRE(length == packet.length);
    };
}
