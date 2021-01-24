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
    // FIXME: check types are nothrow move assignable
    // CHECK(std::is_nothrow_move_assignable<T>::value);

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

TEST_CASE("AllTypes: Check all types share a unified interface")
{
    test_type(fast_ber::All::The_BitString("TestString"));
    test_type(fast_ber::All::The_Boolean(true));
    test_type(fast_ber::All::The_CharacterString("TestString"));
    test_type(fast_ber::All::The_Choice(fast_ber::All::The_Choice::Member_one(true)));
    // test_type(fast_ber::Date<>);
    // test_type(fast_ber::DateTime<>);
    test_type(fast_ber::Default<fast_ber::Integer<>, dflt::IntDefault>(dflt::IntDefault::get_value()));
    test_type(fast_ber::Default<fast_ber::All::The_Integer, dflt::IntDefault>(-123456));
    test_type(fast_ber::Default<fast_ber::All::The_OctetString, dflt::StringDefault>(dflt::StringDefault::get_value()));
    test_type(fast_ber::Default<fast_ber::OctetString<>, dflt::StringDefault>("Non default string value"));
    // test_type(fast_ber::Duration<>);
    test_type(fast_ber::All::The_Enum(fast_ber::All::The_Enum::Values::pear));
    test_type(fast_ber::All::The_GeneralizedTime(absl::Now()));
    test_type(fast_ber::All::The_Integer(5));
    test_type(fast_ber::All::The_Null());
    test_type(fast_ber::All::The_ObjectIdentifier(fast_ber::ObjectIdentifierComponents{1, 2, 500, 9999}));
    test_type(fast_ber::All::The_OctetString("TestString"));
    test_type(fast_ber::Optional<fast_ber::All::The_Null>(fast_ber::All::The_Null()));
    test_type(fast_ber::Optional<fast_ber::Integer<>>(42));
    test_type(fast_ber::All::The_Real(0.7774750819971197));
    test_type(fast_ber::All::The_Sequence{"Hello", 42});
    test_type(fast_ber::SequenceOf<fast_ber::Integer<>, fast_ber::ExplicitId<fast_ber::UniversalTag::sequence>,
                                   fast_ber::StorageMode::small_buffer_optimised>({1, 4, 6, 100, 2555}));
    test_type(fast_ber::SequenceOf<fast_ber::Integer<>, fast_ber::ExplicitId<fast_ber::UniversalTag::sequence>,
                                   fast_ber::StorageMode::dynamic>({1, 4, 6, 100, 2555}));
    test_type(fast_ber::All::The_Set{"Hello", 42});
    test_type(fast_ber::All::The_SetOf({"A", "list", "of", "strings"}));
    // test_type(fast_ber::Time);
    // test_type(fast_ber::TimeOfDay);
    test_type(fast_ber::All::The_VisibleString("TestString"));
}
