#include "fast_ber/ber_types/Integer.hpp"

#include <catch2/catch.hpp>

TEST_CASE ("Integer: Construction from zero")
{
    fast_ber::Integer i(0);
    REQUIRE(i.value() == 0);
}

TEST_CASE ("Integer: Construction from int")
{
    fast_ber::Integer i(999);
    REQUIRE(i.value() == 999);
}

TEST_CASE ("Integer: Construction from negative int")
{
    fast_ber::Integer i(-999);
    REQUIRE(i.value() == 999);
}

TEST_CASE ("Integer: Construction from large int")
{
    fast_ber::Integer i(999999999999);
    REQUIRE(i.value() == 999999999999);
}

TEST_CASE ("Integer: Construction from binary data")
{

}
