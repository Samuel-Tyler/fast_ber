#include "autogen/top_level_encodings.hpp"

#include "catch2/catch.hpp"

#include <vector>

TEST_CASE("Different Top Level Types: Integer as top level type")
{
    std::array<uint8_t, 5000> buffer     = {};
    fast_ber::TopLevel::MyInt my_int     = 20;
    fast_ber::TopLevel::MyInt my_new_int = -20;

    my_int = 50;
    REQUIRE(my_int == 50);

    fast_ber::encode(absl::MakeSpan(buffer.data(), buffer.size()), my_int);
    fast_ber::decode(absl::MakeSpan(buffer.data(), buffer.size()), my_new_int);

    REQUIRE(my_int == my_new_int);
}

TEST_CASE("Different Top Level Types: String as top level type")
{
    std::array<uint8_t, 5000>    buffer        = {};
    fast_ber::TopLevel::MyString my_string     = "<%%%%>";
    fast_ber::TopLevel::MyString my_new_string = "fail";

    fast_ber::encode(absl::MakeSpan(buffer.data(), buffer.size()), my_string);
    fast_ber::decode(absl::MakeSpan(buffer.data(), buffer.size()), my_new_string);

    REQUIRE(my_string == my_new_string);
}

TEST_CASE("Different Top Level Types: Choice as top level type")
{
    using StringType  = fast_ber::OctetString<fast_ber::Id<fast_ber::Class::context_specific, 0>>;
    using IntegerType = fast_ber::Integer<fast_ber::Id<fast_ber::Class::context_specific, 1>>;

    std::array<uint8_t, 5000>    buffer        = {};
    fast_ber::TopLevel::MyChoice my_string     = fast_ber::OctetString<>("The String");
    fast_ber::TopLevel::MyChoice my_new_string = 500;

    REQUIRE(fast_ber::get<StringType>(my_string) == "The String");
    REQUIRE(fast_ber::get<IntegerType>(my_new_string) == 500);

    fast_ber::encode(absl::MakeSpan(buffer.data(), buffer.size()), my_string);
    fast_ber::decode(absl::MakeSpan(buffer.data(), buffer.size()), my_new_string);

    REQUIRE(fast_ber::get<StringType>(my_string) == "The String");
    REQUIRE(fast_ber::get<StringType>(my_new_string) == "The String");
}

TEST_CASE("Different Top Level Types: Collection")
{
    std::array<uint8_t, 5000>      buffer = {};
    fast_ber::TopLevel::Collection my_collection;
    fast_ber::TopLevel::Collection my_new_collection;

    my_collection.my_int    = 50;
    my_collection.my_string = "hello!";

    fast_ber::encode(absl::MakeSpan(buffer.data(), buffer.size()), my_collection);
    fast_ber::decode(absl::MakeSpan(buffer.data(), buffer.size()), my_new_collection);

    REQUIRE(my_collection.my_int == fast_ber::Integer<>{50});
    REQUIRE(my_collection.my_string == fast_ber::OctetString<>{"hello!"});
}
