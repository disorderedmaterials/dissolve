// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "tests/testData.h"
#include <gtest/gtest.h>
#include <vector>

namespace UnitTest
{
class EnergyModuleTest : public ::testing::Test
{
    protected:
    DissolveSystemTest systemTest;
};

TEST_F(EnergyModuleTest, Water3000Full)
{
    ASSERT_NO_THROW(systemTest.setUp("dissolve/input/energyForce-water3000.txt"));
    systemTest.setModuleEnabled("Forces01", false);
    ASSERT_TRUE(systemTest.dissolve().iterate(1));

    // Full interatomic energy: 1770.1666370083758 LJ + -29163.384451743802 Coulomb (LJ correction accounted for)
    auto &interEnergy = systemTest.dissolve().processingModuleData().value<Data1D>("Energy01//Bulk//Inter");
    EXPECT_TRUE(systemTest.checkDouble("interatomic energy", interEnergy.values().back(), -27393.217815, 4.5e-2));

    // Full intramolecular energy
    auto &intraEnergy = systemTest.dissolve().processingModuleData().value<Data1D>("Energy01//Bulk//Intra");
    EXPECT_TRUE(systemTest.checkDouble("intramolecular energy", intraEnergy.values().back(), 5584.45180873, 3.5e-3));
}

TEST_F(EnergyModuleTest, Water3000VanDerWaals)
{
    ASSERT_NO_THROW(systemTest.setUp("dissolve/input/energyForce-water3000.txt",
                                     [](Dissolve &D, CoreData &C)
                                     {
                                         D.setAtomTypeChargeSource(true);
                                         D.setAutomaticChargeSource(false);
                                         C.masterBonds().front()->setInteractionParameters("k=0.0 eq=1.0");
                                         C.masterAngles().front()->setInteractionParameters("k=0.0 eq=1.0");
                                     }));
    systemTest.setModuleEnabled("Forces01", false);
    ASSERT_TRUE(systemTest.dissolve().iterate(1));

    auto &interEnergy = systemTest.dissolve().processingModuleData().value<Data1D>("Energy01//Bulk//Inter");
    EXPECT_TRUE(
        systemTest.checkDouble("interatomic van der Waals energy", interEnergy.values().back(), 1770.1666370083758, 6.0e-2));
}

TEST_F(EnergyModuleTest, Water3000Electrostatics)
{
    ASSERT_NO_THROW(systemTest.setUp("dissolve/input/energyForce-water3000.txt",
                                     [](Dissolve &D, CoreData &C)
                                     {
                                         C.atomType(0)->interactionPotential().parseParameters("epsilon=0.0 sigma=0.0");
                                         C.atomType(1)->interactionPotential().parseParameters("epsilon=0.0 sigma=0.0");
                                         C.masterBonds().front()->setInteractionParameters("k=0.0 eq=1.0");
                                         C.masterAngles().front()->setInteractionParameters("k=0.0 eq=1.0");
                                     }));
    systemTest.setModuleEnabled("Forces01", false);
    ASSERT_TRUE(systemTest.dissolve().iterate(1));
    auto &interEnergy = systemTest.dissolve().processingModuleData().value<Data1D>("Energy01//Bulk//Inter");

    // Shifted coulomb sum
    EXPECT_TRUE(systemTest.checkDouble("shifted coulomb energy", interEnergy.values().back(), -29163.384451743802, 1.1e-2));

    // Straight Coulomb sum (no truncation)
    PairPotential::setCoulombTruncationScheme(PairPotential::CoulombTruncationScheme::NoCoulombTruncation);
    ASSERT_TRUE(systemTest.dissolve().iterate(1));
    EXPECT_TRUE(systemTest.checkDouble("straight coulomb energy", interEnergy.values().back(), -34173.942995962962, 1.0e-2));
}

TEST_F(EnergyModuleTest, Water3000Bound)
{
    ASSERT_NO_THROW(systemTest.setUp("dissolve/input/energyForce-water3000.txt",
                                     [](Dissolve &D, CoreData &C)
                                     {
                                         D.setAtomTypeChargeSource(true);
                                         C.atomType(0)->interactionPotential().parseParameters("epsilon=0.0 sigma=0.0");
                                         C.atomType(1)->interactionPotential().parseParameters("epsilon=0.0 sigma=0.0");
                                     }));
    systemTest.setModuleEnabled("Forces01", false);
    ASSERT_TRUE(systemTest.dissolve().iterate(1));
    auto &intraEnergy = systemTest.dissolve().processingModuleData().value<Data1D>("Energy01//Bulk//Intra");

    EXPECT_TRUE(systemTest.checkDouble("bound term", intraEnergy.values().back(), 5584.45180873, 3.5e-3));
}

TEST_F(EnergyModuleTest, Hexane1Full)
{
    ASSERT_NO_THROW(systemTest.setUp("dissolve/input/energyForce-hexane1.txt"));
    systemTest.setModuleEnabled("Forces01", false);
    ASSERT_TRUE(systemTest.dissolve().iterate(1));

    // Full interatomic energy: LJ (3.504968) + Coul (10.081520) - LJCorrect (-0.00501830)
    auto &interEnergy = systemTest.dissolve().processingModuleData().value<Data1D>("Energy01//Liquid//Inter");
    EXPECT_TRUE(systemTest.checkDouble("interatomic energy", interEnergy.values().back(), 14.32518630, 5.0e-4));

    // Full intramolecular energy
    auto &intraEnergy = systemTest.dissolve().processingModuleData().value<Data1D>("Energy01//Liquid//Intra");
    EXPECT_TRUE(systemTest.checkDouble("intramolecular energy", intraEnergy.values().back(), 71.59299, 6.0e-4));
}

TEST_F(EnergyModuleTest, Hexane2Full)
{
    ASSERT_NO_THROW(systemTest.setUp("dissolve/input/energyForce-hexane2.txt"));
    systemTest.setModuleEnabled("Forces01", false);
    ASSERT_TRUE(systemTest.dissolve().iterate(1));

    // Full interatomic energy: LJ (5.2003) + Coul (21.523) - LJCorrect (-0.0200732)
    auto &interEnergy = systemTest.dissolve().processingModuleData().value<Data1D>("Energy01//Liquid//Inter");
    EXPECT_TRUE(systemTest.checkDouble("interatomic energy", interEnergy.values().back(), 26.743373, 5.0e-4));

    // Full intramolecular energy: Bond (33.91651) + Angle (72.22392) + Torsion (24.49599)
    auto &intraEnergy = systemTest.dissolve().processingModuleData().value<Data1D>("Energy01//Liquid//Intra");
    EXPECT_TRUE(systemTest.checkDouble("intramolecular energy", intraEnergy.values().back(), 130.636420, 6.0e-4));
}

TEST_F(EnergyModuleTest, Hexane200Full)
{
    ASSERT_NO_THROW(systemTest.setUp("dissolve/input/energyForce-hexane200.txt"));
    systemTest.setModuleEnabled("Forces01", false);
    ASSERT_TRUE(systemTest.dissolve().iterate(1));

    // Full interatomic energy:  LJ (-5124.720) + Coul (2020.063) - LJ correct (-200.732)
    auto &interEnergy = systemTest.dissolve().processingModuleData().value<Data1D>("Energy01//Liquid//Inter");
    EXPECT_TRUE(systemTest.checkDouble("interatomic energy", interEnergy.values().back(), -2903.925, 5.0e-2));

    // Full intramolecular energy
    auto &intraEnergy = systemTest.dissolve().processingModuleData().value<Data1D>("Energy01//Liquid//Intra");
    EXPECT_TRUE(systemTest.checkDouble("intramolecular energy", intraEnergy.values().back(), 11488.122, 6.0e-4));
}

TEST_F(EnergyModuleTest, Hexane200Bound)
{
    ASSERT_NO_THROW(systemTest.setUp("dissolve/input/energyForce-hexane200.txt",
                                     [](Dissolve &D, CoreData &C)
                                     {
                                         for (auto at : C.atomTypes())
                                             at->interactionPotential().parseParameters("epsilon=0.0 sigma=0.0");
                                         D.setAtomTypeChargeSource(true);
                                         D.setAutomaticChargeSource(false);
                                     }));
    systemTest.setModuleEnabled("Forces01", false);
    ASSERT_TRUE(systemTest.dissolve().iterate(1));

    // Full intramolecular energy
    auto &intraEnergy = systemTest.dissolve().processingModuleData().value<Data1D>("Energy01//Liquid//Intra");
    EXPECT_TRUE(systemTest.checkDouble("intramolecular energy", intraEnergy.values().back(), 11488.122, 6.0e-4));
}

TEST_F(EnergyModuleTest, Hexane200Torsions)
{
    ASSERT_NO_THROW(systemTest.setUp("dissolve/input/energyForce-hexane200.txt",
                                     [](Dissolve &D, CoreData &C)
                                     {
                                         for (auto at : C.atomTypes())
                                             at->interactionPotential().parseParameters("epsilon=0.0 sigma=0.0");
                                         D.setAtomTypeChargeSource(true);
                                         D.setAutomaticChargeSource(false);
                                         for (auto b : C.masterBonds())
                                             b->setInteractionParameters("k=0.0 eq=0.0");
                                         for (auto a : C.masterAngles())
                                             a->setInteractionParameters("k=0.0 eq=0.0");
                                     }));
    systemTest.setModuleEnabled("Forces01", false);
    ASSERT_TRUE(systemTest.dissolve().iterate(1));

    auto &intraEnergy = systemTest.dissolve().processingModuleData().value<Data1D>("Energy01//Liquid//Intra");
    EXPECT_TRUE(systemTest.checkDouble("torsion energy", intraEnergy.values().back(), 2173.978, 5.0e-4));
}

} // namespace UnitTest
