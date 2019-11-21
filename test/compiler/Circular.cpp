#include "autogen/circular.hpp"

#include "catch2/catch.hpp"

TEST_CASE("Circular Types: Optional Members")
{
    fast_ber::Circular::CircularSequence1<> copy;
    fast_ber::Circular::CircularSequence1<> circular = {"layer1",
                                                        {{"layer2", {{"layer3", {{"layer4", {{"layer5", {}}}}}}}}}};

    REQUIRE(copy != circular);

    std::vector<uint8_t>          buffer(500, 0);
    const fast_ber::EncodeResult& encode_res = fast_ber::encode(absl::Span<uint8_t>(buffer), circular);
    const fast_ber::DecodeResult& decode_res = fast_ber::decode(absl::Span<uint8_t>(buffer), copy);

    REQUIRE(encode_res.success);
    REQUIRE(decode_res.success);
    REQUIRE(copy == circular);
    REQUIRE(copy.cs2->cs1.cs2->cs1.cs2->cs1.cs2->cs1.name == "layer5");
}

TEST_CASE("Circular Types: Sequence Of")
{
    fast_ber::Circular::CircularSequenceOf1<> copy;
    fast_ber::Circular::CircularSequenceOf1<> circular = {{{{{}, {}, {}, {}}}, {}, {{{}, {}}}, {}, {{{}, {}, {}}}}};
    REQUIRE(copy != circular);

    std::vector<uint8_t>          buffer(500, 0);
    const fast_ber::EncodeResult& encode_res = fast_ber::encode(absl::Span<uint8_t>(buffer), circular);
    const fast_ber::DecodeResult& decode_res = fast_ber::decode(absl::Span<uint8_t>(buffer), copy);

    REQUIRE(encode_res.success);
    REQUIRE(decode_res.success);
    REQUIRE(copy == circular);
    REQUIRE(copy.sequence[0].sequence.size() == 4);
}
