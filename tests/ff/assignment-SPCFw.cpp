// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "data/ff/library.h"
#include "tests/graphData.h"
#include "tests/testData.h"
#include <gtest/gtest.h>

namespace UnitTest
{
TEST(SPCFwAssignmentTest, Water)
{
    DissolveSystemTest systemTest;
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

    systemTest.checkSpeciesAtomType(&species, {{0, "OW"}, {1, "HW"}, {2, "HW"}});
    systemTest.checkSpeciesIntramolecular(&species, {0, 1}, {BondFunctions::Form::Harmonic, "k=4431.53 eq=1"});
    systemTest.checkSpeciesIntramolecular(&species, {1, 0, 2}, {AngleFunctions::Form::Harmonic, "k=317.5656 eq=113.24"});
}
}; // namespace UnitTest
