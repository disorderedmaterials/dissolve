// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "data/ff/library.h"
#include "nodes/species.h"
#include "tests/testGraph.h"

namespace UnitTest
{
TEST(OPLSAA2005AromaticsAssignmentTest, Benzene)
{
    TestGraph testGraph;
    auto *speciesNode = testGraph.createSpeciesFromStructureAndForcefield(
        "Benzene", "ImportXYZStructure", "xyz/benzene.xyz", ForcefieldLibrary::forcefield("OPLSAA2005/Aromatics"));

    ASSERT_TRUE(speciesNode);
    auto &species = speciesNode->species();
    ASSERT_EQ(speciesNode->run(), NodeConstants::ProcessResult::Success);

    ASSERT_EQ(species.bonds().size(), 12);
    ASSERT_EQ(species.angles().size(), 18);
    ASSERT_EQ(species.torsions().size(), 24);
    ASSERT_EQ(species.impropers().size(), 6);

    checkSpeciesAtomType(&species, {{0, "CA"}, {1, "HA"}});
    checkSpeciesIntramolecular(&species, {0, 1}, {BondFunctions::Form::Harmonic, "k=3071.06 eq=1.08"});
    checkSpeciesIntramolecular(&species, {0, 2}, {BondFunctions::Form::Harmonic, "k=3924.59 eq=1.4"});
    checkSpeciesIntramolecular(&species, {1, 0, 2}, {AngleFunctions::Form::Harmonic, "k=292.88 eq=120"});
    checkSpeciesIntramolecular(&species, {0, 2, 4}, {AngleFunctions::Form::Harmonic, "k=527.184 eq=120"});
    checkSpeciesIntramolecular(&species, {1, 0, 2, 3}, {TorsionFunctions::Form::Cos3, "0  30.334  0"});
    checkSpeciesIntramolecular(&species, {1, 0, 2, 4}, {TorsionFunctions::Form::Cos3, "0  30.334  0"});
    checkSpeciesIntramolecular(&species, {10, 0, 2, 4}, {TorsionFunctions::Form::Cos3, "0  30.334  0"});
}

TEST(OPLSAA2005AromaticsAssignmentTest, Naphthalene)
{
    TestGraph testGraph;
    auto *speciesNode = testGraph.createSpeciesFromStructureAndForcefield(
        "Naphthalene", "ImportXYZStructure", "xyz/naphthalene.xyz", ForcefieldLibrary::forcefield("OPLSAA2005/Aromatics"));

    ASSERT_TRUE(speciesNode);
    auto &species = speciesNode->species();
    ASSERT_EQ(speciesNode->run(), NodeConstants::ProcessResult::Success);

    ASSERT_EQ(species.bonds().size(), 19);
    ASSERT_EQ(species.angles().size(), 30);
    ASSERT_EQ(species.torsions().size(), 44);
    ASSERT_EQ(species.impropers().size(), 10);

    checkSpeciesAtomType(&species, {{0, "CA"},
                                    {1, "CA"},
                                    {2, "CNap"},
                                    {3, "CA"},
                                    {4, "CA"},
                                    {5, "CA"},
                                    {6, "CA"},
                                    {7, "CNap"},
                                    {8, "CA"},
                                    {9, "CA"},
                                    {10, "HA"},
                                    {11, "HA"},
                                    {12, "HA"},
                                    {13, "HA"},
                                    {14, "HA"},
                                    {15, "HA"},
                                    {16, "HA"},
                                    {17, "HA"}});
    checkSpeciesIntramolecular(&species, {2, 7}, {BondFunctions::Form::Harmonic, "k=3924.59 eq=1.4"});
    checkSpeciesIntramolecular(&species, {1, 2, 7}, {AngleFunctions::Form::Harmonic, "k=527.184 eq=120"});
    checkSpeciesIntramolecular(&species, {1, 2, 3}, {AngleFunctions::Form::Harmonic, "k=527.184 eq=120"});
    checkSpeciesIntramolecular(&species, {8, 0, 1, 2}, {TorsionFunctions::Form::Cos3, "0  30.334  0"});
}
}; // namespace UnitTest
