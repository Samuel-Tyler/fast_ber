#include "autogen/choice.hpp"

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

TEST_CASE("Choice: Generated choice")
{
    fast_ber::MakeAChoice::Collection<> collection;
    collection.the_choice = fast_ber::Integer<fast_ber::Id<fast_ber::Class::context_specific, 2>>(5);

    std::vector<uint8_t> buffer;
    size_t               expected_length = fast_ber::encoded_length(collection);
    buffer.resize(expected_length);
    size_t length = fast_ber::encode(absl::MakeSpan(buffer.data(), buffer.size()), collection).length;

    fast_ber::MakeAChoice::Collection<> copy;
    bool success = fast_ber::decode(absl::MakeSpan(buffer.data(), buffer.size()), copy).success;

    CHECK(length > 0);
    CHECK(length == expected_length);

    CHECK(success);
    CHECK(copy.the_choice == collection.the_choice);
}

TEST_CASE("Choice: Generated choice explicit tags")
{
    fast_ber::ExplicitChoice::MyChoice<> choice;
    choice = fast_ber::ExplicitChoice::MySequence<>{};

    std::vector<uint8_t> buffer(1000, 0x00);
    size_t               expected_length = fast_ber::encoded_length(choice);
    buffer.resize(expected_length);
    size_t length = fast_ber::encode(absl::MakeSpan(buffer.data(), buffer.size()), choice).length;

    fast_ber::ExplicitChoice::MyChoice<> copy;
    bool success = fast_ber::decode(absl::MakeSpan(buffer.data(), buffer.size()), copy).success;

    CHECK(length > 0);
    CHECK(length == expected_length);
    CHECK(success);
    CHECK(copy == choice);
}

TEST_CASE("Choice: Type deduction")
{
    using TestChoices   = fast_ber::Choices<fast_ber::Integer<fast_ber::Id<fast_ber::Class::context_specific, 1>>,
                                          fast_ber::OctetString<fast_ber::Id<fast_ber::Class::context_specific, 2>>,
                                          fast_ber::Boolean<fast_ber::Id<fast_ber::Class::context_specific, 3>>>;
    using TestChoiceIds = typename TestChoices::DefaultIdentifier;

    fast_ber::Choice<TestChoices, TestChoiceIds, fast_ber::StorageMode::static_> choice1;
    choice1 = fast_ber::OctetString<>{};
    choice1 = fast_ber::Integer<>{};
    choice1 = fast_ber::Boolean<>{};

    fast_ber::Choice<TestChoices, TestChoiceIds, fast_ber::StorageMode::dynamic> choice2;
    choice2 = fast_ber::OctetString<>{};
    choice2 = fast_ber::Integer<>{};
    choice2 = fast_ber::Boolean<>{};

    fast_ber::MakeAChoice::Collection<> collection{fast_ber::Boolean<>(true)};

    collection.the_choice = fast_ber::OctetString<fast_ber::Id<fast_ber::Class::context_specific, 0>>("one");
    CHECK(fast_ber::get<0>(collection.the_choice) == "one");

    collection.the_choice = fast_ber::OctetString<fast_ber::Id<fast_ber::Class::context_specific, 1>>("two");
    CHECK(fast_ber::get<1>(collection.the_choice) == "two");

    collection.the_choice = fast_ber::Integer<>(5);
    CHECK(fast_ber::get<2>(collection.the_choice) == 5);

    collection.the_choice = fast_ber::Boolean<>(true);
    CHECK(fast_ber::get<3>(collection.the_choice));

    collection.the_choice = fast_ber::Boolean<>(false);
    CHECK(!fast_ber::get<3>(collection.the_choice));
}

