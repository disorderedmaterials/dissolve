// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

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

/*
 * Tests against energies calculated with DL_POLY Classic 2.19
 */

TEST_F(EnergyModuleTest, DLPOLYWater3000Full)
{
    ASSERT_NO_THROW(systemTest.setUp("dissolve/input/energyForce-water3000.txt"));
    systemTest.setModuleEnabled("Forces01", false);
    ASSERT_TRUE(systemTest.dissolve().iterate(1));

    // Full interatomic energy: 1770.1666370083758 LJ + -29163.384451743802 Coulomb (LJ correction accounted for)
    auto &interEnergy = systemTest.dissolve().processingModuleData().value<Data1D>("Energy01//Bulk//PairPotential");
    EXPECT_TRUE(systemTest.checkDouble("interatomic energy", interEnergy.values().back(), -27393.217815, 4.5e-2));

    // Full intramolecular energy
    auto &intraEnergy = systemTest.dissolve().processingModuleData().value<Data1D>("Energy01//Bulk//Bound");
    EXPECT_TRUE(systemTest.checkDouble("intramolecular energy", intraEnergy.values().back(), 5584.45180873, 3.5e-3));
}

TEST_F(EnergyModuleTest, DLPOLYWater3000VanDerWaals)
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

    auto &interEnergy = systemTest.dissolve().processingModuleData().value<Data1D>("Energy01//Bulk//PairPotential");
    EXPECT_TRUE(
        systemTest.checkDouble("interatomic van der Waals energy", interEnergy.values().back(), 1770.1666370083758, 6.0e-2));
}

TEST_F(EnergyModuleTest, DLPOLYWater3000Electrostatics)
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
    auto &interEnergy = systemTest.dissolve().processingModuleData().value<Data1D>("Energy01//Bulk//PairPotential");

    // Shifted coulomb sum
    EXPECT_TRUE(systemTest.checkDouble("shifted coulomb energy", interEnergy.values().back(), -29163.384451743802, 1.1e-2));

    // Straight Coulomb sum (no truncation)
    PairPotential::setCoulombTruncationScheme(PairPotential::CoulombTruncationScheme::NoCoulombTruncation);
    ASSERT_TRUE(systemTest.dissolve().iterate(1));
    EXPECT_TRUE(systemTest.checkDouble("straight coulomb energy", interEnergy.values().back(), -34173.942995962962, 1.0e-2));
}

TEST_F(EnergyModuleTest, DLPOLYWater3000Bound)
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
    auto &intraEnergy = systemTest.dissolve().processingModuleData().value<Data1D>("Energy01//Bulk//Bound");

    EXPECT_TRUE(systemTest.checkDouble("bound term", intraEnergy.values().back(), 5584.45180873, 3.5e-3));
}

TEST_F(EnergyModuleTest, DLPOLYHexane1Full)
{
    ASSERT_NO_THROW(systemTest.setUp("dissolve/input/energyForce-hexane1.txt"));
    systemTest.setModuleEnabled("Forces01", false);
    ASSERT_TRUE(systemTest.dissolve().iterate(1));

    // Full interatomic energy: LJ (3.504968) + Coul (10.081520) - LJCorrect (-0.00501830)
    auto &interEnergy = systemTest.dissolve().processingModuleData().value<Data1D>("Energy01//Liquid//PairPotential");
    EXPECT_TRUE(systemTest.checkDouble("interatomic energy", interEnergy.values().back(), 14.32518630, 5.0e-4));

    // Full intramolecular energy
    auto &intraEnergy = systemTest.dissolve().processingModuleData().value<Data1D>("Energy01//Liquid//Bound");
    EXPECT_TRUE(systemTest.checkDouble("intramolecular energy", intraEnergy.values().back(), 71.59299, 6.0e-4));
}

TEST_F(EnergyModuleTest, DLPOLYHexane2Full)
{
    ASSERT_NO_THROW(systemTest.setUp("dissolve/input/energyForce-hexane2.txt"));
    systemTest.setModuleEnabled("Forces01", false);
    ASSERT_TRUE(systemTest.dissolve().iterate(1));

    // Full interatomic energy: LJ (5.2003) + Coul (21.523) - LJCorrect (-0.0200732)
    auto &interEnergy = systemTest.dissolve().processingModuleData().value<Data1D>("Energy01//Liquid//PairPotential");
    EXPECT_TRUE(systemTest.checkDouble("interatomic energy", interEnergy.values().back(), 26.743373, 5.0e-4));

    // Full intramolecular energy: Bond (33.91651) + Angle (72.22392) + Torsion (24.49599)
    auto &intraEnergy = systemTest.dissolve().processingModuleData().value<Data1D>("Energy01//Liquid//Bound");
    EXPECT_TRUE(systemTest.checkDouble("intramolecular energy", intraEnergy.values().back(), 130.636420, 6.0e-4));
}

