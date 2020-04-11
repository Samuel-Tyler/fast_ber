#include "fast_ber/ber_types/All.hpp"

#include "autogen/all_types.hpp"

#include <catch2/catch.hpp>

#include "absl/time/clock.h"

namespace dflt
{
struct StringDefault
{
    constexpr static const char* get_value() noexcept { return "Hello"; }
};

struct IntDefault
{
    constexpr static int get_value() noexcept { return 10; }
};
} // namespace dflt

template <typename T>
struct IsOptional : std::false_type
{
};

template <typename T>
struct IsOptional<fast_ber::Optional<T>> : std::true_type
{
};

template <typename T>
struct IsDefault : std::false_type
{
};

template <typename T, typename U>
struct IsDefault<fast_ber::Default<T, U>> : std::true_type
{
};

template <typename T>
void test_type(const T& a)
{
    using ID = fast_ber::Identifier<T>;

    INFO(a);
    INFO(ID{});

    // Check that type can be default constructed
    CHECK(T{} == T{});
    CHECK(std::is_nothrow_constructible<T>::value);

    // Check that type can be copied
    T b = a;
    b   = a;
    T c(b);
    CHECK(c == a);
    CHECK(std::is_copy_constructible<T>::value);
    CHECK(std::is_copy_assignable<T>::value);

    // Check that type can be moved
    T d = std::move(b);
    d   = std::move(c);
    T e(std::move(d));
    CHECK(e == a);
    CHECK(std::is_nothrow_move_constructible<T>::value);
    CHECK(std::is_nothrow_move_assignable<T>::value);

    // Check that type can be compared
    CHECK(e == a);
    CHECK(!(e != a));

    // Check that type can be serialized
    T                         f;
    std::array<uint8_t, 1000> buffer        = {};
    size_t                    encoded_len   = a.encoded_length();
    fast_ber::EncodeResult    encode_result = a.encode(absl::Span<uint8_t>(buffer));
    fast_ber::DecodeResult    decode_result = f.decode(fast_ber::BerView(absl::MakeSpan(buffer.data(), encoded_len)));
    CHECK(encode_result.success);
    CHECK(decode_result.success);
    CHECK(encode_result.length == encoded_len);
    CHECK(a == f);
    if ((!IsDefault<T>::value && !IsOptional<T>::value)) // Default / Optional types may have empty encoding
    {
        CHECK(encoded_len > 0);
    }
    if (encoded_len > 0)
    {
        CHECK(ID::check_id_match(fast_ber::BerView(buffer).class_(), fast_ber::BerView(buffer).tag()));
    }

    // Destructive tests - Check for undefined behaviour when using too small buffer
    for (size_t i = 0; i < encoded_len; i++)
    {
        INFO(i);

        fast_ber::EncodeResult destructive_encode_result = fast_ber::encode(absl::MakeSpan(buffer.data(), i), a);
        fast_ber::DecodeResult destructive_decode_result = fast_ber::decode(absl::MakeSpan(buffer.data(), i), f);

        CHECK(!destructive_encode_result.success);

        // buffer may be valid for decoding, by chance
        (void)destructive_decode_result;
    }

    // Printing check
    std::cout << "All test : " << a << "\n      id : " << ID{} << "\n\n";
}

template <typename Identifier>
void test_type_with_id()
{
    test_type(fast_ber::BitString<Identifier>("TestString"));
    test_type(fast_ber::Boolean<Identifier>(true));
    test_type(fast_ber::CharacterString<Identifier>("TestString"));
    test_type(fast_ber::Choice<fast_ber::Choices<fast_ber::Boolean<>, fast_ber::OctetString<>>, Identifier>(
        fast_ber::Boolean<>(true)));
    test_type(fast_ber::Choice<fast_ber::Choices<fast_ber::Boolean<>, fast_ber::OctetString<>>, Identifier,
                               fast_ber::StorageMode::dynamic>(fast_ber::Boolean<>(true)));
    // test_type(fast_ber::Date<>);
    // test_type(fast_ber::DateTime<>);
    test_type(fast_ber::Default<fast_ber::Integer<Identifier>, dflt::IntDefault>(dflt::IntDefault::get_value()));
    test_type(fast_ber::Default<fast_ber::Integer<Identifier>, dflt::IntDefault>(-123456));
    test_type(fast_ber::Default<fast_ber::OctetString<Identifier>, dflt::StringDefault>(dflt::StringDefault::get_value()));
    test_type(fast_ber::Default<fast_ber::OctetString<Identifier>, dflt::StringDefault>("Non default string value"));
    // test_type(fast_ber::Duration<>);
    test_type(fast_ber::All::The_Enum<Identifier>(fast_ber::All::The_Enum<>::Values::pear));
    test_type(fast_ber::GeneralizedTime<Identifier>(absl::Now()));
    test_type(fast_ber::Integer<Identifier>(5));
    test_type(fast_ber::Null<Identifier>());
    test_type(fast_ber::ObjectIdentifier<Identifier>(fast_ber::ObjectIdentifierComponents{1, 2, 500, 9999}));
    test_type(fast_ber::OctetString<Identifier>("TestString"));
    test_type(fast_ber::Optional<fast_ber::Null<Identifier>>(fast_ber::Null<>()));
    test_type(fast_ber::Optional<fast_ber::All::The_Set<Identifier>>(fast_ber::All::The_Set<>{"Hello", 42}));
    // test_type(fast_ber::Real);
    test_type(fast_ber::All::The_Sequence<Identifier>{"Hello", 42});
    test_type(fast_ber::SequenceOf<fast_ber::Integer<>, Identifier, fast_ber::StorageMode::small_buffer_optimised>(
        {1, 4, 6, 100, 2555}));
    test_type(
        fast_ber::SequenceOf<fast_ber::Integer<>, Identifier, fast_ber::StorageMode::dynamic>({1, 4, 6, 100, 2555}));
    test_type(fast_ber::All::The_Set<Identifier>{"Hello", 42});
    test_type(fast_ber::SetOf<fast_ber::OctetString<Identifier>>({"A", "list", "of", "strings"}));
    test_type(fast_ber::All::The_Set<Identifier>({"Hello", 42}));
    // test_type(fast_ber::Time);
    // test_type(fast_ber::TimeOfDay);
    test_type(fast_ber::VisibleString<Identifier>("TestString"));
}