TEST_CASE("Choice: Tags")
{
    fast_ber::MakeAChoice::Collection<> c;
    CHECK(std::is_same<
          fast_ber::Identifier<decltype(c.the_choice)>,
          fast_ber::ChoiceId<
              fast_ber::Id<fast_ber::Class::context_specific, 0>, fast_ber::Id<fast_ber::Class::context_specific, 1>,
              fast_ber::Id<fast_ber::Class::context_specific, 2>, fast_ber::Id<fast_ber::Class::context_specific, 3>,
              fast_ber::Id<fast_ber::Class::context_specific, 4>>>::value);
    CHECK(std::is_same<fast_ber::Identifier<fast_ber::variant_alternative_t<0, decltype(c.the_choice)>>,
                       fast_ber::Id<fast_ber::Class::context_specific, 0>>::value);
    CHECK(std::is_same<fast_ber::Identifier<fast_ber::variant_alternative_t<1, decltype(c.the_choice)>>,
                       fast_ber::Id<fast_ber::Class::context_specific, 1>>::value);
    CHECK(std::is_same<fast_ber::Identifier<fast_ber::variant_alternative_t<2, decltype(c.the_choice)>>,
                       fast_ber::Id<fast_ber::Class::context_specific, 2>>::value);
    CHECK(std::is_same<fast_ber::Identifier<fast_ber::variant_alternative_t<3, decltype(c.the_choice)>>,
                       fast_ber::Id<fast_ber::Class::context_specific, 3>>::value);
    CHECK(std::is_same<fast_ber::Identifier<fast_ber::variant_alternative_t<4, decltype(c.the_choice)>>,
                       fast_ber::Id<fast_ber::Class::context_specific, 4>>::value);
}

TEST_CASE("Choice: Explicit Tags")
{
    fast_ber::ExplicitChoice::MyChoice<> c;
    CHECK(std::is_same<
          fast_ber::Identifier<absl::remove_reference_t<decltype(c)>>,
          fast_ber::ChoiceId<
              fast_ber::Id<fast_ber::Class::context_specific, 0>, fast_ber::Id<fast_ber::Class::context_specific, 1>,
              fast_ber::Id<fast_ber::Class::context_specific, 2>, fast_ber::Id<fast_ber::Class::context_specific, 3>,
              fast_ber::Id<fast_ber::Class::context_specific, 4>>>::value);

    CHECK(std::is_same<fast_ber::Identifier<fast_ber::variant_alternative_t<0, decltype(c)::AliasedType>>,
                       fast_ber::DoubleId<fast_ber::Id<fast_ber::Class::context_specific, 0>,
                                          fast_ber::ExplicitId<fast_ber::UniversalTag::octet_string>>>::value);
    CHECK(std::is_same<fast_ber::Identifier<fast_ber::variant_alternative_t<1, decltype(c)::AliasedType>>,
                       fast_ber::DoubleId<fast_ber::Id<fast_ber::Class::context_specific, 1>,
                                          fast_ber::ExplicitId<fast_ber::UniversalTag::octet_string>>>::value);
    CHECK(std::is_same<fast_ber::Identifier<fast_ber::variant_alternative_t<2, decltype(c)::AliasedType>>,
                       fast_ber::DoubleId<fast_ber::Id<fast_ber::Class::context_specific, 2>,
                                          fast_ber::ExplicitId<fast_ber::UniversalTag::integer>>>::value);
    CHECK(std::is_same<fast_ber::Identifier<fast_ber::variant_alternative_t<3, decltype(c)::AliasedType>>,
                       fast_ber::DoubleId<fast_ber::Id<fast_ber::Class::context_specific, 3>,
                                          fast_ber::ExplicitId<fast_ber::UniversalTag::boolean>>>::value);
    CHECK(std::is_same<fast_ber::Identifier<fast_ber::variant_alternative_t<4, decltype(c)::AliasedType>>,
                       fast_ber::DoubleId<fast_ber::Id<fast_ber::Class::context_specific, 4>,
                                          fast_ber::ExplicitId<fast_ber::UniversalTag::sequence>>>::value);
}