TEST_F(EnergyModuleTest, DLPOLYHexane200Full)
{
    ASSERT_NO_THROW(systemTest.setUp("dissolve/input/energyForce-hexane200.txt"));
    systemTest.setModuleEnabled("Forces01", false);
    ASSERT_TRUE(systemTest.dissolve().iterate(1));

    // Full interatomic energy:  LJ (-5124.720) + Coul (2020.063) - LJ correct (-200.732)
    auto &interEnergy = systemTest.dissolve().processingModuleData().value<Data1D>("Energy01//Liquid//PairPotential");
    EXPECT_TRUE(systemTest.checkDouble("interatomic energy", interEnergy.values().back(), -2903.925, 5.0e-2));

    // Full intramolecular energy
    auto &intraEnergy = systemTest.dissolve().processingModuleData().value<Data1D>("Energy01//Liquid//Bound");
    EXPECT_TRUE(systemTest.checkDouble("intramolecular energy", intraEnergy.values().back(), 11488.122, 6.0e-4));
}

TEST_F(EnergyModuleTest, DLPOLYHexane200Bound)
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
    auto &intraEnergy = systemTest.dissolve().processingModuleData().value<Data1D>("Energy01//Liquid//Bound");
    EXPECT_TRUE(systemTest.checkDouble("intramolecular energy", intraEnergy.values().back(), 11488.122, 6.0e-4));
}

TEST_F(EnergyModuleTest, DLPOLYHexane200Torsions)
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

    auto &intraEnergy = systemTest.dissolve().processingModuleData().value<Data1D>("Energy01//Liquid//Bound");
    EXPECT_TRUE(systemTest.checkDouble("torsion energy", intraEnergy.values().back(), 2173.978, 5.0e-4));
}

TEST_F(EnergyModuleTest, DLPOLYBenzene181Full)
{
    ASSERT_NO_THROW(systemTest.setUp("dissolve/input/energyForce-benzene181.txt"));
    systemTest.setModuleEnabled("Forces01", false);
    ASSERT_TRUE(systemTest.dissolve().iterate(1));

    // Full interatomic energy: -5.911071e+02   # 5.612389E+02 (elec) - 1.334653E+03 (VDW) + 0.182307E+03 (VDW LR correction)
    auto &interEnergy = systemTest.dissolve().processingModuleData().value<Data1D>("Energy01//Bulk//PairPotential");
    EXPECT_TRUE(systemTest.checkDouble("interatomic energy", interEnergy.values().back(), -5.911071e2, 3.5e-2));

    // Full intramolecular energy
    auto &intraEnergy = systemTest.dissolve().processingModuleData().value<Data1D>("Energy01//Bulk//Bound");
    EXPECT_TRUE(systemTest.checkDouble("intramolecular energy", intraEnergy.values().back(), 5.901646e+03, 3.5e-3));
}

TEST_F(EnergyModuleTest, DLPOLYBenzene181VanDerWaals)
{
    ASSERT_NO_THROW(systemTest.setUp("dissolve/input/energyForce-benzene181.txt",
                                     [](Dissolve &D, CoreData &C)
                                     {
                                         D.setAtomTypeChargeSource(true);
                                         D.setAutomaticChargeSource(false);
                                         for (auto &b : C.masterBonds())
                                             b->setInteractionForm(BondFunctions::Form::None);
                                         for (auto &a : C.masterAngles())
                                             a->setInteractionForm(AngleFunctions::Form::None);
                                         for (auto &t : C.masterTorsions())
                                             t->setInteractionForm(TorsionFunctions::Form::None);
                                     }));
    systemTest.setModuleEnabled("Forces01", false);
    ASSERT_TRUE(systemTest.dissolve().iterate(1));

    // Full van der Waals Energy: -1.152346e+03   # -1.334653E+03 (full) + 0.182307E+03 (LR correction)
    auto &interEnergy = systemTest.dissolve().processingModuleData().value<Data1D>("Energy01//Bulk//PairPotential");
    EXPECT_TRUE(systemTest.checkDouble("interatomic van der Waals energy", interEnergy.values().back(), -1.152346e+03, 3.5e-2));
}

TEST_F(EnergyModuleTest, DLPOLYBenzene181Electrostatics)
{
    ASSERT_NO_THROW(systemTest.setUp("dissolve/input/energyForce-benzene181.txt",
                                     [](Dissolve &D, CoreData &C)
                                     {
                                         C.atomType(0)->interactionPotential().parseParameters("epsilon=0.0 sigma=0.0");
                                         C.atomType(1)->interactionPotential().parseParameters("epsilon=0.0 sigma=0.0");
                                         for (auto &b : C.masterBonds())
                                             b->setInteractionForm(BondFunctions::Form::None);
                                         for (auto &a : C.masterAngles())
                                             a->setInteractionForm(AngleFunctions::Form::None);
                                         for (auto &t : C.masterTorsions())
                                             t->setInteractionForm(TorsionFunctions::Form::None);
                                     }));
    systemTest.setModuleEnabled("Forces01", false);
    ASSERT_TRUE(systemTest.dissolve().iterate(1));
    auto &interEnergy = systemTest.dissolve().processingModuleData().value<Data1D>("Energy01//Bulk//PairPotential");

    // Shifted coulomb sum
    EXPECT_TRUE(systemTest.checkDouble("shifted coulomb energy", interEnergy.values().back(), 5.612389E+02, 1.8e-4));
}

