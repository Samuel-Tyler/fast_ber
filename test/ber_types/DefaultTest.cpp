#include "fast_ber/ber_types/Default.hpp"

#include "fast_ber/ber_types/Integer.hpp"
#include "fast_ber/ber_types/OctetString.hpp"

#include <catch2/catch.hpp>

struct StringDefault
{
    constexpr static const char* const value = "Hello";
};

struct IntDefault
{
    constexpr static const int value = 10;
};

constexpr const char* const StringDefault::value;
constexpr const int IntDefault::value;

TEST_CASE("Default: Construct")
{
    fast_ber::Default<fast_ber::OctetString<>, StringDefault> default_str;
    fast_ber::Default<fast_ber::Integer<>, IntDefault>        default_int;

    CHECK(default_str == "Hello");
    CHECK(default_str != "Goodbye");
    CHECK(default_str.is_default());

    CHECK(default_int == 10);
    CHECK(default_int != -10);
    CHECK(default_str.is_default());
}

TEST_CASE("Default: Assign")
{
    fast_ber::Default<fast_ber::OctetString<>, StringDefault> default_str = "racoon";
    fast_ber::Default<fast_ber::Integer<>, IntDefault>        default_int = -20;

    CHECK(!default_str.is_default());
    CHECK(default_str == "racoon");
    CHECK(!default_int.is_default());
    CHECK(default_int == -20);

    default_str = "Hello";
    default_int = 10;

    CHECK(default_str.is_default());
    CHECK(default_str == "Hello");
    CHECK(default_int.is_default());
    CHECK(default_int == 10);
}

TEST_CASE("Default: Encode default")
{
    fast_ber::Default<fast_ber::OctetString<>, StringDefault> default_str;
    fast_ber::Default<fast_ber::Integer<>, IntDefault>        default_int;
    std::array<uint8_t, 100>                                  buffer;

    size_t encoded_length_1 = fast_ber::encoded_length(default_str);
    size_t encoded_length_2 = fast_ber::encoded_length(default_int);

    fast_ber::EncodeResult encode_res_1 = fast_ber::encode(absl::Span<uint8_t>(buffer), default_str);
    fast_ber::EncodeResult encode_res_2 = fast_ber::encode(absl::Span<uint8_t>(buffer), default_int);

    CHECK(encoded_length_1 == 0);
    CHECK(encoded_length_2 == 0);

    CHECK(encode_res_1.success);
    CHECK(encode_res_2.success);

    CHECK(encode_res_1.length == 0);
    CHECK(encode_res_2.length == 0);
}

TEST_CASE("Default: Encode non default")
{
    fast_ber::Default<fast_ber::OctetString<>, StringDefault> default_str = "racoon";
    fast_ber::Default<fast_ber::Integer<>, IntDefault>        default_int = -20;
    std::array<uint8_t, 100>                                  buffer1;
    std::array<uint8_t, 100>                                  buffer2;
    std::array<uint8_t, 100>                                  buffer3;
    std::array<uint8_t, 100>                                  buffer4;

    size_t encoded_length_1 = fast_ber::encoded_length(default_str);
    size_t encoded_length_2 = fast_ber::encoded_length(default_int);
    size_t encoded_length_3 = fast_ber::encoded_length(fast_ber::OctetString<>("racoon"));
    size_t encoded_length_4 = fast_ber::encoded_length(fast_ber::Integer<>(-20));

    fast_ber::EncodeResult encode_res_1 = fast_ber::encode(absl::Span<uint8_t>(buffer1), default_str);
    fast_ber::EncodeResult encode_res_2 = fast_ber::encode(absl::Span<uint8_t>(buffer2), default_int);
    fast_ber::EncodeResult encode_res_3 =
        fast_ber::encode(absl::Span<uint8_t>(buffer3), fast_ber::OctetString<>("racoon"));
    fast_ber::EncodeResult encode_res_4 = fast_ber::encode(absl::Span<uint8_t>(buffer4), fast_ber::Integer<>(-20));

    CHECK(encoded_length_1 == encoded_length_3);
    CHECK(encoded_length_2 == encoded_length_4);

    CHECK(encode_res_1.success);
    CHECK(encode_res_2.success);
    CHECK(encode_res_3.success);
    CHECK(encode_res_4.success);

    CHECK(encode_res_1.length == encode_res_3.length);
    CHECK(encode_res_2.length == encode_res_4.length);
}

TEST_CASE("Default: Decode default")
{
    fast_ber::Default<fast_ber::OctetString<>, StringDefault> default_str;
    fast_ber::Default<fast_ber::Integer<>, IntDefault>        default_int;
    std::array<uint8_t, 100>                                  buffer;

    fast_ber::DecodeResult decode_res_1 = fast_ber::decode(absl::Span<uint8_t>(buffer.data(), 0), default_str);
    fast_ber::DecodeResult decode_res_2 = fast_ber::decode(absl::Span<uint8_t>(buffer.data(), 0), default_int);

    CHECK(decode_res_1.success);
    CHECK(decode_res_2.success);

    CHECK(default_str == StringDefault::value);
    CHECK(default_int == IntDefault::value);
}

TEST_CASE("Default: Decode non default")
{
    fast_ber::Default<fast_ber::OctetString<>, StringDefault> default_str;
    fast_ber::Default<fast_ber::Integer<>, IntDefault>        default_int;
    std::array<uint8_t, 100>                                  buffer;

    fast_ber::OctetString<>("Unexpected string!").encode(absl::Span<uint8_t>(buffer));
    fast_ber::DecodeResult decode_res_1 = fast_ber::decode(absl::Span<uint8_t>(buffer), default_str);

    fast_ber::Integer<>(-99999999).encode(absl::Span<uint8_t>(buffer));
    fast_ber::DecodeResult decode_res_2 = fast_ber::decode(absl::Span<uint8_t>(buffer), default_int);

    CHECK(decode_res_1.success);
    CHECK(decode_res_2.success);

    CHECK(default_str == "Unexpected string!");
    CHECK(default_int == -99999999);
    CHECK(default_str != StringDefault::value);
    CHECK(default_int != IntDefault::value);
}
