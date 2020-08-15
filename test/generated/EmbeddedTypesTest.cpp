#include "autogen/embedded.hpp"

#include "catch2/catch.hpp"

#include <vector>

TEST_CASE("Embedded Types: Encode and decode nested structures")
{
    fast_ber::Simple::OuterCollection collection;
    fast_ber::Simple::OuterCollection copy;

    collection.middle_collection.inner_collection.string = "We're really deep in this";
    collection.outer_choice                              = 500;

    REQUIRE(copy != collection);

    std::vector<uint8_t>          buffer(500, 0);
    const fast_ber::EncodeResult& encode_res = fast_ber::encode(absl::Span<uint8_t>(buffer), collection);
    const fast_ber::DecodeResult& decode_res = fast_ber::decode(absl::Span<uint8_t>(buffer), copy);

    REQUIRE(encode_res.success);
    REQUIRE(decode_res.success);
    REQUIRE(copy == collection);
}
