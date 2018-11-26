#include "autogen/tags.hpp"

#include "fast_ber/util/BerView.hpp"

#include "catch2/catch.hpp"

#include <fstream>
#include <vector>

TEST_CASE("Tags: Encoding a packet with various tagging modes")
{
    std::array<uint8_t, 5000> buffer;

    fast_ber::Tags::Collection tags{};

    fast_ber::EncodeResult encode_result = fast_ber::Tags::encode(absl::MakeSpan(buffer.data(), buffer.size()), tags);
    REQUIRE(encode_result.success);

    fast_ber::BerView tags_view(absl::MakeSpan(buffer.data(), encode_result.length));
    auto              iter = tags_view.begin();
    for (int i = 0; i < 6; ++i, ++iter)
    {
        REQUIRE(iter->tag() == i);
        REQUIRE(iter->is_valid());
        if (i == 0)
        {
            REQUIRE(iter->construction() == fast_ber::Construction::primitive);
            REQUIRE(iter->class_() == fast_ber::Class::context_specific);
        }
        else if (i == 1)
        {
            REQUIRE(iter->construction() == fast_ber::Construction::primitive);
            REQUIRE(iter->class_() == fast_ber::Class::context_specific);
        }
        else if (i == 2)
        {
            REQUIRE(iter->construction() == fast_ber::Construction::constructed);
            REQUIRE(iter->class_() == fast_ber::Class::context_specific);

            REQUIRE(iter->begin()->construction() == fast_ber::Construction::primitive);
            REQUIRE(iter->begin()->class_() == fast_ber::Class::universal);
        }
        else if (i == 3)
        {
            REQUIRE(iter->construction() == fast_ber::Construction::primitive);
            REQUIRE(iter->class_() == fast_ber::Class::context_specific);
        }
        else if (i == 4)
        {
            REQUIRE(iter->construction() == fast_ber::Construction::constructed);
            REQUIRE(iter->class_() == fast_ber::Class::private_);

            REQUIRE(iter->begin()->construction() == fast_ber::Construction::primitive);
            REQUIRE(iter->begin()->class_() == fast_ber::Class::universal);
        }
        else if (i == 5)
        {
            REQUIRE(iter->construction() == fast_ber::Construction::primitive);
            REQUIRE(iter->class_() == fast_ber::Class::application);
        }
    }
}

TEST_CASE("Tags: Encoding and decoding a packet with various tagging modes")
{
    std::array<uint8_t, 5000> buffer;

    fast_ber::Tags::Collection tags{"Implicit", "And explicit tags", 0, true, false, true};
    fast_ber::Tags::Collection tags_copy{};

    fast_ber::EncodeResult encode_result = fast_ber::Tags::encode(absl::MakeSpan(buffer.data(), buffer.size()), tags);
    REQUIRE(encode_result.success);
    bool decode_result = fast_ber::Tags::decode(absl::MakeSpan(buffer.data(), buffer.size()), tags_copy);
    REQUIRE(decode_result);

    REQUIRE(tags_copy.string1 == "Implicit");
    REQUIRE(tags_copy.string2 == "And explicit tags");
    REQUIRE(tags_copy.integer == 0);
    REQUIRE(tags_copy.bool1);
    REQUIRE(!tags_copy.bool2);
    REQUIRE(tags_copy.bool3);
}
