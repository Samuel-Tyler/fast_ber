#include "autogen/anonymous_members.hpp"

#include "catch2/catch.hpp"

TEST_CASE("Anonymous: Unnamed members of sequence")
{
    CHECK(std::is_same<decltype(fast_ber::Anonymous::Anon::anonymous_member_0), fast_ber::Null<>>::value);
    CHECK(std::is_same<
          fast_ber::Anonymous::Anon::Anonymous_member_1::AliasedType,
          fast_ber::SequenceOf<fast_ber::OctetString<>, fast_ber::ExplicitId<fast_ber::UniversalTag::sequence>,
                               fast_ber::StorageMode::small_buffer_optimised>>::value);
    CHECK(std::is_same<decltype(fast_ber::Anonymous::Anon::anonymous_member_2), fast_ber::Integer<>>::value);
}
