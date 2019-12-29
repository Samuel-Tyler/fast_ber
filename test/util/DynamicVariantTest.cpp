#include "fast_ber/util/DynamicVariant.hpp"

#include <catch2/catch.hpp>

#include <string>

#if true
template <typename... Args>
using TestVariant = fast_ber::DynamicVariant<Args...>;
namespace ns      = fast_ber;
#else
template <typename... Args>
using TestVariant = absl::variant<Args...>;
namespace ns      = absl;
#endif

TEST_CASE("DynamicVariant: Construct")
{
    TestVariant<int, char, bool>       var_a;
    TestVariant<int, char, bool>       var_b(5);
    const TestVariant<int, char, bool> var_c('c');
    const TestVariant<int, char, bool> var_d(true);

    REQUIRE(var_a.index() == 0);
    REQUIRE(var_b.index() == 0);
    REQUIRE(var_c.index() == 1);
    REQUIRE(var_d.index() == 2);
}

TEST_CASE("DynamicVariant: Conversion Construct")
{
    const TestVariant<int, std::string> var_a = "pear";
    const TestVariant<int, std::string> var_b = short(-10);

    REQUIRE(ns::get<std::string>(var_a) == "pear");
    REQUIRE(ns::get<int>(var_b) == -10);
}

TEST_CASE("DynamicVariant: Conversion Assign")
{
    TestVariant<int, std::string> var_a;

    var_a = "pear";
    REQUIRE(ns::get<std::string>(var_a) == "pear");

    var_a = short(10);
    REQUIRE(ns::get<int>(var_a) == 10);
}

TEST_CASE("DynamicVariant: Equality")
{
    TestVariant<int, short, char, bool>       var_a(5);
    TestVariant<int, short, char, bool>       var_b(short(5));
    const TestVariant<int, short, char, bool> var_c('c');
    const TestVariant<int, short, char, bool> var_d(true);

    REQUIRE(var_a == var_a);
    REQUIRE(var_b == var_b);
    REQUIRE(var_c == var_c);
    REQUIRE(var_d == var_d);
    REQUIRE(var_a != var_b);
    REQUIRE(var_c != var_d);

    REQUIRE(!(var_a == var_b));
    REQUIRE(!(var_c == var_d));
    REQUIRE(!(var_a != var_a));
    REQUIRE(!(var_c != var_c));
}
TEST_CASE("DynamicVariant: Get")
{
    TestVariant<int, char, bool, std::string>       var_a(5);
    TestVariant<int, char, bool, std::string>       var_b('c');
    const TestVariant<int, char, bool, std::string> var_c(true);
    const TestVariant<int, char, bool, std::string> var_d(std::string("apple"));

    REQUIRE(ns::get<int>(var_a) == 5);
    REQUIRE(ns::get<char>(var_b) == 'c');
    REQUIRE(ns::get<bool>(var_c) == true);
    REQUIRE(ns::get<std::string>(var_d) == "apple");

    REQUIRE(ns::get<0>(var_a) == 5);
    REQUIRE(ns::get<1>(var_b) == 'c');
    REQUIRE(ns::get<2>(var_c) == true);
    REQUIRE(ns::get<3>(var_d) == "apple");
}

TEST_CASE("DynamicVariant: Get If")
{
    TestVariant<int, char, bool, std::string>       var_a(5);
    TestVariant<int, char, bool, std::string>       var_b('c');
    const TestVariant<int, char, bool, std::string> var_c(true);
    const TestVariant<int, char, bool, std::string> var_d(std::string("apple"));

    REQUIRE(*ns::get_if<int>(&var_a) == 5);
    REQUIRE(*ns::get_if<char>(&var_b) == 'c');
    REQUIRE(*ns::get_if<bool>(&var_c) == true);
    REQUIRE(*ns::get_if<std::string>(&var_d) == "apple");

    REQUIRE(ns::get_if<std::string>(&var_a) == nullptr);
    REQUIRE(ns::get_if<bool>(&var_b) == nullptr);
    REQUIRE(ns::get_if<char>(&var_c) == nullptr);
    REQUIRE(ns::get_if<int>(&var_d) == nullptr);

    REQUIRE(*ns::get_if<0>(&var_a) == 5);
    REQUIRE(*ns::get_if<1>(&var_b) == 'c');
    REQUIRE(*ns::get_if<2>(&var_c) == true);
    REQUIRE(*ns::get_if<3>(&var_d) == "apple");

    REQUIRE(ns::get_if<3>(&var_a) == nullptr);
    REQUIRE(ns::get_if<2>(&var_b) == nullptr);
    REQUIRE(ns::get_if<1>(&var_c) == nullptr);
    REQUIRE(ns::get_if<0>(&var_d) == nullptr);
}

TEST_CASE("DynamicVariant: Copy")
{
    TestVariant<int, char, bool, std::string>       var_a(5);
    TestVariant<int, char, bool, std::string>       var_b('c');
    const TestVariant<int, char, bool, std::string> var_c(var_b);

    var_b = var_a;
    REQUIRE(ns::get<int>(var_a) == 5);
    REQUIRE(ns::get<int>(var_b) == 5);
    REQUIRE(ns::get<char>(var_c) == 'c');
}

TEST_CASE("DynamicVariant: Move")
{
    TestVariant<int, char, bool, std::string> var_a(5);
    TestVariant<int, char, bool, std::string> var_b('c');
    TestVariant<int, char, bool, std::string> var_c(std::move(var_b));

    var_b = std::move(var_a);
    REQUIRE(ns::get<int>(var_b) == 5);
    REQUIRE(ns::get<char>(var_c) == 'c');
}

TEST_CASE("DynamicVariant: emplace")
{
    TestVariant<int, char, bool, std::string> var_a(5);

    REQUIRE(var_a.emplace<int>(10) == 10);
    REQUIRE(ns::get<int>(var_a) == 10);

    REQUIRE(var_a.emplace<char>('$') == '$');
    REQUIRE(ns::get<char>(var_a) == '$');

    REQUIRE(var_a.emplace<bool>(false) == false);
    REQUIRE(ns::get<bool>(var_a) == false);

    REQUIRE(var_a.emplace<0>(10) == 10);
    REQUIRE(ns::get<int>(var_a) == 10);

    REQUIRE(var_a.emplace<1>('$') == '$');
    REQUIRE(ns::get<char>(var_a) == '$');

    REQUIRE(var_a.emplace<2>(false) == false);
    REQUIRE(ns::get<bool>(var_a) == false);
}

struct Square
{
    template <typename T>
    long operator()(T t)
    {
        return t * t;
    }
};

TEST_CASE("DynamicVariant: Visit")
{
    TestVariant<long, short> var_a(long(5));
    TestVariant<long, short> var_b(long(-10));

    REQUIRE(ns::visit(Square(), var_a) == 25);
    REQUIRE(ns::visit(Square(), var_b) == 100);
}
