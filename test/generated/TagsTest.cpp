#include "autogen/tags.hpp"

#include "fast_ber/util/BerView.hpp"

#include "catch2/catch.hpp"

#include <vector>

TEST_CASE("Tags: Encoding a packet with various tagging modes")
{
    std::array<uint8_t, 5000>  buffer = {};
    fast_ber::Tags::Collection tags   = {};

    fast_ber::EncodeResult encode_result = fast_ber::encode(absl::MakeSpan(buffer.data(), buffer.size()), tags);
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
        else if (i == 6)
        {
            REQUIRE(iter->construction() == fast_ber::Construction::primitive);
            REQUIRE(iter->class_() == fast_ber::Class::application);
        }
    }
}

TEST_CASE("Tags: Unspecified tags of an automatic sequence should be sequential")
{
    fast_ber::Tags::DefaultCollection c1;
    std::vector<uint8_t>              buffer(1000, 0x00);

    c1.encode(absl::Span<uint8_t>(buffer));
    fast_ber::BerViewIterator itr = fast_ber::BerView(buffer).begin();

    CHECK(fast_ber::has_correct_header(*itr, fast_ber::ExplicitId<fast_ber::UniversalTag::octet_string>{},
                                       fast_ber::Construction::primitive));
    ++itr;
    CHECK(fast_ber::has_correct_header(*itr, fast_ber::ExplicitId<fast_ber::UniversalTag::octet_string>{},
                                       fast_ber::Construction::primitive));
    ++itr;
    CHECK(fast_ber::has_correct_header(*itr, fast_ber::ExplicitId<fast_ber::UniversalTag::octet_string>{},
                                       fast_ber::Construction::primitive));
    ++itr;

    fast_ber::AutomaticTags::DefaultCollection c2;
    c2.encode(absl::Span<uint8_t>(buffer));
    itr = fast_ber::BerView(buffer).begin();

    CHECK(fast_ber::has_correct_header(*itr, fast_ber::Id<fast_ber::Class::context_specific, 0>{},
                                       fast_ber::Construction::primitive));
    ++itr;
    CHECK(fast_ber::has_correct_header(*itr, fast_ber::Id<fast_ber::Class::context_specific, 1>{},
                                       fast_ber::Construction::primitive));
    ++itr;
    CHECK(fast_ber::has_correct_header(*itr, fast_ber::Id<fast_ber::Class::context_specific, 2>{},
                                       fast_ber::Construction::primitive));
    ++itr;
}

TEST_CASE("Tags: Encoding and decoding a packet with various tagging modes")
{
    std::array<uint8_t, 5000> buffer = {};

    fast_ber::Tags::Collection tags{"Implicit", "And explicit tags", 0, true, false, true, {}};
    fast_ber::Tags::Collection tags_copy{};

    fast_ber::EncodeResult encode_result = fast_ber::encode(absl::MakeSpan(buffer.data(), buffer.size()), tags);
    CHECK(encode_result.success);
    fast_ber::DecodeResult decode_result = fast_ber::decode(absl::MakeSpan(buffer.data(), buffer.size()), tags_copy);
    CHECK(decode_result.success);

    CHECK(tags_copy.string1 == "Implicit");
    CHECK(tags_copy.string2 == "And explicit tags");
    CHECK(tags_copy.integer == 0);
    CHECK(tags_copy.bool1);
    CHECK(!tags_copy.bool2);
    CHECK(tags_copy.bool3);
    CHECK(tags_copy.enumerated == decltype(fast_ber::Tags::Collection::enumerated){});

    CHECK(tags_copy.bool3);

    fast_ber::BerViewIterator itr = fast_ber::BerView(buffer).begin();
    CHECK(fast_ber::has_correct_header(*itr, fast_ber::Id<fast_ber::Class::context_specific, 0>{},
                                       fast_ber::Construction::primitive));
    ++itr;
    CHECK(fast_ber::has_correct_header(*itr, fast_ber::Id<fast_ber::Class::context_specific, 1>{},
                                       fast_ber::Construction::primitive));
    ++itr;
    CHECK(fast_ber::has_correct_header(*itr,
                                       fast_ber::DoubleId<fast_ber::Id<fast_ber::Class::context_specific, 2>,
                                                          fast_ber::ExplicitId<fast_ber::UniversalTag::integer>>{},
                                       fast_ber::Construction::primitive));
    ++itr;
    CHECK(fast_ber::has_correct_header(*itr, fast_ber::Id<fast_ber::Class::context_specific, 3>{},
                                       fast_ber::Construction::primitive));
    ++itr;
    CHECK(fast_ber::has_correct_header(*itr,
                                       fast_ber::DoubleId<fast_ber::Id<fast_ber::Class::private_, 4>,
                                                          fast_ber::ExplicitId<fast_ber::UniversalTag::boolean>>{},
                                       fast_ber::Construction::primitive));
    ++itr;
    CHECK(fast_ber::has_correct_header(*itr, fast_ber::Id<fast_ber::Class::application, 5>{},
                                       fast_ber::Construction::primitive));
    ++itr;

    // TODO: FIXME!
    //    CHECK(fast_ber::has_correct_header(*itr, fast_ber::Id<fast_ber::Class::application, 6>{},
    //                                     fast_ber::Construction::primitive));
}

TEST_CASE("Tags: Tagging an enum")
{
    /* TODO: FIXME!
    fast_ber::Tags::TaggedEnum a = fast_ber::Tags::TaggedEnum::Values::enum_;
    REQUIRE(fast_ber::Identifier<fast_ber::Tags::TaggedEnum>::tag() == 7);
    REQUIRE(a == fast_ber::Tags::TaggedEnum::Values::enum_);

    std::array<uint8_t, 5000> buffer        = {};
    fast_ber::EncodeResult    encode_result = fast_ber::encode(absl::MakeSpan(buffer.data(), buffer.size()), a);
    fast_ber::DecodeResult    decode_result = fast_ber::decode(absl::MakeSpan(buffer.data(), buffer.size()), a);

    REQUIRE(encode_result.success);
    REQUIRE(decode_result.success);
    */
}
