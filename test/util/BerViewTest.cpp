#include "fast_ber/util/BerView.hpp"

#include <catch2/catch.hpp>

#include <array>
#include <sstream>

const static std::array<uint8_t, 31> sample_packet = {0x30, 0x1d, 0x80, 0x06, 0x46, 0x61, 0x6c, 0x63, 0x6f, 0x6e, 0x82,
                                                      0x01, 0x00, 0xa3, 0x04, 0x80, 0x02, 0x46, 0x50, 0xa4, 0x0a, 0x0c,
                                                      0x03, 0x43, 0x61, 0x72, 0x0c, 0x03, 0x47, 0x50, 0x53};

const static std::array<uint8_t, 2> empty_packet = {0x00, 0x00};

TEST_CASE("BerView: Sample data")
{
    fast_ber::BerView view(sample_packet);

    REQUIRE(view.is_valid());
    REQUIRE(view.construction() == fast_ber::Construction::constructed);
    REQUIRE(view.class_() == fast_ber::Class::universal);
    REQUIRE(view.tag() == 16);

    REQUIRE(view.ber() == sample_packet);
    REQUIRE(view.content() == absl::MakeSpan(sample_packet.data() + 2, sample_packet.size() - 2));
}

TEST_CASE("BerView: Empty packet")
{
    fast_ber::BerView view(empty_packet);

    REQUIRE(view.is_valid());
    REQUIRE(view.construction() == fast_ber::Construction::primitive);
    REQUIRE(view.class_() == fast_ber::Class::universal);
    REQUIRE(view.tag() == 0);

    REQUIRE(view.ber() == empty_packet);
    REQUIRE(view.content().empty());
}

TEST_CASE("BerView: Output")
{
    fast_ber::BerView view(sample_packet);
    std::stringstream ss;

    ss << view;
    std::cout << view << std::endl;

    REQUIRE(
        ss.str() ==
        R"({ "length": 31, "identifier": { "class": "Universal", "tag": "Sequence / Sequence Of"}, "content": [{ "length": 8, "identifier": { "class": "Context Specific", "tag": 0}, "content": "Falcon"}, { "length": 3, "identifier": { "class": "Context Specific", "tag": 2}, "content": "binary data"}, { "length": 6, "identifier": { "class": "Context Specific", "tag": 3}, "content": [{ "length": 4, "identifier": { "class": "Context Specific", "tag": 0}, "content": "FP"}]}, { "length": 12, "identifier": { "class": "Context Specific", "tag": 4}, "content": [{ "length": 5, "identifier": { "class": "Universal", "tag": "UTF8 String"}, "content": "Car"}, { "length": 5, "identifier": { "class": "Universal", "tag": "UTF8 String"}, "content": "GPS"}]}]})");
}
