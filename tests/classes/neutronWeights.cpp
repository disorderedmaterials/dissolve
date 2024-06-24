// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "classes/neutronWeights.h"
#include "tests/testData.h"
#include <gtest/gtest.h>

namespace UnitTest
{
TEST(NeutronWeightsTest, Simple)
{
    SmallMolecules molecules;
    NeutronWeights nwts;

    // Basic population of ten molecules with two atoms of the same type (N2)
    nwts.addIsotopologue(&molecules.N2(), 10, molecules.N2().naturalIsotopologue(), 1.0);
    nwts.createFromIsotopologues({});
    EXPECT_EQ(20, nwts.atomTypes().atomTypeData(molecules.atN())->get().population());
    EXPECT_NEAR(1.0, nwts.atomTypes().atomTypeData(molecules.atN())->get().fraction(), 1.0e-6);
    EXPECT_NEAR(pow(Sears91::boundCoherent(Sears91::N_Natural), 2) / 100.0, nwts.boundCoherentSquareOfAverage(), 1.0e-6);

    // Adding more N2 natural isotopologue shouldn't make any difference - the species already exists, so just the
    // relative weight of the isotopologue will be updated, and this will be normalised back to 1.0.
    nwts.addIsotopologue(&molecules.N2(), 10, molecules.N2().naturalIsotopologue(), 50.0);
    nwts.createFromIsotopologues({});
    EXPECT_EQ(20, nwts.atomTypes().atomTypeData(molecules.atN())->get().population());
    EXPECT_NEAR(1.0, nwts.atomTypes().atomTypeData(molecules.atN())->get().fraction(), 1.0e-6);
    EXPECT_NEAR(pow(Sears91::boundCoherent(Sears91::N_Natural), 2) / 100.0, nwts.boundCoherentSquareOfAverage(), 1.0e-6);
}

TEST(NeutronWeightsTest, Water)
{
    SmallMolecules molecules;
    NeutronWeights nwts;

    nwts.addIsotopologue(&molecules.H2O(), 1, molecules.H2O().naturalIsotopologue(), 1.0);
    nwts.createFromIsotopologues({});
    EXPECT_EQ(1, nwts.atomTypes().atomTypeData(molecules.atOW())->get().population());
    EXPECT_EQ(2, nwts.atomTypes().atomTypeData(molecules.atHW())->get().population());
    EXPECT_NEAR(1.0 / 3.0, nwts.atomTypes().atomTypeData(molecules.atOW())->get().fraction(), 1.0e-6);
    EXPECT_NEAR(2.0 / 3.0, nwts.atomTypes().atomTypeData(molecules.atHW())->get().fraction(), 1.0e-6);
    EXPECT_NEAR(
        pow((Sears91::boundCoherent(Sears91::O_Natural) / 3.0) + (Sears91::boundCoherent(Sears91::H_Natural) * 2.0 / 3.0), 2) /
            100.0,
        nwts.boundCoherentSquareOfAverage(), 1.0e-6);
}

TEST(NeutronWeightsTest, D2O)
{
    SmallMolecules molecules;
    NeutronWeights nwts;

    nwts.addIsotopologue(&molecules.H2O(), 1, molecules.D2O(), 1.0);
    nwts.createFromIsotopologues({});
    EXPECT_EQ(1, nwts.atomTypes().atomTypeData(molecules.atOW())->get().population());
    EXPECT_EQ(2, nwts.atomTypes().atomTypeData(molecules.atHW())->get().population());
    EXPECT_NEAR(1.0 / 3.0, nwts.atomTypes().atomTypeData(molecules.atOW())->get().fraction(), 1.0e-6);
    EXPECT_NEAR(2.0 / 3.0, nwts.atomTypes().atomTypeData(molecules.atHW())->get().fraction(), 1.0e-6);
    EXPECT_NEAR(
        pow((Sears91::boundCoherent(Sears91::O_Natural) / 3.0) + (Sears91::boundCoherent(Sears91::H_2) * 2.0 / 3.0), 2) / 100.0,
        nwts.boundCoherentSquareOfAverage(), 1.0e-6);
}

TEST(NeutronWeightsTest, NullWater)
{
    SmallMolecules molecules;
    NeutronWeights nwts;

    auto ratio = fabs(Sears91::boundCoherent(Sears91::H_2) / Sears91::boundCoherent(Sears91::H_Natural));
    nwts.addIsotopologue(&molecules.H2O(), 1000, molecules.H2O().naturalIsotopologue(), ratio);
    nwts.addIsotopologue(&molecules.H2O(), 0, molecules.D2O(), 1.0);
    nwts.createFromIsotopologues({});
    EXPECT_EQ(1000, nwts.atomTypes().atomTypeData(molecules.atOW())->get().population());
    EXPECT_EQ(2000, nwts.atomTypes().atomTypeData(molecules.atHW())->get().population());
    EXPECT_NEAR(1.0 / 3.0, nwts.atomTypes().atomTypeData(molecules.atOW())->get().fraction(), 1.0e-6);
    EXPECT_NEAR(2.0 / 3.0, nwts.atomTypes().atomTypeData(molecules.atHW())->get().fraction(), 1.0e-6);
    EXPECT_NEAR(pow(Sears91::boundCoherent(Sears91::O_Natural) / 3.0, 2) / 100.0, nwts.boundCoherentSquareOfAverage(), 1.0e-6);

    // Making the H atomtype exchangeable should make no difference
    nwts.createFromIsotopologues({molecules.atHW()});
    EXPECT_NEAR(pow(Sears91::boundCoherent(Sears91::O_Natural) / 3.0, 2) / 100.0, nwts.boundCoherentSquareOfAverage(), 1.0e-6);
}

TEST(NeutronWeightsTest, Mix)
{
    SmallMolecules molecules;
    NeutronWeights nwts;

    nwts.addIsotopologue(&molecules.H2O(), 1, molecules.H2O().naturalIsotopologue(), 1.0);
    nwts.addIsotopologue(&molecules.N2(), 1, molecules.N2().naturalIsotopologue(), 1.0);
    nwts.addIsotopologue(&molecules.N2(), 1, molecules.N2A15(), 1.0);
    nwts.createFromIsotopologues({});
    EXPECT_EQ(1, nwts.atomTypes().atomTypeData(molecules.atOW())->get().population());
    EXPECT_EQ(2, nwts.atomTypes().atomTypeData(molecules.atHW())->get().population());
    EXPECT_EQ(2, nwts.atomTypes().atomTypeData(molecules.atN())->get().population());
    EXPECT_NEAR(1.0 / 5.0, nwts.atomTypes().atomTypeData(molecules.atOW())->get().fraction(), 1.0e-6);
    EXPECT_NEAR(2.0 / 5.0, nwts.atomTypes().atomTypeData(molecules.atHW())->get().fraction(), 1.0e-6);
    EXPECT_NEAR(2.0 / 5.0, nwts.atomTypes().atomTypeData(molecules.atN())->get().fraction(), 1.0e-6);
    EXPECT_NEAR(pow((Sears91::boundCoherent(Sears91::O_Natural) / 5.0) +
                        (Sears91::boundCoherent(Sears91::H_Natural) * 2.0 / 5.0) +
                        (Sears91::boundCoherent(Sears91::N_Natural) / 5.0) + (Sears91::boundCoherent(Sears91::N_15) / 5.0),
                    2) /
                    100.0,
                nwts.boundCoherentSquareOfAverage(), 1.0e-6);
}

} // namespace UnitTest
