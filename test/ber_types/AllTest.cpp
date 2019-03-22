#include "fast_ber/ber_types/All.hpp"

#include "autogen/all_types.hpp"

#include <catch2/catch.hpp>

using namespace fast_ber;
enum TestEnum
{
    apple,
    pear,
    orange
};

template <typename T>
void test_type(const T& a)
{
    // Check that type can be default constructed
    REQUIRE(T{} == T{});

    // Check that type can be copied
    T b = a;
    T c(b);
    REQUIRE(c == a);

    // Check that type can be moved
    T d = std::move(b);
    T e(std::move(d));
    REQUIRE(e == a);

    // Check that type can be compared
    REQUIRE(e == a);
    REQUIRE(!(e != a));

    // Check that type can be serialized
    T                         f;
    std::array<uint8_t, 1000> buffer        = {};
    fast_ber::EncodeResult    encode_result = fast_ber::encode(absl::Span<uint8_t>(buffer), a);
    fast_ber::DecodeResult    decode_result = fast_ber::decode(absl::Span<uint8_t>(buffer), f);
    REQUIRE(encode_result.success);
    REQUIRE(decode_result.success);
    REQUIRE(fast_ber::BerView(buffer).tag() == fast_ber::reference_tag(identifier(&a)));
    REQUIRE(a == f);
}

TEST_CASE("AllTypes: Check all types share a unified interface")
{
    test_type(fast_ber::BitString("TestString"));
    test_type(fast_ber::Boolean(true));
    test_type(fast_ber::CharacterString("TestString"));
    test_type(fast_ber::Choice<fast_ber::Boolean, fast_ber::Integer, fast_ber::OctetString>(fast_ber::Boolean(true)));
    // test_type(fast_ber::Date);
    // test_type(fast_ber::DateTime);
    // test_type(fast_ber::Duration);
    // test_type(fast_ber::EmbeddedPDV);
    test_type(fast_ber::All::The_Enum(fast_ber::All::The_Enum::pear));
    // test_type(fast_ber::External);
    test_type(fast_ber::GeneralizedTime(absl::Now()));
    // test_type(fast_ber::IRI);
    // test_type(fast_ber::InstanceOf);
    test_type(fast_ber::Integer(5));
    test_type(fast_ber::Null());
    // test_type(fast_ber::ObjectField);
    test_type(fast_ber::ObjectIdentifier(fast_ber::ObjectIdentifierComponents{1, 2, 500, 9999}));
    test_type(fast_ber::OctetString("TestString"));
    test_type(fast_ber::Optional<fast_ber::Null>(fast_ber::Null()));
    // test_type(fast_ber::Prefixed);
    // test_type(fast_ber::Real);
    // test_type(fast_ber::RelativeIRI);
    // test_type(fast_ber::RelativeOID);
    test_type(fast_ber::All::The_Sequence{"Hello", 42});
    test_type(fast_ber::SequenceOf<fast_ber::Integer>({1, 4, 6, 100, 2555}));
    test_type(fast_ber::All::The_Set{"Hello", 42});
    test_type(fast_ber::SetOf<fast_ber::OctetString>({"A", "list", "of", "strings"}));
    test_type(
        fast_ber::TaggedType<fast_ber::Integer, fast_ber::ImplicitIdentifier<fast_ber::Class::application, 500>>(500));
    // test_type(fast_ber::Time);
    // test_type(fast_ber::TimeOfDay);
    test_type(fast_ber::VisibleString("TestString"));
}
