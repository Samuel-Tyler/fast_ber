#include "fast_ber/util/BerContainer.hpp"

#include <catch2/catch.hpp>

#include <array>

TEST_CASE("BerContainer: Empty construction")
{
    fast_ber::BerContainer container;
    const auto             expected = std::initializer_list<uint8_t>({0x80, 0x00});

    REQUIRE(container.is_valid());
    REQUIRE(container.tag() == 0);
    REQUIRE(container.content_length() == 0);
    REQUIRE(container.ber() == absl::MakeSpan(expected.begin(), expected.size()));
}

TEST_CASE("BerContainer: Resize larger")
{
    const auto             test_data = std::initializer_list<uint8_t>({'a', 'b', 'c'});
    fast_ber::BerContainer container(test_data, fast_ber::ConstructionMethod::construct_with_provided_content);

    size_t new_size = 99999;

    REQUIRE(container.is_valid());
    REQUIRE(container.tag() == 0);
    REQUIRE(container.content_length() == test_data.size());
    REQUIRE(container.content() == test_data);

    container.resize_content(new_size);

    REQUIRE(container.is_valid());
    REQUIRE(container.tag() == 0);
    REQUIRE(container.content_length() == new_size);
    REQUIRE(absl::MakeSpan(container.content_data(), test_data.size()) == test_data);
}

TEST_CASE("BerContainer: Resize smaller")
{
    const auto             test_data = std::initializer_list<uint8_t>({'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i'});
    fast_ber::BerContainer container(test_data, fast_ber::ConstructionMethod::construct_with_provided_content);

    size_t new_size = 5;

    REQUIRE(container.is_valid());
    REQUIRE(container.tag() == 0);
    REQUIRE(container.content_length() == test_data.size());
    REQUIRE(container.content() == test_data);

    container.resize_content(new_size);

    REQUIRE(container.is_valid());
    REQUIRE(container.tag() == 0);
    REQUIRE(container.content_length() == new_size);
    REQUIRE(container.content() == absl::MakeSpan(test_data.begin(), new_size));
}

TEST_CASE("BerContainer: Resize same size")
{
    const auto             test_data = std::initializer_list<uint8_t>({'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i'});
    fast_ber::BerContainer container(test_data, fast_ber::ConstructionMethod::construct_with_provided_content);

    size_t new_size = test_data.size();

    REQUIRE(container.is_valid());
    REQUIRE(container.tag() == 0);
    REQUIRE(container.content_length() == test_data.size());
    REQUIRE(container.content() == test_data);

    container.resize_content(new_size);

    REQUIRE(container.is_valid());
    REQUIRE(container.tag() == 0);
    REQUIRE(container.content_length() == new_size);
    REQUIRE(container.content() == test_data);
}
