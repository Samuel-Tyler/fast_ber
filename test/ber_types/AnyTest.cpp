#include "autogen/choice.hpp"

#include "fast_ber/ber_types/Any.hpp"

#include <catch2/catch.hpp>

TEST_CASE("Any: Identifier")
{
    REQUIRE(
        std::is_same<fast_ber::ChoiceId<
                         fast_ber::Id<fast_ber::Class::universal, 3>, fast_ber::Id<fast_ber::Class::universal, 1>,
                         fast_ber::Id<fast_ber::Class::universal, 29>, fast_ber::Id<fast_ber::Class::universal, 24>,
                         fast_ber::Id<fast_ber::Class::universal, 24>, fast_ber::Id<fast_ber::Class::universal, 24>,
                         fast_ber::Id<fast_ber::Class::universal, 24>, fast_ber::Id<fast_ber::Class::universal, 2>,
                         fast_ber::Id<fast_ber::Class::universal, 5>, fast_ber::Id<fast_ber::Class::universal, 6>,
                         fast_ber::Id<fast_ber::Class::universal, 4>, fast_ber::Id<fast_ber::Class::universal, 9>,
                         fast_ber::Id<fast_ber::Class::universal, 23>, fast_ber::Id<fast_ber::Class::universal, 23>,
                         fast_ber::Id<fast_ber::Class::universal, 23>, fast_ber::Id<fast_ber::Class::universal, 26>>,
                     fast_ber::Identifier<fast_ber::Any<>>>::value);
}
