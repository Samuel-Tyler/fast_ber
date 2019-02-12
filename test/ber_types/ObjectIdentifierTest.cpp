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
        fast_ber::ObjectIdentifier oid(val);
        REQUIRE(oid.value() == val);
    }
}

TEST_CASE("Object Identifier: Encoding")
{
    fast_ber::ObjectIdentifier oid(fast_ber::ObjectIdentifierComponents{1, 2, 840, 113549});
    std::array<uint8_t, 100>   buffer   = {};
    std::array<uint8_t, 8>     expected = {0x06, 0x06, 0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d};
    size_t                     size = fast_ber::encode(absl::Span<uint8_t>(buffer.data(), buffer.size()), oid).length;

    REQUIRE(absl::MakeSpan(buffer.data(), size) == absl::MakeSpan(expected));
}

TEST_CASE("Object Identifier: Assign from raw")
{
    fast_ber::ObjectIdentifier oid;
    std::array<uint8_t, 8>     test_data = {0x06, 0x06, 0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d};

    size_t size = oid.assign_ber(absl::MakeSpan(test_data.data(), test_data.size()));
    REQUIRE(oid.value() == fast_ber::ObjectIdentifierComponents{1, 2, 840, 113549});
    REQUIRE(size == test_data.size());
}

TEST_CASE("Object Identifier: Default value")
{
    REQUIRE(fast_ber::ObjectIdentifier().value() == fast_ber::ObjectIdentifierComponents{});
}
