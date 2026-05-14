// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "classes/neutronWeights.h"
#include "classes/isotopologueSet.h"
#include "tests/graphData.h"
#include "tests/tempFile.h"
#include "tests/testData.h"
#include <gtest/gtest.h>

namespace UnitTest
{
TEST(NeutronWeightsTest, Simple)
{
    std::map<const Species *, double> speciesPopulations;
    IsotopologueSet isotopologues;
    auto N2node = TestGraph::loadTOMLSpecies("species/n2.toml");
    auto &N2 = N2node->species();
    auto atN = N2.findAtomType("N");
    ASSERT_TRUE(atN);

    // Basic population of ten molecules with two atoms of the same type (N2)
    speciesPopulations[&N2] = 10;
    isotopologues.add(N2.naturalIsotopologue(), 1.0);
    NeutronWeights nwts(speciesPopulations, isotopologues);
    EXPECT_EQ(20, nwts.isotopeMix().population(atN));
    EXPECT_NEAR(1.0, nwts.isotopeMix().fraction(atN), 1.0e-6);
    EXPECT_NEAR(pow(Sears91::boundCoherent(Sears91::N_Natural), 2) / 100.0, nwts.boundCoherentSquareOfAverage(), 1.0e-6);

    // Adding more N2 natural isotopologue shouldn't make any difference - the species already exists, so just the
    // relative weight of the isotopologue will be updated, and this will be normalised back to 1.0.
    isotopologues.add(N2.naturalIsotopologue(), 50.0);
    nwts = NeutronWeights(speciesPopulations, isotopologues);
    EXPECT_EQ(20, nwts.isotopeMix().population(atN));
    EXPECT_NEAR(1.0, nwts.isotopeMix().fraction(atN), 1.0e-6);
    EXPECT_NEAR(pow(Sears91::boundCoherent(Sears91::N_Natural), 2) / 100.0, nwts.boundCoherentSquareOfAverage(), 1.0e-6);
}

TEST(NeutronWeightsTest, Water)
{
    std::map<const Species *, double> speciesPopulations;
    IsotopologueSet isotopologues;
    auto H2Onode = TestGraph::loadTOMLSpecies("species/water.toml");
    auto &H2O = H2Onode->species();
    auto atOW = H2O.findAtomType("OW");
    ASSERT_TRUE(atOW);
    auto atHW = H2O.findAtomType("HW");
    ASSERT_TRUE(atHW);

    speciesPopulations[&H2O] = 1;
    isotopologues.add(H2O.naturalIsotopologue(), 1.0);

    NeutronWeights nwts(speciesPopulations, isotopologues);
    EXPECT_EQ(1, nwts.isotopeMix().population(atOW));
    EXPECT_EQ(2, nwts.isotopeMix().population(atHW));
    EXPECT_NEAR(1.0 / 3.0, nwts.isotopeMix().fraction(atOW), 1.0e-6);
    EXPECT_NEAR(2.0 / 3.0, nwts.isotopeMix().fraction(atHW), 1.0e-6);
    EXPECT_NEAR(
        pow((Sears91::boundCoherent(Sears91::O_Natural) / 3.0) + (Sears91::boundCoherent(Sears91::H_Natural) * 2.0 / 3.0), 2) /
            100.0,
        nwts.boundCoherentSquareOfAverage(), 1.0e-6);
}

TEST(NeutronWeightsTest, D2O)
{
    std::map<const Species *, double> speciesPopulations;
    IsotopologueSet isotopologues;
    auto H2Onode = TestGraph::loadTOMLSpecies("species/water.toml");
    auto &H2O = H2Onode->species();
    auto atOW = H2O.findAtomType("OW");
    ASSERT_TRUE(atOW);
    auto atHW = H2O.findAtomType("HW");
    ASSERT_TRUE(atHW);

    speciesPopulations[&H2O] = 1;
    isotopologues.add(H2O.findIsotopologue("D2O"), 1.0);

    NeutronWeights nwts(speciesPopulations, isotopologues);
    EXPECT_EQ(1, nwts.isotopeMix().population(atOW));
    EXPECT_EQ(2, nwts.isotopeMix().population(atHW));
    EXPECT_NEAR(1.0 / 3.0, nwts.isotopeMix().fraction(atOW), 1.0e-6);
    EXPECT_NEAR(2.0 / 3.0, nwts.isotopeMix().fraction(atHW), 1.0e-6);
    EXPECT_NEAR(
        pow((Sears91::boundCoherent(Sears91::O_Natural) / 3.0) + (Sears91::boundCoherent(Sears91::H_2) * 2.0 / 3.0), 2) / 100.0,
        nwts.boundCoherentSquareOfAverage(), 1.0e-6);
}

TEST(NeutronWeightsTest, NullWater)
{
    std::map<const Species *, double> speciesPopulations;
    IsotopologueSet isotopologues;
    auto H2Onode = TestGraph::loadTOMLSpecies("species/water.toml");
    auto &H2O = H2Onode->species();
    auto atOW = H2O.findAtomType("OW");
    ASSERT_TRUE(atOW);
    auto atHW = H2O.findAtomType("HW");
    ASSERT_TRUE(atHW);

    speciesPopulations[&H2O] = 1000;

    auto ratio = fabs(Sears91::boundCoherent(Sears91::H_2) / Sears91::boundCoherent(Sears91::H_Natural));
    isotopologues.add(H2O.naturalIsotopologue(), ratio);
    isotopologues.add(H2O.findIsotopologue("D2O"), 1.0);

    NeutronWeights nwts(speciesPopulations, isotopologues);
    EXPECT_EQ(1000, nwts.isotopeMix().population(atOW));
    EXPECT_EQ(2000, nwts.isotopeMix().population(atHW));
    EXPECT_NEAR(1.0 / 3.0, nwts.isotopeMix().fraction(atOW), 1.0e-6);
    EXPECT_NEAR(2.0 / 3.0, nwts.isotopeMix().fraction(atHW), 1.0e-6);
    EXPECT_NEAR(pow(Sears91::boundCoherent(Sears91::O_Natural) / 3.0, 2) / 100.0, nwts.boundCoherentSquareOfAverage(), 1.0e-6);

    // Making the H atomtype exchangeable should make no difference to <b>**2
    nwts = NeutronWeights(speciesPopulations, isotopologues, Exchangeables({atHW->name()}));
    EXPECT_NEAR(pow(Sears91::boundCoherent(Sears91::O_Natural) / 3.0, 2) / 100.0, nwts.boundCoherentSquareOfAverage(), 1.0e-6);
}

TEST(NeutronWeightsTest, Mix)
{
    std::map<const Species *, double> speciesPopulations;
    IsotopologueSet isotopologues;
    auto H2Onode = TestGraph::loadTOMLSpecies("species/water.toml");
    auto &H2O = H2Onode->species();
    auto atOW = H2O.findAtomType("OW");
    ASSERT_TRUE(atOW);
    auto atHW = H2O.findAtomType("HW");
    ASSERT_TRUE(atHW);
    auto N2node = TestGraph::loadTOMLSpecies("species/n2.toml");
    auto &N2 = N2node->species();
    auto atN = N2.findAtomType("N");
    ASSERT_TRUE(atN);

    speciesPopulations[&N2] = 1;
    speciesPopulations[&H2O] = 1;

    isotopologues.add(H2O.naturalIsotopologue(), 1.0);
    isotopologues.add(N2.naturalIsotopologue(), 1.0);
    isotopologues.add(N2.findIsotopologue("N15"), 1.0);

    NeutronWeights nwts(speciesPopulations, isotopologues);
    EXPECT_EQ(1, nwts.isotopeMix().population(atOW));
    EXPECT_EQ(2, nwts.isotopeMix().population(atHW));
    EXPECT_EQ(2, nwts.isotopeMix().population(atN));
    EXPECT_NEAR(1.0 / 5.0, nwts.isotopeMix().fraction(atOW), 1.0e-6);
    EXPECT_NEAR(2.0 / 5.0, nwts.isotopeMix().fraction(atHW), 1.0e-6);
    EXPECT_NEAR(2.0 / 5.0, nwts.isotopeMix().fraction(atN), 1.0e-6);
    EXPECT_NEAR(pow((Sears91::boundCoherent(Sears91::O_Natural) / 5.0) +
                        (Sears91::boundCoherent(Sears91::H_Natural) * 2.0 / 5.0) +
                        (Sears91::boundCoherent(Sears91::N_Natural) / 5.0) + (Sears91::boundCoherent(Sears91::N_15) / 5.0),
                    2) /
                    100.0,
                nwts.boundCoherentSquareOfAverage(), 1.0e-6);
}

} // namespace UnitTest
