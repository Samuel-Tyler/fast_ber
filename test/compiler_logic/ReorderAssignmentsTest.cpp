#include "fast_ber/compiler/ReorderAssignments.hpp"

#include "catch2/catch.hpp"

TEST_CASE("ReorderAssignments: No depdendencies")
{
    // No dependencies between assignments.
    // Assignments should maintain their order

    std::vector<Assignment> assignments = {
        Assignment{"Int", TypeAssignment{IntegerType{}}, {}, {}},
        Assignment{"String", TypeAssignment{OctetStringType{}}, {}, {}},
        Assignment{"Boolean", TypeAssignment{BooleanType{}}, {}, {}},
    };

    REQUIRE(is_integer(absl::get<TypeAssignment>(assignments[0].specific).type));
    REQUIRE(is_octet_string(absl::get<TypeAssignment>(assignments[1].specific).type));
    REQUIRE(is_boolean(absl::get<TypeAssignment>(assignments[2].specific).type));

    bool circ      = false;
    auto reordered = reorder_assignments(assignments, "module", circ);

    REQUIRE(assignments.size() == reordered.size());

    REQUIRE(reordered[0].depends_on.size() == 0);
    REQUIRE(reordered[1].depends_on.size() == 0);
    REQUIRE(reordered[2].depends_on.size() == 0);

    REQUIRE(is_integer(absl::get<TypeAssignment>(reordered[0].specific).type));
    REQUIRE(is_octet_string(absl::get<TypeAssignment>(reordered[1].specific).type));
    REQUIRE(is_boolean(absl::get<TypeAssignment>(reordered[2].specific).type));
}

TEST_CASE("ReorderAssignments: Defined")
{
    std::vector<Assignment> assignments = {
        Assignment{"Defined", TypeAssignment{DefinedType{{}, {"Boolean"}, {}}}, {}, {}},
        Assignment{"Boolean", TypeAssignment{BooleanType{}}, {}, {}},
    };

    REQUIRE(is_defined(absl::get<TypeAssignment>(assignments[0].specific).type));
    REQUIRE(is_boolean(absl::get<TypeAssignment>(assignments[1].specific).type));

    bool circ      = false;
    auto reordered = reorder_assignments(assignments, "module", circ);

    REQUIRE(assignments.size() == reordered.size());

    REQUIRE(reordered[0].depends_on.size() == 0);
    REQUIRE(reordered[1].depends_on.size() == 1);

    REQUIRE(is_boolean(absl::get<TypeAssignment>(reordered[0].specific).type));
    REQUIRE(is_defined(absl::get<TypeAssignment>(reordered[1].specific).type));
}

TEST_CASE("ReorderAssignments: Sequence")
{
    std::vector<Assignment> assignments = {
        Assignment{
            "Defined",
            TypeAssignment{
                SequenceType{{ComponentType{NamedType{"member", DefinedType{{}, {"Boolean"}, {}}}, false, {}, {}}}},
            },
            {},
            {}},
        Assignment{"Boolean", TypeAssignment{BooleanType{}}, {}, {}},
    };

    REQUIRE(is_sequence(absl::get<TypeAssignment>(assignments[0].specific).type));
    REQUIRE(is_boolean(absl::get<TypeAssignment>(assignments[1].specific).type));

    bool circ      = false;
    auto reordered = reorder_assignments(assignments, "module", circ);

    REQUIRE(assignments.size() == reordered.size());

    REQUIRE(reordered[0].depends_on.size() == 0);
    REQUIRE(reordered[1].depends_on.size() == 1);

    REQUIRE(is_boolean(absl::get<TypeAssignment>(reordered[0].specific).type));
    REQUIRE(is_sequence(absl::get<TypeAssignment>(reordered[1].specific).type));
}
