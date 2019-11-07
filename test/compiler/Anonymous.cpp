#include "autogen/anonymous_members.hpp"

#include "catch2/catch.hpp"

static_assert(std::is_same<decltype(fast_ber::Anonymous::Anon::anonymous_member_0), fast_ber::Null<>>::value, "Anon");
static_assert(std::is_same<decltype(fast_ber::Anonymous::Anon::anonymous_member_1),
                           fast_ber::SequenceOf<fast_ber::OctetString<>>>::value,
              "Anon");
static_assert(std::is_same<decltype(fast_ber::Anonymous::Anon::anonymous_member_2), fast_ber::Integer<>>::value,
              "Anon");
