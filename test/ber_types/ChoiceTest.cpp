#include "autogen/choice.hpp"

#include "fast_ber/ber_types/Choice.hpp"
#include "fast_ber/ber_types/Identifier.hpp"
#include "fast_ber/ber_types/Integer.hpp"
#include "fast_ber/ber_types/OctetString.hpp"

#include <catch2/catch.hpp>

TEST_CASE("Choice: Check string choice matches simple string type")
{
    const auto choice = fast_ber::Choice<fast_ber::Integer<>, fast_ber::OctetString<>>("Test string");

    std::vector<uint8_t> choice_encoded(100, 0x00);
    std::vector<uint8_t> string_encoded(100, 0x00);

    size_t choice_encode_length =
        fast_ber::encode(absl::MakeSpan(choice_encoded.data(), choice_encoded.size()), choice).length;
    size_t string_encoded_length =
        fast_ber::encode(absl::MakeSpan(string_encoded.data(), string_encoded.size()), choice).length;

    choice_encoded.resize(choice_encode_length);
    string_encoded.resize(string_encoded_length);

    REQUIRE(choice_encoded == string_encoded);
}

TEST_CASE("Choice: Basic choice")
{
    fast_ber::Choice<fast_ber::Integer<>, fast_ber::OctetString<>> choice_1;
    fast_ber::Choice<fast_ber::Integer<>, fast_ber::OctetString<>> choice_2;

    choice_1 = "Test string";
    choice_2 = 10;

    REQUIRE(absl::holds_alternative<fast_ber::OctetString<>>(choice_1));
    REQUIRE(absl::holds_alternative<fast_ber::Integer<>>(choice_2));

    std::vector<uint8_t> choice_encoded(100, 0x00);
    bool enc_success = fast_ber::encode(absl::MakeSpan(choice_encoded.data(), choice_encoded.size()), choice_1).success;
    bool dec_success = fast_ber::decode(absl::MakeSpan(choice_encoded.data(), choice_encoded.size()), choice_2).success;

    REQUIRE(enc_success);
    REQUIRE(dec_success);
    REQUIRE(absl::holds_alternative<fast_ber::OctetString<>>(choice_1));
    REQUIRE(absl::holds_alternative<fast_ber::OctetString<>>(choice_2));
    REQUIRE(choice_1 == choice_2);
}

TEST_CASE("Choice: Clashing type")
{
    using choice_type =
        fast_ber::Choice<fast_ber::Integer<>,
                         fast_ber::TaggedType<fast_ber::OctetString<>,
                                              fast_ber::ImplicitIdentifier<fast_ber::Class::context_specific, 99>>,
                         fast_ber::TaggedType<fast_ber::OctetString<>,
                                              fast_ber::ImplicitIdentifier<fast_ber::Class::context_specific, 100>>>;
    auto choice_1 = choice_type(
        fast_ber::TaggedType<fast_ber::OctetString<>,
                             fast_ber::ImplicitIdentifier<fast_ber::Class::context_specific, 100>>("Test string"));
    auto choice_2 = choice_type();

    std::vector<uint8_t> choice_encoded(100, 0x00);

    bool enc_success = fast_ber::encode(absl::MakeSpan(choice_encoded.data(), choice_encoded.size()), choice_1).success;
    bool dec_success = fast_ber::decode(absl::MakeSpan(choice_encoded.data(), choice_encoded.size()), choice_2).success;

    REQUIRE(enc_success);
    REQUIRE(dec_success);
    REQUIRE(absl::holds_alternative<fast_ber::variant_alternative<2, choice_type>::type>(choice_1));
    REQUIRE(absl::holds_alternative<fast_ber::variant_alternative<2, choice_type>::type>(choice_2));
    REQUIRE(choice_1 == choice_2);
}

TEST_CASE("Choice: Generated choice")
{
    fast_ber::MakeAChoice::Collection collection;
    collection.the_choice = fast_ber::Integer<fast_ber::ImplicitIdentifier<fast_ber::Class::context_specific, 2>>(5);

    std::vector<uint8_t> buffer(1000, 0x00);
    size_t               length = fast_ber::encode(absl::MakeSpan(buffer.data(), buffer.size()), collection).length;
    buffer.resize(length);

    fast_ber::MakeAChoice::Collection copy;
    bool success = fast_ber::decode(absl::MakeSpan(buffer.data(), buffer.size()), copy).success;

    REQUIRE(length > 0);
    REQUIRE(success);
    REQUIRE(copy.the_choice == collection.the_choice);
}