TEST_CASE("AllTypes: Check all types share a unified interface")
{
    test_type_with_id<fast_ber::Id<fast_ber::Class::private_, 200>>();
    test_type_with_id<fast_ber::Id<fast_ber::Class::application, 999999999>>();
    test_type_with_id<fast_ber::Id<fast_ber::Class::context_specific, 4000>>();
    test_type_with_id<fast_ber::ExplicitId<fast_ber::UniversalTag::octet_string>>();
    test_type_with_id<fast_ber::DoubleId<fast_ber::Id<fast_ber::Class::private_, 4>,
                                         fast_ber::ExplicitId<fast_ber::UniversalTag::octet_string>>>();
    test_type_with_id<fast_ber::DoubleId<fast_ber::Id<fast_ber::Class::context_specific, 999999999999999>,
                                         fast_ber::Id<fast_ber::Class::context_specific, 9999999999999999>>>();

    test_type(fast_ber::Choice<fast_ber::Choices<fast_ber::Boolean<>, fast_ber::Integer<>, fast_ber::OctetString<>>>(
        fast_ber::Boolean<>(true)));
    test_type(fast_ber::Choice<fast_ber::Choices<fast_ber::Boolean<>, fast_ber::Integer<>, fast_ber::OctetString<>>,
                               fast_ber::Id<fast_ber::Class::application, 20>, fast_ber::StorageMode::dynamic>(
        fast_ber::Boolean<>(true)));
}

TEST_CASE("AllTypes: Default Id")
{
    test_type(fast_ber::BitString<>("TestString"));
    test_type(fast_ber::Boolean<>(true));
    test_type(fast_ber::CharacterString<>("TestString"));
    test_type(fast_ber::Choice<fast_ber::Choices<fast_ber::Boolean<>, fast_ber::OctetString<>>>(true));
    // test_type(fast_ber::Date<>);
    // test_type(fast_ber::DateTime<>);
    // test_type(fast_ber::Duration<>);
    test_type(fast_ber::All::The_Enum<>(fast_ber::All::The_Enum<>::Values::pear));
    test_type(fast_ber::GeneralizedTime<>(absl::Now()));
    test_type(fast_ber::Integer<>(5));
    test_type(fast_ber::Null<>());
    test_type(fast_ber::ObjectIdentifier<>(fast_ber::ObjectIdentifierComponents{1, 2, 500, 9999}));
    test_type(fast_ber::OctetString<>("TestString"));
    test_type(fast_ber::Optional<fast_ber::Null<>>());
    test_type(fast_ber::Optional<fast_ber::Null<>>(fast_ber::Null<>()));
    test_type(fast_ber::Optional<fast_ber::All::The_Set<>>(fast_ber::All::The_Set<>{"Hello", 42}));
    // test_type(fast_ber::Real);
    test_type(fast_ber::All::The_Sequence<>{"Hello", 42});
    test_type(fast_ber::SequenceOf<fast_ber::Integer<>>({1, 4, 6, 100, 2555}));
    test_type(fast_ber::All::The_Set<>{"Hello", 42});
    test_type(fast_ber::SetOf<fast_ber::OctetString<>>({"A", "list", "of", "strings"}));
    test_type(fast_ber::All::The_Set<>({"Hello", 42}));
    // test_type(fast_ber::Time);
    // test_type(fast_ber::TimeOfDay);
    test_type(fast_ber::VisibleString<>("TestString"));
}
