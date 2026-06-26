// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "data/ff/library.h"
#include "nodes/species.h"
#include "tests/testGraph.h"
#include <vector>

namespace UnitTest
{
TEST(LudwigPy5AssignmentTest, Py5)
{
    TestGraph testGraph;
    auto *speciesNode = testGraph.createSpeciesFromStructureAndForcefield("Py5", "ImportXYZStructure", "xyz/py5.xyz",
                                                                          ForcefieldLibrary::forcefield("Ludwig/Py5"));

    ASSERT_TRUE(speciesNode);
    auto &species = speciesNode->species();
    ASSERT_EQ(speciesNode->run(), NodeConstants::ProcessResult::Success);

    ASSERT_EQ(species.bonds().size(), 27);
    ASSERT_EQ(species.angles().size(), 48);
    ASSERT_EQ(species.torsions().size(), 66);
    ASSERT_EQ(species.impropers().size(), 6);

    testSpeciesAtomType(&species,
                        {{0, "nc"},    {1, "ca_o"},  {2, "ca_m"},  {3, "ca_p"},  {4, "ca_m"},  {5, "ca_o"},  {6, "ha_o"},
                         {7, "ha_m"},  {8, "ha_p"},  {9, "ha_m"},  {10, "ha_o"}, {11, "ct_1"}, {12, "ct_2"}, {13, "hc_1"},
                         {14, "hc_1"}, {15, "ct_3"}, {16, "hc_2"}, {17, "hc_2"}, {18, "ct_4"}, {19, "hc_3"}, {20, "hc_3"},
                         {21, "cm"},   {22, "hc_4"}, {23, "hc_4"}, {24, "hm"},   {25, "hm"},   {26, "hm"}});
    testSpeciesIntramolecular(&species, {0, 1}, {BondFunctions::Form::Harmonic, "k=4042 eq=1.339"});
    testSpeciesIntramolecular(&species, {1, 6}, {BondFunctions::Form::Harmonic, "k=3071 eq=1.08"});
    testSpeciesIntramolecular(&species, {11, 0, 1}, {AngleFunctions::Form::Harmonic, "k=585.8 eq=121.5"});
    testSpeciesIntramolecular(&species, {0, 1, 2, 3}, {TorsionFunctions::Form::Cosine, "15.178  2  180  1"});
    testSpeciesIntramolecular(&species, {0, 1, 5, 11}, {TorsionFunctions::Form::Cosine, "4.606 2 180 1"});
    testSpeciesIntramolecular(&species, {1, 0, 2, 6}, {TorsionFunctions::Form::Cosine, "4.606 2 180 1"});
    testSpeciesIntramolecular(&species, {2, 1, 3, 7}, {TorsionFunctions::Form::Cosine, "4.606 2 180 1"});
    testSpeciesIntramolecular(&species, {3, 2, 4, 8}, {TorsionFunctions::Form::Cosine, "4.606 2 180 1"});
    testSpeciesIntramolecular(&species, {4, 3, 5, 9}, {TorsionFunctions::Form::Cosine, "4.606 2 180 1"});
    testSpeciesIntramolecular(&species, {5, 0, 4, 10}, {TorsionFunctions::Form::Cosine, "4.606 2 180 1"});
}
}; // namespace UnitTest