TEST_F(EnergyModuleTest, DLPOLYBenzene181Bound)
{
    ASSERT_NO_THROW(systemTest.setUp("dissolve/input/energyForce-benzene181.txt",
                                     [](Dissolve &D, CoreData &C)
                                     {
                                         D.setAtomTypeChargeSource(true);
                                         C.atomType(0)->interactionPotential().parseParameters("epsilon=0.0 sigma=0.0");
                                         C.atomType(1)->interactionPotential().parseParameters("epsilon=0.0 sigma=0.0");
                                     }));
    systemTest.setModuleEnabled("Forces01", false);
    ASSERT_TRUE(systemTest.dissolve().iterate(1));
    auto &intraEnergy = systemTest.dissolve().processingModuleData().value<Data1D>("Energy01//Bulk//Bound");

    EXPECT_TRUE(systemTest.checkDouble("bound term", intraEnergy.values().back(), 5.901646e+03, 3.5e-3));
}

// Tests against energies calculated with MOSCITO 4.180.

TEST_F(EnergyModuleTest, MoscitoPOETorsions)
{
    ASSERT_NO_THROW(systemTest.setUp("dissolve/input/energyForce-POE.txt"));
    systemTest.setModuleEnabled("Forces01", false);
    ASSERT_TRUE(systemTest.dissolve().iterate(1));

    // Intramolecular energy: 183.4801   # (2.866876 per molecule) * 64 molecules
    auto &intraEnergy = systemTest.dissolve().processingModuleData().value<Data1D>("Energy01//POE//Bound");
    EXPECT_TRUE(systemTest.checkDouble("torsion energy", intraEnergy.values().back(), 183.4801, 1.0e-2));
}

TEST_F(EnergyModuleTest, MoscitoPy4OHNTf2Torsions)
{
    ASSERT_NO_THROW(systemTest.setUp("dissolve/input/energyForce-Py4OH-NTf2.txt"));
    systemTest.setModuleEnabled("Forces01", false);
    ASSERT_TRUE(systemTest.dissolve().iterate(1));

    // Intramolecular energy: 51.050222   # (25.525111 per molecule) * 2 molecules
    auto &intraEnergy = systemTest.dissolve().processingModuleData().value<Data1D>("Energy01//Py4OH-NTf2//Bound");
    EXPECT_TRUE(systemTest.checkDouble("torsion energy", intraEnergy.values().back(), 51.050222, 2.0e-5));
}

TEST_F(EnergyModuleTest, MoscitoPy4OHNTf2Impropers)
{
    ASSERT_NO_THROW(systemTest.setUp("dissolve/input/energyForce-Py4OH-NTf2-impropers.txt"));
    systemTest.setModuleEnabled("Forces01", false);
    ASSERT_TRUE(systemTest.dissolve().iterate(1));

    // Intramolecular energy: 0.055228   # (0.027614 per molecule) * 2 molecules
    auto &intraEnergy = systemTest.dissolve().processingModuleData().value<Data1D>("Energy01//Py4OH-NTf2//Bound");
    EXPECT_TRUE(systemTest.checkDouble("improper energy", intraEnergy.values().back(), 0.055228, 2.0e-5));
}

TEST_F(EnergyModuleTest, MoscitoPy5NTf2Torsions)
{
    ASSERT_NO_THROW(systemTest.setUp("dissolve/input/energyForce-Py5-NTf2.txt"));
    systemTest.setModuleEnabled("Forces01", false);
    ASSERT_TRUE(systemTest.dissolve().iterate(1));

    // Intramolecular energy: 39.29711  # (19.648555 per molecule) * 2 molecules
    auto &intraEnergy = systemTest.dissolve().processingModuleData().value<Data1D>("Energy01//Py5-NTf2//Bound");
    EXPECT_TRUE(systemTest.checkDouble("torsion energy", intraEnergy.values().back(), 39.29711, 5.0e-5));
}

TEST_F(EnergyModuleTest, MoscitoPy5NTf2Impropers)
{
    ASSERT_NO_THROW(systemTest.setUp("dissolve/input/energyForce-Py5-NTf2-impropers.txt"));
    systemTest.setModuleEnabled("Forces01", false);
    ASSERT_TRUE(systemTest.dissolve().iterate(1));

    // Intramolecular energy: 0.34961  # (0.174805 per molecule) * 2 molecules
    auto &intraEnergy = systemTest.dissolve().processingModuleData().value<Data1D>("Energy01//Py5-NTf2//Bound");
    EXPECT_TRUE(systemTest.checkDouble("improper energy", intraEnergy.values().back(), 0.34961, 5.0e-5));
}

} // namespace UnitTest
