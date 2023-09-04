// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "classes/neutronWeights.h"
#include "classes/atomType.h"
#include "classes/configuration.h"
#include "classes/isotopologue.h"
#include "classes/species.h"
#include <gtest/gtest.h>

namespace UnitTest
{
class NeutronWeightsTest : public ::testing::Test
{
    public:
    NeutronWeightsTest()
    {
        // Set up core data
        atO_ = std::make_shared<AtomType>(Elements::O);
        atO_->setName("O");
        atN_ = std::make_shared<AtomType>(Elements::N);
        atN_->setName("N");
        atH1_ = std::make_shared<AtomType>(Elements::H);
        atH1_->setName("H1");
        atH2_ = std::make_shared<AtomType>(Elements::H);
        atH2_->setName("H2");

        // Set up N2 species
        n2_.addAtom(Elements::N, {});
        n2_.addAtom(Elements::N, {1.2, 0.0, 0.0});
        n2_.atom(0).setAtomType(atN_);
        n2_.atom(1).setAtomType(atN_);
        n2_.addBond(0, 1);
        n2A15_ = n2_.addIsotopologue("N15");
        n2A15_->setAtomTypeIsotope(atN_, Sears91::N_15);

        // Set up H2 species
        h2_.addAtom(Elements::H, {});
        h2_.addAtom(Elements::H, {0.7, 0.0, 0.0});
        h2_.atom(0).setAtomType(atH1_);
        h2_.atom(1).setAtomType(atH1_);
        h2_.addBond(0, 1);
        h2A2_ = h2_.addIsotopologue("H2");
        h2A2_->setAtomTypeIsotope(atH1_, Sears91::H_2);

        // Set up H2O species
        h2o_.addAtom(Elements::H, {1.0, 0.0, 0.0});
        h2o_.addAtom(Elements::O, {});
        h2o_.addAtom(Elements::H, {cos(107.4 / DEGRAD), sin(107.4 / DEGRAD), 0.0});
        h2o_.atom(0).setAtomType(atH2_);
        h2o_.atom(1).setAtomType(atO_);
        h2o_.atom(2).setAtomType(atH2_);
        h2o_.addBond(0, 1);
        h2o_.addBond(1, 2);
        h2o_.addAngle(0, 1, 2);
        d2o_ = h2o_.addIsotopologue("D2O");
        d2o_->setAtomTypeIsotope(atH2_, Sears91::H_2);
    }

