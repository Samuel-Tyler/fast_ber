#include "autogen/simple.hpp"

#include "catch2/catch.hpp"

#include <fstream>
#include <vector>

TEST_CASE("Testing a generated ber container")
{
    std::array<uint8_t, 5000> buffer;
    std::ofstream             output("test_out.ber");

    const std::string hello   = "Hello world!";
    const std::string goodbye = "Good bye world!";

    fast_ber::Simple::Collection new_collection;
    bool                         success     = false;
    size_t                       encode_size = 0;
    fast_ber::Simple::Collection collection;

    collection.hello                 = hello;
    collection.goodbye               = goodbye;
    collection.integer               = 5;
    collection.boolean               = true;
    collection.child.meaning_of_life = -42;
    collection.optional_child        = fast_ber::Simple::Child{999999999, {"The", "second", "child"}};

    //  for (long i = 0; i < 100000000; i++)
    {
        encode_size = fast_ber::Simple::encode_with_new_id(absl::MakeSpan(buffer.data(), buffer.size()), collection,
                                                           fast_ber::Class::context_specific, 0)
                          .encode_length;
    }
    std::cout << "encoding done\n";

    // for (long i = 0; i < 100000000; i++)
    {
        success = fast_ber::Simple::decode(absl::MakeSpan(buffer.data(), buffer.size()), new_collection, 0);
    }

    output.write(reinterpret_cast<const char*>(buffer.data()), (long)encode_size);

    REQUIRE(success);
    REQUIRE(new_collection.hello == "Hello world!");
    REQUIRE(new_collection.goodbye == "Good bye world!");
    REQUIRE(new_collection.integer == 5);
    REQUIRE(new_collection.boolean == true);
    REQUIRE(new_collection.child.meaning_of_life.has_value());
    REQUIRE(new_collection.optional_child.has_value());
    REQUIRE(new_collection.optional_child->meaning_of_life.has_value());
    REQUIRE(*new_collection.child.meaning_of_life == -42);
    REQUIRE(new_collection.child.list.size() == 0);
    REQUIRE(new_collection.optional_child->list ==
            fast_ber::SequenceOf<fast_ber::OctetString>{"The", "second", "child"});
    REQUIRE(*new_collection.optional_child->meaning_of_life == 999999999);
}
