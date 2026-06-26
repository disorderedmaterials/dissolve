// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "data/ff/library.h"
#include "nodes/species.h"
#include "tests/testGraph.h"

namespace UnitTest
{
TEST(SPCFwAssignmentTest, Water)
{
    TestGraph testGraph;
    auto *speciesNode = testGraph.createSpeciesFromStructureAndForcefield("Water", "ImportXYZStructure", "xyz/water.xyz",
                                                                          ForcefieldLibrary::forcefield("SPC/Fw"));

    ASSERT_TRUE(speciesNode);
    auto &species = speciesNode->species();
    ASSERT_EQ(speciesNode->run(), NodeConstants::ProcessResult::Success);

    ASSERT_EQ(species.bonds().size(), 2);
    ASSERT_EQ(species.angles().size(), 1);
    ASSERT_EQ(species.torsions().size(), 0);
    ASSERT_EQ(species.impropers().size(), 0);

    testSpeciesAtomType(&species, {{0, "OW"}, {1, "HW"}, {2, "HW"}});
    testSpeciesIntramolecular(&species, {0, 1}, {BondFunctions::Form::Harmonic, "k=4431.53 eq=1"});
    testSpeciesIntramolecular(&species, {1, 0, 2}, {AngleFunctions::Form::Harmonic, "k=317.5656 eq=113.24"});
}
}; // namespace UnitTest
