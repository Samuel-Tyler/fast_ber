#include "fast_ber/ber_types/Identifier.hpp"
#include "fast_ber/ber_types/ObjectIdentifier.hpp"
#include "fast_ber/util/EncodeHelpers.hpp"

#include <catch2/catch.hpp>
#include <limits>

TEST_CASE("Object Identifier: Construction")
{
    std::initializer_list<fast_ber::ObjectIdentifierComponents> test_vals{
        {0, 2}, {0, 1, 45993, 2343432, 29999}, {0, 2, 423434343}, {1, 2, 840, 4816, 113549}};

    for (const fast_ber::ObjectIdentifierComponents& val : test_vals)
    {
        fast_ber::ObjectIdentifier<> oid(val);
        REQUIRE(oid.value() == val);
    }
}

TEST_CASE("Object Identifier: Encoding")
{
    fast_ber::ObjectIdentifier<> oid(fast_ber::ObjectIdentifierComponents{1, 2, 840, 113549});
    std::array<uint8_t, 100>     buffer   = {};
    std::array<uint8_t, 8>       expected = {0x06, 0x06, 0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d};
    size_t                       size = fast_ber::encode(absl::Span<uint8_t>(buffer.data(), buffer.size()), oid).length;

    REQUIRE(absl::MakeSpan(buffer.data(), size) == absl::MakeSpan(expected));
}

TEST_CASE("Object Identifier: Assign from raw")
{
    fast_ber::ObjectIdentifier<> oid;
    std::array<uint8_t, 8>       test_data = {0x06, 0x06, 0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d};

    size_t size = oid.assign_ber(absl::MakeSpan(test_data.data(), test_data.size()));
    REQUIRE(oid.value() == fast_ber::ObjectIdentifierComponents{1, 2, 840, 113549});
    REQUIRE(size == test_data.size());
}

TEST_CASE("Object Identifier: Default value")
{
    REQUIRE(fast_ber::ObjectIdentifier<>().value() == fast_ber::ObjectIdentifierComponents{});
}

TEST_CASE("Object Identifier: Encode decode")
{
    using Identifier = fast_ber::DoubleId<fast_ber::Id<fast_ber::Class::context_specific, 2>,
                                          fast_ber::ExplicitId<fast_ber::UniversalTag::integer>>;

    fast_ber::ObjectIdentifier<Identifier> oid1 =
        fast_ber::ObjectIdentifierComponents{1, 2, 9999999999999, 9999999999999, 999999999999999, 9999999999999};
    fast_ber::ObjectIdentifier<Identifier> oid2;

    std::vector<uint8_t> buffer(100, 0x00);

    fast_ber::EncodeResult encode_result = fast_ber::encode(absl::MakeSpan(buffer.data(), buffer.size()), oid1);
    fast_ber::DecodeResult decode_result = fast_ber::decode(absl::MakeSpan(buffer.data(), buffer.size()), oid2);

    CHECK(encode_result.success);
    CHECK(decode_result.success);
    CHECK(oid1 == oid2);
    CHECK(fast_ber::has_correct_header(fast_ber::BerView(buffer), Identifier{}, fast_ber::Construction::primitive));
}
