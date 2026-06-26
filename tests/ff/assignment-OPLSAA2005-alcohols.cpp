// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "data/ff/library.h"
#include "nodes/species.h"
#include "tests/testGraph.h"

namespace UnitTest
{
TEST(OPLSAA2005AlcoholsAssignmentTest, Methanol)
{
    TestGraph testGraph;
    auto *speciesNode = testGraph.createSpeciesFromStructureAndForcefield("Methanol", "ImportXYZStructure", "xyz/methanol.xyz",
                                                                          ForcefieldLibrary::forcefield("OPLSAA2005/Alcohols"));

    ASSERT_TRUE(speciesNode);
    auto &species = speciesNode->species();
    ASSERT_EQ(speciesNode->run(), NodeConstants::ProcessResult::Success);

    ASSERT_EQ(species.bonds().size(), 5);
    ASSERT_EQ(species.angles().size(), 7);
    ASSERT_EQ(species.torsions().size(), 3);
    ASSERT_EQ(species.impropers().size(), 0);

    testSpeciesAtomType(&species, {{0, "CT"}, {1, "OH"}, {2, "HO"}, {3, "HC"}, {4, "HC"}, {5, "HC"}});
    testSpeciesIntramolecular(&species, {0, 1}, {BondFunctions::Form::Harmonic, "k=2677.76 eq=1.41"});
    testSpeciesIntramolecular(&species, {0, 3}, {BondFunctions::Form::Harmonic, "k=2845.12 eq=1.09"});
    testSpeciesIntramolecular(&species, {0, 1, 2}, {AngleFunctions::Form::Harmonic, "k=460.24 eq=108.5"});
    testSpeciesIntramolecular(&species, {4, 0, 1}, {AngleFunctions::Form::Harmonic, "k=292.88 eq=109.5"});
    testSpeciesIntramolecular(&species, {3, 0, 1, 2}, {TorsionFunctions::Form::Cos3, "0  0  1.47444"});
}
}; // namespace UnitTest