    protected:
    std::shared_ptr<AtomType> atO_, atN_, atH1_, atH2_;
    Species n2_, h2_, h2o_;
    Isotopologue *n2A15_, *h2A2_, *d2o_;
};

TEST_F(NeutronWeightsTest, Simple)
{
    NeutronWeights nwts;
    // Basic population of ten molecules with two atoms of the same type (N2)
    nwts.addIsotopologue(&n2_, 10, n2_.naturalIsotopologue(), 1.0);
    nwts.createFromIsotopologues({});
    EXPECT_EQ(20, nwts.atomTypes().atomTypeData(atN_)->get().population());
    EXPECT_NEAR(1.0, nwts.atomTypes().atomTypeData(atN_)->get().fraction(), 1.0e-6);
    EXPECT_NEAR(pow(Sears91::boundCoherent(Sears91::N_Natural), 2) / 100.0, nwts.boundCoherentSquareOfAverage(), 1.0e-6);
    // -- Adding more N2 natural isotopologue shouldn't make any difference - the species already exists, so just the
    // relative weight of the isotopologue will be updated, and this will be normalised back to 1.0.
    nwts.addIsotopologue(&n2_, 10, n2_.naturalIsotopologue(), 50.0);
    nwts.createFromIsotopologues({});
    EXPECT_EQ(20, nwts.atomTypes().atomTypeData(atN_)->get().population());
    EXPECT_NEAR(1.0, nwts.atomTypes().atomTypeData(atN_)->get().fraction(), 1.0e-6);
    EXPECT_NEAR(pow(Sears91::boundCoherent(Sears91::N_Natural), 2) / 100.0, nwts.boundCoherentSquareOfAverage(), 1.0e-6);
}

TEST_F(NeutronWeightsTest, Water)
{
    NeutronWeights nwts;
    nwts.addIsotopologue(&h2o_, 1, h2o_.naturalIsotopologue(), 1.0);
    nwts.createFromIsotopologues({});
    EXPECT_EQ(1, nwts.atomTypes().atomTypeData(atO_)->get().population());
    EXPECT_EQ(2, nwts.atomTypes().atomTypeData(atH2_)->get().population());
    EXPECT_NEAR(1.0 / 3.0, nwts.atomTypes().atomTypeData(atO_)->get().fraction(), 1.0e-6);
    EXPECT_NEAR(2.0 / 3.0, nwts.atomTypes().atomTypeData(atH2_)->get().fraction(), 1.0e-6);
    EXPECT_NEAR(
        pow((Sears91::boundCoherent(Sears91::O_Natural) / 3.0) + (Sears91::boundCoherent(Sears91::H_Natural) * 2.0 / 3.0), 2) /
            100.0,
        nwts.boundCoherentSquareOfAverage(), 1.0e-6);
}

TEST_F(NeutronWeightsTest, D2O)
{
    NeutronWeights nwts;
    nwts.addIsotopologue(&h2o_, 1, d2o_, 1.0);
    nwts.createFromIsotopologues({});
    EXPECT_EQ(1, nwts.atomTypes().atomTypeData(atO_)->get().population());
    EXPECT_EQ(2, nwts.atomTypes().atomTypeData(atH2_)->get().population());
    EXPECT_NEAR(1.0 / 3.0, nwts.atomTypes().atomTypeData(atO_)->get().fraction(), 1.0e-6);
    EXPECT_NEAR(2.0 / 3.0, nwts.atomTypes().atomTypeData(atH2_)->get().fraction(), 1.0e-6);
    EXPECT_NEAR(
        pow((Sears91::boundCoherent(Sears91::O_Natural) / 3.0) + (Sears91::boundCoherent(Sears91::H_2) * 2.0 / 3.0), 2) / 100.0,
        nwts.boundCoherentSquareOfAverage(), 1.0e-6);
}

TEST_F(NeutronWeightsTest, NullWater)
{
    NeutronWeights nwts;
    auto ratio = fabs(Sears91::boundCoherent(Sears91::H_2) / Sears91::boundCoherent(Sears91::H_Natural));
    nwts.addIsotopologue(&h2o_, 1000, h2o_.naturalIsotopologue(), ratio);
    nwts.addIsotopologue(&h2o_, 0, d2o_, 1.0);
    nwts.createFromIsotopologues({});
    EXPECT_EQ(1000, nwts.atomTypes().atomTypeData(atO_)->get().population());
    EXPECT_EQ(2000, nwts.atomTypes().atomTypeData(atH2_)->get().population());
    EXPECT_NEAR(1.0 / 3.0, nwts.atomTypes().atomTypeData(atO_)->get().fraction(), 1.0e-6);
    EXPECT_NEAR(2.0 / 3.0, nwts.atomTypes().atomTypeData(atH2_)->get().fraction(), 1.0e-6);
    EXPECT_NEAR(pow(Sears91::boundCoherent(Sears91::O_Natural) / 3.0, 2) / 100.0, nwts.boundCoherentSquareOfAverage(), 1.0e-6);
    // Making the H atomtype exchangeable should make no difference
    nwts.createFromIsotopologues({atH2_});
    EXPECT_NEAR(pow(Sears91::boundCoherent(Sears91::O_Natural) / 3.0, 2) / 100.0, nwts.boundCoherentSquareOfAverage(), 1.0e-6);
}

TEST_F(NeutronWeightsTest, Mix)
{
    NeutronWeights nwts;
    nwts.addIsotopologue(&h2o_, 1, h2o_.naturalIsotopologue(), 1.0);
    nwts.addIsotopologue(&n2_, 1, n2_.naturalIsotopologue(), 1.0);
    nwts.addIsotopologue(&n2_, 1, n2A15_, 1.0);
    nwts.createFromIsotopologues({});
    EXPECT_EQ(1, nwts.atomTypes().atomTypeData(atO_)->get().population());
    EXPECT_EQ(2, nwts.atomTypes().atomTypeData(atH2_)->get().population());
    EXPECT_EQ(2, nwts.atomTypes().atomTypeData(atN_)->get().population());
    EXPECT_NEAR(1.0 / 5.0, nwts.atomTypes().atomTypeData(atO_)->get().fraction(), 1.0e-6);
    EXPECT_NEAR(2.0 / 5.0, nwts.atomTypes().atomTypeData(atH2_)->get().fraction(), 1.0e-6);
    EXPECT_NEAR(2.0 / 5.0, nwts.atomTypes().atomTypeData(atN_)->get().fraction(), 1.0e-6);
    EXPECT_NEAR(pow((Sears91::boundCoherent(Sears91::O_Natural) / 5.0) +
                        (Sears91::boundCoherent(Sears91::H_Natural) * 2.0 / 5.0) +
                        (Sears91::boundCoherent(Sears91::N_Natural) / 5.0) + (Sears91::boundCoherent(Sears91::N_15) / 5.0),
                    2) /
                    100.0,
                nwts.boundCoherentSquareOfAverage(), 1.0e-6);
}

} // namespace UnitTest
