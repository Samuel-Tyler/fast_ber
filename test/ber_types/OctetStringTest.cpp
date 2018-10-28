#include "fast_ber/ber_types/OctetString.hpp"

#include <catch2/catch.hpp>

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
        "-------------------------------------------------------------------------------------------------------------"
        ""};

    for (const auto& val : test_vals)
    {
        fast_ber::OctetString str(val);
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
        "-------------------------------------------------------------------------------------------------------------"
        ""};

    for (const auto& val : test_vals)
    {
        fast_ber::OctetString str(val);
        REQUIRE(str.value() == val);
    }
}
