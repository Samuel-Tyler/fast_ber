#include "fast_ber/ber_types/Identifier.hpp"
#include "fast_ber/ber_types/OctetString.hpp"
#include "fast_ber/util/EncodeHelpers.hpp"

#include <catch2/catch.hpp>

const static std::initializer_list<uint8_t> hello_world_packet = {0x04, 0x0B, 'H', 'e', 'l', 'l', 'o',
                                                                  ' ',  'w',  'o', 'r', 'l', 'd'};

TEST_CASE("OctetString: Construction from string")
{
    std::initializer_list<std::string> test_vals{
        "",
        "a",
        "A",
        "ABCD",
        "123456",
        "ZZZZZZZZ",
        "!$@",
        "HELLO!",
        "LONG STRING "
        "--------------------------------------------------------------------------------------------------------------"
        "--------------------------------------------------------------------------------------------------------------"
        "-------------------------------------------------------------------------------------------------------------",
        std::string(2000, '5')};

    for (const auto& val : test_vals)
    {
        fast_ber::OctetString<> str(val);
        REQUIRE(str.length() == str.size());
        REQUIRE(str.value() == val);
    }
}

TEST_CASE("OctetString: Construction from const char*")
{
    std::initializer_list<const char*> test_vals{
        "",
        "a",
        "A",
        "ABCD",
        "123456",
        "ZZZZZZZZ",
        "!$@",
        "HELLO!",
        "LONG STRING "
        "--------------------------------------------------------------------------------------------------------------"
        "--------------------------------------------------------------------------------------------------------------"
        "------------------------------------------------------------------------------------------------------------"
        "-"};

    for (const auto& val : test_vals)
    {
        fast_ber::OctetString<> str(val);
        REQUIRE(str.value() == val);
    }
}

TEST_CASE("OctetString: Construction from sample packet")
{
    fast_ber::OctetString<> octet_string;
    octet_string.assign_ber(hello_world_packet);
    REQUIRE(octet_string.value() == "Hello world");
}

TEST_CASE("OctetString: Encode to buffer")
{
    std::array<uint8_t, 100> buffer = {};

    fast_ber::OctetString<> octet_string(std::string("Hello world"));
    size_t encoded_length = fast_ber::encode(absl::MakeSpan(buffer.data(), buffer.size()), octet_string,
                                             fast_ber::ExplicitIdentifier<fast_ber::UniversalTag::octet_string>{})
                                .length;
    REQUIRE(absl::MakeSpan(buffer.data(), encoded_length) == hello_world_packet);
}

TEST_CASE("OctetString: Destructive encode to buffer")
{
    std::array<uint8_t, 200> buffer = {};

    fast_ber::OctetString<> octet_string(std::string(150, 'c'));
    size_t encoded_length = fast_ber::encode(absl::MakeSpan(buffer.data(), buffer.size()), octet_string,
                                             fast_ber::ExplicitIdentifier<fast_ber::UniversalTag::octet_string>{})
                                .length;

    for (size_t i = 0; i < encoded_length; i++)
    {
        bool success = fast_ber::encode(absl::MakeSpan(buffer.data(), i), octet_string,
                                        fast_ber::ExplicitIdentifier<fast_ber::UniversalTag::octet_string>{})
                           .success;
        REQUIRE(!success);
    }
}

TEST_CASE("OctetString: Iterators")
{
    const std::string       test_string     = "deliver no evil";
    const std::string       reversed_string = "live on reviled";
    fast_ber::OctetString<> octet_string(test_string);

    std::reverse(octet_string.begin(), octet_string.end());
    REQUIRE(octet_string.value() == reversed_string);
}

TEST_CASE("OctetString: Assign")
{
    const auto                     test_data      = std::string(9999, 'c');
    const fast_ber::OctetString<>  octet_string_1 = fast_ber::OctetString<>(test_data);
    const fast_ber::OctetString<>& octet_string_2(octet_string_1);
    const fast_ber::OctetString<>  octet_string_3(test_data);
    const fast_ber::OctetString<>  octet_string_4 = test_data;
    fast_ber::OctetString<>        octet_string_5;

    octet_string_5 = octet_string_4;

    REQUIRE(octet_string_1.value() == test_data);
    REQUIRE(octet_string_2.value() == test_data);
    REQUIRE(octet_string_3.value() == test_data);
    REQUIRE(octet_string_4.value() == test_data);
    REQUIRE(octet_string_5.value() == test_data);
}

TEST_CASE("OctetString: StringView")
{
    const auto                    test = "StringView test";
    const fast_ber::OctetString<> octet_string(test);

    REQUIRE(absl::string_view(octet_string.c_str(), octet_string.length()) == test);
}

TEST_CASE("OctetString: Equality")
{
    fast_ber::OctetString<> test_octets = "Duck";
    REQUIRE(test_octets == "Duck");
    REQUIRE(test_octets != "Quack");
}

TEST_CASE("OctetString: Default value") { REQUIRE(fast_ber::OctetString<>{} == ""); }
