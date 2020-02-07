#include "autogen/circular.hpp"

#include "catch2/catch.hpp"

TEST_CASE("Circular Types: Optional Members")
{
    fast_ber::Circular::CircularSequence1<> copy;
    fast_ber::Circular::CircularSequence1<> circular = {
        "layer1",
        fast_ber::Circular::CircularSequence2<>{fast_ber::Circular::CircularSequence1<>{
            "layer2",
            fast_ber::Circular::CircularSequence2<>{fast_ber::Circular::CircularSequence1<>{
                "layer3", fast_ber::Circular::CircularSequence2<>{fast_ber::Circular::CircularSequence1<>{
                              "layer4", fast_ber::Circular::CircularSequence2<>{
                                            fast_ber::Circular::CircularSequence1<>{"layer5", fast_ber::empty}}}}}}}}};

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

TEST_CASE("Circular Types: Choice")
{
    fast_ber::Circular::CircularChoice1<> copy;
    fast_ber::Circular::CircularChoice1<> circular = fast_ber::Circular::CircularChoice2<>{"bla", {}};

    REQUIRE(copy != circular);

    std::vector<uint8_t>          buffer(500, 0);
    const fast_ber::EncodeResult& encode_res = fast_ber::encode(absl::Span<uint8_t>(buffer), circular);
    const fast_ber::DecodeResult& decode_res = fast_ber::decode(absl::Span<uint8_t>(buffer), copy);

    REQUIRE(encode_res.success);
    REQUIRE(decode_res.success);
    REQUIRE(copy == circular);
    REQUIRE(fast_ber::get<0>(fast_ber::get<1>(copy).choice) == 0);
}

TEST_CASE("Circular Types: Choice Self Reference")
{
    using String = fast_ber::OctetString<fast_ber::Id<fast_ber::Class::context_specific, 0>>;
    using Self   = fast_ber::Circular::SelfReference<fast_ber::Id<fast_ber::Class::context_specific, 1>>;

    Self copy;
    Self circular;

    circular.emplace<1>().emplace<1>().emplace<1>().emplace<0>(String("Hello!"));

    REQUIRE(copy != circular);

    std::vector<uint8_t>          buffer(500, 0);
    const size_t                  encode_res_len = fast_ber::encoded_length(circular);
    const fast_ber::EncodeResult& encode_res     = fast_ber::encode(absl::Span<uint8_t>(buffer), circular);
    const fast_ber::DecodeResult& decode_res     = fast_ber::decode(absl::Span<uint8_t>(buffer), copy);

    REQUIRE(encode_res.success);
    REQUIRE(decode_res.success);
    REQUIRE(encode_res_len == encode_res.length);
    REQUIRE(copy == circular);
    REQUIRE(fast_ber::get<0>(fast_ber::get<1>(fast_ber::get<1>(fast_ber::get<1>(copy)))) == "Hello!");
}
