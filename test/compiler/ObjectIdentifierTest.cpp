#include "autogen/object_identifier.hpp"

#include "fast_ber/ber_types/Enumerated.hpp"
#include "fast_ber/util/BerView.hpp"

#include "catch2/catch.hpp"

#include <vector>

void test_encoding(const fast_ber::ObjectIdentifier& oid, const std::vector<uint8_t>& expected)
{
    std::vector<uint8_t>          buffer(200, 0x0);
    const fast_ber::EncodeResult& result = fast_ber::encode(absl::Span<uint8_t>(buffer), oid);
    REQUIRE(result.success);
    REQUIRE(absl::Span<const uint8_t>(buffer.data(), result.length) == expected);
}

void test_decoding(const fast_ber::ObjectIdentifier& expected_oid, const std::vector<uint8_t>& data)
{
    fast_ber::ObjectIdentifier    decoded_oid;
    const fast_ber::DecodeResult& result = fast_ber::decode(absl::Span<const uint8_t>(data), decoded_oid);
    REQUIRE(result.success);
    REQUIRE(decoded_oid == expected_oid);
}

TEST_CASE("ObjectIdentifier (Compiled): Test encoding")
{
    test_encoding(fast_ber::ObjectIds::rsa_oid, {0x06, 0x09, 0x2A, 0x86, 0x48, 0x86, 0xF7, 0x0D, 0x01, 0x01, 0x01});
}

TEST_CASE("ObjectIdentifier (Compiled): Test decoding")
{
    test_decoding(fast_ber::ObjectIds::rsa_oid, {0x06, 0x09, 0x2A, 0x86, 0x48, 0x86, 0xF7, 0x0D, 0x01, 0x01, 0x01});
}
