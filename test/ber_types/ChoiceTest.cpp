#include "autogen/choice.hpp"

#include "fast_ber/ber_types/Choice.hpp"
#include "fast_ber/ber_types/Identifier.hpp"
#include "fast_ber/ber_types/Integer.hpp"
#include "fast_ber/ber_types/OctetString.hpp"

#include <catch2/catch.hpp>

TEST_CASE("Choice: Check string choice matches simple string type")
{
    const auto choice = fast_ber::Choice<fast_ber::Integer, fast_ber::OctetString>("Test string");
    const auto tags   = fast_ber::make_choice_id(fast_ber::ExplicitIdentifier{fast_ber::UniversalTag::integer},
                                               fast_ber::ExplicitIdentifier{fast_ber::UniversalTag::octet_string});

    std::vector<uint8_t> choice_encoded(100);
    std::vector<uint8_t> string_encoded(100);

    size_t choice_encode_length =
        fast_ber::encode_with_specific_id(absl::MakeSpan(choice_encoded.data(), choice_encoded.size()), choice, tags)
            .length;
    size_t string_encoded_length =
        fast_ber::encode_with_specific_id(absl::MakeSpan(string_encoded.data(), string_encoded.size()), choice, tags)
            .length;

    choice_encoded.resize(choice_encode_length);
    string_encoded.resize(string_encoded_length);

    REQUIRE(choice_encoded == string_encoded);
}

TEST_CASE("Choice: Generated choice")
{
    fast_ber::MakeAChoice::Collection collection;
    collection.the_choice = fast_ber::Integer(5);

    std::vector<uint8_t> buffer(1000);
    size_t length = fast_ber::MakeAChoice::encode(absl::MakeSpan(buffer.data(), buffer.size()), collection).length;
    buffer.resize(length);

    fast_ber::MakeAChoice::Collection copy;
    bool success = fast_ber::MakeAChoice::decode(absl::MakeSpan(buffer.data(), buffer.size()), copy);

    REQUIRE(length > 0);
    REQUIRE(success);
    REQUIRE(copy.the_choice == collection.the_choice);
}
