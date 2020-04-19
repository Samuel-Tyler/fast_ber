#include "fast_ber/ber_types/Choice.hpp"
#include "fast_ber/ber_types/Identifier.hpp"
#include "fast_ber/ber_types/Integer.hpp"
#include "fast_ber/ber_types/OctetString.hpp"

#include <catch2/catch.hpp>

TEST_CASE("Choice: Check string choice matches simple string type")
{
    const auto choice =
        fast_ber::Choice<fast_ber::Choices<fast_ber::Integer<>, fast_ber::OctetString<>>>("Test string");

    std::vector<uint8_t> choice_encoded(100, 0x00);
    std::vector<uint8_t> string_encoded(100, 0x00);

    size_t choice_encode_length =
        fast_ber::encode(absl::MakeSpan(choice_encoded.data(), choice_encoded.size()), choice).length;
    size_t string_encoded_length =
        fast_ber::encode(absl::MakeSpan(string_encoded.data(), string_encoded.size()), choice).length;

    choice_encoded.resize(choice_encode_length);
    string_encoded.resize(string_encoded_length);

    CHECK(choice_encoded == string_encoded);
}

TEST_CASE("Choice: Encode decode")
{
    using Identifier = fast_ber::DoubleId<fast_ber::Id<fast_ber::Class::context_specific, 2>,
                                          fast_ber::ExplicitId<fast_ber::UniversalTag::integer>>;
    fast_ber::Choice<fast_ber::Choices<fast_ber::Integer<>, fast_ber::OctetString<>>, Identifier> choice("Test string");

    std::vector<uint8_t> buffer(100, 0x00);

    fast_ber::EncodeResult encode_result = fast_ber::encode(absl::MakeSpan(buffer.data(), buffer.size()), choice);
    fast_ber::DecodeResult decode_result = fast_ber::decode(absl::MakeSpan(buffer.data(), buffer.size()), choice);

    CHECK(encode_result.success);
    CHECK(decode_result.success);
    CHECK(fast_ber::has_correct_header(fast_ber::BerView(buffer), Identifier{}, fast_ber::Construction::constructed));
}

TEST_CASE("Choice: Basic choice")
{
    fast_ber::Choice<fast_ber::Choices<fast_ber::Integer<>, fast_ber::OctetString<>>> choice_1;
    fast_ber::Choice<fast_ber::Choices<fast_ber::Integer<>, fast_ber::OctetString<>>> choice_2;

    choice_1 = "Test string";
    choice_2 = 10;

    CHECK(fast_ber::holds_alternative<fast_ber::OctetString<>>(choice_1));
    CHECK(fast_ber::holds_alternative<fast_ber::Integer<>>(choice_2));

    std::vector<uint8_t> choice_encoded(100, 0x00);
    bool enc_success = fast_ber::encode(absl::MakeSpan(choice_encoded.data(), choice_encoded.size()), choice_1).success;
    bool dec_success = fast_ber::decode(absl::MakeSpan(choice_encoded.data(), choice_encoded.size()), choice_2).success;

    CHECK(enc_success);
    CHECK(dec_success);
    CHECK(fast_ber::holds_alternative<fast_ber::OctetString<>>(choice_1));
    CHECK(fast_ber::holds_alternative<fast_ber::OctetString<>>(choice_2));
    CHECK(choice_1 == choice_2);
}

TEST_CASE("Choice: Clashing type")
{
    using choice_type = fast_ber::Choice<fast_ber::Choices<
        fast_ber::Integer<>, fast_ber::OctetString<fast_ber::Id<fast_ber::Class::context_specific, 99>>,
        fast_ber::OctetString<fast_ber::Id<fast_ber::Class::context_specific, 100>>>>;
    auto choice_1 =
        choice_type(fast_ber::OctetString<fast_ber::Id<fast_ber::Class::context_specific, 100>>("Test string"));
    auto choice_2 = choice_type();

    std::vector<uint8_t> choice_encoded(100, 0x00);

    bool enc_success = fast_ber::encode(absl::MakeSpan(choice_encoded.data(), choice_encoded.size()), choice_1).success;
    bool dec_success = fast_ber::decode(absl::MakeSpan(choice_encoded.data(), choice_encoded.size()), choice_2).success;

    CHECK(enc_success);
    CHECK(dec_success);
    CHECK(fast_ber::holds_alternative<fast_ber::variant_alternative<2, choice_type>::type>(choice_1));
    CHECK(fast_ber::holds_alternative<fast_ber::variant_alternative<2, choice_type>::type>(choice_2));
    CHECK(choice_1 == choice_2);
}

TEST_CASE("Choice: Choice of choices")
{
    using Choice1 =
        fast_ber::Choice<fast_ber::Choices<fast_ber::OctetString<fast_ber::Id<fast_ber::Class::context_specific, 1>>,
                                           fast_ber::OctetString<fast_ber::Id<fast_ber::Class::context_specific, 2>>>>;
    using Choice2 =
        fast_ber::Choice<fast_ber::Choices<fast_ber::OctetString<fast_ber::Id<fast_ber::Class::context_specific, 3>>,
                                           fast_ber::OctetString<fast_ber::Id<fast_ber::Class::context_specific, 4>>>>;
    using Choice3 = fast_ber::Choice<fast_ber::Choices<Choice1, Choice2>>;

    using ExpectedId = fast_ber::ChoiceId<fast_ber::ChoiceId<fast_ber::Id<fast_ber::Class::context_specific, 1>,
                                                             fast_ber::Id<fast_ber::Class::context_specific, 2>>,
                                          fast_ber::ChoiceId<fast_ber::Id<fast_ber::Class::context_specific, 3>,
                                                             fast_ber::Id<fast_ber::Class::context_specific, 4>>>;

    Choice3 choice_orig = fast_ber::OctetString<fast_ber::Id<fast_ber::Class::context_specific, 4>>{};
    Choice3 choice_copy = fast_ber::OctetString<fast_ber::Id<fast_ber::Class::context_specific, 1>>{};

    CHECK(fast_ber::holds_alternative<Choice2>(choice_orig));
    CHECK(fast_ber::holds_alternative<Choice1>(choice_copy));

    std::vector<uint8_t> choice_encoded(100, 0x00);

    bool enc_success =
        fast_ber::encode(absl::MakeSpan(choice_encoded.data(), choice_encoded.size()), choice_orig).success;
    bool dec_success =
        fast_ber::decode(absl::MakeSpan(choice_encoded.data(), choice_encoded.size()), choice_copy).success;

    CHECK(enc_success);
    CHECK(dec_success);
    CHECK(fast_ber::holds_alternative<Choice2>(choice_orig));
    CHECK(fast_ber::holds_alternative<Choice2>(choice_copy));
    CHECK(choice_orig == choice_copy);
    CHECK(std::is_same<fast_ber::Identifier<Choice3>, ExpectedId>::value);
}
