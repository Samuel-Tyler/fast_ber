#include "autogen/simple.hpp"

#include "catch2/catch.hpp"

#include <vector>

TEST_CASE("SimpleCompilerOutput: Testing a generated ber container")
{
    std::array<uint8_t, 5000> buffer = {};

    const std::string hello   = "Hello world!";
    const std::string goodbye = "Good bye world!";

    fast_ber::Simple::Collection<> new_collection;
    fast_ber::Simple::Collection<> collection;

    collection.hello                 = hello;
    collection.goodbye               = goodbye;
    collection.integer               = 5;
    collection.boolean               = true;
    collection.child.meaning_of_life = -42;
    collection.optional_child        = fast_ber::Simple::Child<fast_ber::Id<fast_ber::Class::context_specific, 5>>{
        999999999, {"The", "second", "child", std::string(2000, 'x')}};
    collection.the_choice = decltype(collection.the_choice){absl::in_place_index_t<1>(), "I chose a string!"};

    fast_ber::EncodeResult encode_result = fast_ber::encode(absl::MakeSpan(buffer.data(), buffer.size()), collection);
    fast_ber::DecodeResult decode_result =
        fast_ber::decode(absl::MakeSpan(buffer.data(), buffer.size()), new_collection);

    REQUIRE(encode_result.success);
    REQUIRE(decode_result.success);
    REQUIRE(new_collection.hello == "Hello world!");
    REQUIRE(new_collection.goodbye == "Good bye world!");
    REQUIRE(new_collection.integer == 5);
    REQUIRE(new_collection.boolean == true);
    REQUIRE(new_collection.child.meaning_of_life.has_value());
    REQUIRE(new_collection.optional_child.has_value());
    REQUIRE(new_collection.optional_child->meaning_of_life.has_value());
    REQUIRE(*new_collection.child.meaning_of_life == -42);
    REQUIRE(new_collection.child.list.empty());
    REQUIRE(new_collection.optional_child->list ==
            fast_ber::SequenceOf<fast_ber::OctetString<>>{"The", "second", "child", std::string(2000, 'x')});
    REQUIRE(*new_collection.optional_child->meaning_of_life == 999999999);
    REQUIRE(absl::get<1>(new_collection.the_choice) == "I chose a string!");

    fast_ber::BerView view = absl::Span<const uint8_t>(buffer);
    REQUIRE(std::distance(view.begin(), view.end()) == 7);

    std::cout << "fast_ber::Simple::Collection: " << collection << std::endl;
}
