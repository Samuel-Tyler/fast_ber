#include "autogen/tags.hpp"

#include "fast_ber/util/BerView.hpp"

#include "catch2/catch.hpp"

#include <vector>

TEST_CASE("Tags: Encoding a packet with various tagging modes")
{
    std::array<uint8_t, 5000>    buffer = {};
    fast_ber::Tags::Collection<> tags   = {};

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

    std::cout << fast_ber::Identifier<decltype(fast_ber::Tags::DefaultCollection<>::string1)>{} << std::endl;
    REQUIRE(std::is_same<fast_ber::Identifier<decltype(fast_ber::Tags::DefaultCollection<>::string1)>,
                         fast_ber::ExplicitId<fast_ber::UniversalTag::octet_string>>::value);
    REQUIRE(std::is_same<fast_ber::Identifier<decltype(fast_ber::Tags::DefaultCollection<>::string2)>,
                         fast_ber::ExplicitId<fast_ber::UniversalTag::octet_string>>::value);
    REQUIRE(std::is_same<fast_ber::Identifier<decltype(fast_ber::Tags::DefaultCollection<>::string3)>,
                         fast_ber::ExplicitId<fast_ber::UniversalTag::octet_string>>::value);

    REQUIRE(std::is_same<fast_ber::Identifier<decltype(fast_ber::AutomaticTags::DefaultCollection<>::string1)>,
                         fast_ber::Id<fast_ber::Class::context_specific, 0>>::value);
    REQUIRE(std::is_same<fast_ber::Identifier<decltype(fast_ber::AutomaticTags::DefaultCollection<>::string2)>,
                         fast_ber::Id<fast_ber::Class::context_specific, 1>>::value);
    REQUIRE(std::is_same<fast_ber::Identifier<decltype(fast_ber::AutomaticTags::DefaultCollection<>::string3)>,
                         fast_ber::Id<fast_ber::Class::context_specific, 2>>::value);
}

TEST_CASE("Tags: Encoding and decoding a packet with various tagging modes")
{
    std::array<uint8_t, 5000> buffer = {};

    fast_ber::Tags::Collection<> tags{"Implicit", "And explicit tags", 0, true, false, true, {}};
    fast_ber::Tags::Collection<> tags_copy{};

    fast_ber::EncodeResult encode_result = fast_ber::encode(absl::MakeSpan(buffer.data(), buffer.size()), tags);
    REQUIRE(encode_result.success);
    fast_ber::DecodeResult decode_result = fast_ber::decode(absl::MakeSpan(buffer.data(), buffer.size()), tags_copy);
    REQUIRE(decode_result.success);

    REQUIRE(tags_copy.string1 == "Implicit");
    REQUIRE(tags_copy.string2 == "And explicit tags");
    REQUIRE(tags_copy.integer == 0);
    REQUIRE(tags_copy.bool1);
    REQUIRE(!tags_copy.bool2);
    REQUIRE(tags_copy.bool3);
    REQUIRE(tags_copy.enumerated == decltype(fast_ber::Tags::Collection<>::enumerated){});

    REQUIRE(tags_copy.bool3);

    REQUIRE(std::is_same<fast_ber::Identifier<decltype(tags_copy.string2)>,
                         fast_ber::Id<fast_ber::Class::context_specific, 1>>::value);
    REQUIRE(std::is_same<fast_ber::Identifier<decltype(tags_copy.integer)>,
                         fast_ber::DoubleId<fast_ber::Id<fast_ber::Class::context_specific, 2>,
                                            fast_ber::ExplicitId<fast_ber::UniversalTag::integer>>>::value);
    REQUIRE(std::is_same<fast_ber::Identifier<decltype(tags_copy.bool1)>,
                         fast_ber::Id<fast_ber::Class::context_specific, 3>>::value);
    REQUIRE(std::is_same<fast_ber::Identifier<decltype(tags_copy.bool2)>,
                         fast_ber::DoubleId<fast_ber::Id<fast_ber::Class::private_, 4>,
                                            fast_ber::ExplicitId<fast_ber::UniversalTag::boolean>>>::value);
    REQUIRE(std::is_same<fast_ber::Identifier<decltype(tags_copy.bool3)>,
                         fast_ber::Id<fast_ber::Class::application, 5>>::value);
    REQUIRE(std::is_same<fast_ber::Identifier<decltype(tags_copy.enumerated)>,
                         fast_ber::Id<fast_ber::Class::application, 6>>::value);
}

TEST_CASE("Tags: Tagging an enum")
{
    fast_ber::Tags::TaggedEnum<> a = fast_ber::Tags::TaggedEnum<>::BaseType::Values::enum_;
    REQUIRE(fast_ber::Identifier<fast_ber::Tags::TaggedEnum<>>::tag() == 7);
    REQUIRE(a == fast_ber::Tags::TaggedEnum<>::BaseType::Values::enum_);

    std::array<uint8_t, 5000> buffer        = {};
    fast_ber::EncodeResult    encode_result = fast_ber::encode(absl::MakeSpan(buffer.data(), buffer.size()), a);
    fast_ber::DecodeResult    decode_result = fast_ber::decode(absl::MakeSpan(buffer.data(), buffer.size()), a);

    REQUIRE(encode_result.success);
    REQUIRE(decode_result.success);
}
