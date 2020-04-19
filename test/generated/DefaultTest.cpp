#include "autogen/defaults.hpp"

#include "catch2/catch.hpp"

#include <array>
#include <sstream>

TEST_CASE("Default: Generated Defaults")
{
    fast_ber::Defaults::SequenceWithDefault<> s;

    REQUIRE(s.integer == 88);
    REQUIRE(s.flag == false);
    REQUIRE(s.string == "dog");
    REQUIRE(s.defined == -172803790);
    REQUIRE(s.colour == fast_ber::Defaults::ColourValues::blue);
    REQUIRE(s.real == 5523432.12323);

    REQUIRE(s.integer.is_default());
    REQUIRE(s.flag.is_default());
    REQUIRE(s.string.is_default());
    REQUIRE(s.defined.is_default());
    REQUIRE(s.colour.is_default());
    REQUIRE(s.real.is_default());
}

TEST_CASE("Default: Encode")
{
    std::array<std::uint8_t, 10>              buffer{};
    fast_ber::Defaults::SequenceWithDefault<> s;

    fast_ber::EncodeResult encode_result = s.encode(absl::Span<uint8_t>(buffer));

    REQUIRE(encode_result.success);
    REQUIRE(encode_result.length == 2);
}

TEST_CASE("Default: Decode")
{
    std::array<std::uint8_t, 2>               buffer{0x30, 0x00};
    fast_ber::Defaults::SequenceWithDefault<> s{20, true, "cat", -10, fast_ber::Defaults::ColourValues::red, 25.0};

    REQUIRE(!s.integer.is_default());
    REQUIRE(!s.flag.is_default());
    REQUIRE(!s.string.is_default());
    REQUIRE(!s.defined.is_default());
    REQUIRE(!s.colour.is_default());
    REQUIRE(!s.real.is_default());

    fast_ber::DecodeResult decode_result = fast_ber::decode(absl::Span<const uint8_t>(buffer), s);

    REQUIRE(decode_result.success);

    REQUIRE(s.integer.is_default());
    REQUIRE(s.flag.is_default());
    REQUIRE(s.string.is_default());
    REQUIRE(s.defined.is_default());
    REQUIRE(s.colour.is_default());
    REQUIRE(s.real.is_default());
}

TEST_CASE("Default: Serialize")
{
    std::stringstream                         ss;
    fast_ber::Defaults::SequenceWithDefault<> s{{}, true, {}, -10, fast_ber::Defaults::ColourValues::red, -16.0};

    ss << s;
    REQUIRE(ss.str() ==
            R"({"integer" : 88, "flag" : true, "string" : "dog", "defined" : -10, "colour" : "red", "real" : -16})");
}
