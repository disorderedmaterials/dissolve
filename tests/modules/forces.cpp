// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "io/import/forces.h"
#include "classes/configuration.h"
#include "main/dissolve.h"
#include "modules/forces/forces.h"
#include "tests/testData.h"
#include <gtest/gtest.h>
#include <vector>

namespace UnitTest
{
class ForcesModuleTest : public ::testing::Test
{
    protected:
    DissolveSystemTest systemTest;

    void testForces(const std::vector<Vec3<double>> &A, const std::vector<Vec3<double>> &B, double tolerance)
    {
        ASSERT_EQ(A.size(), B.size());
        for (auto n = 0; n < A.size(); ++n)
        {
            EXPECT_NEAR(A[n].x, B[n].x, tolerance);
            EXPECT_NEAR(A[n].y, B[n].y, tolerance);
            EXPECT_NEAR(A[n].z, B[n].z, tolerance);
        }
    }
    void testForces(const std::vector<Vec3<double>> &A, ForceImportFileFormat externalForces, double tolerance)
    {
        std::vector<Vec3<double>> B(A.size());
        ASSERT_TRUE(externalForces.importData(B));
        testForces(A, B, tolerance);
    }
};

TEST_F(ForcesModuleTest, Water3000Full)
{
    ASSERT_NO_THROW(systemTest.setUp("dissolve/energyForce-water3000.txt"));
    systemTest.setModuleEnabled("Energy01", false);
    ASSERT_TRUE(systemTest.dissolve().iterate(1));

    auto &forces = systemTest.dissolve().processingModuleData().value<std::vector<Vec3<double>>>("Forces01//Bulk//Forces");
    testForces(forces, {"dlpoly/water3000_energyForce/full.REVCON", ForceImportFileFormat::ForceImportFormat::DLPOLY}, 0.09);
}


TEST_F(ForcesModuleTest, Water3000VanDerWaals)
{
    ASSERT_NO_THROW(systemTest.setUp("dissolve/energyForce-water3000.txt",
                                     [](Dissolve &D, CoreData &C)
                                     {
                                         D.setAtomTypeChargeSource(true);
                                         D.setAutomaticChargeSource(false);
                                         C.masterBonds().front()->setInteractionParameters("k=0.0 eq=1.0");
                                         C.masterAngles().front()->setInteractionParameters("k=0.0 eq=1.0");
                                     }));
    systemTest.setModuleEnabled("Energy01", false);
    ASSERT_TRUE(systemTest.dissolve().iterate(1));

    auto &forces = systemTest.dissolve().processingModuleData().value<std::vector<Vec3<double>>>("Forces01//Bulk//Forces");
    testForces(forces, {"dlpoly/water3000_energyForce/vdw.REVCON", ForceImportFileFormat::ForceImportFormat::DLPOLY}, 8.0e-2);
}

TEST_F(ForcesModuleTest, Water3000Electrostatics)
{
    ASSERT_NO_THROW(systemTest.setUp("dissolve/energyForce-water3000.txt",
                                     [](Dissolve &D, CoreData &C)
                                     {
                                         C.atomType(0)->interactionPotential().parseParameters("epsilon=0.0 sigma=0.0");
                                         C.atomType(1)->interactionPotential().parseParameters("epsilon=0.0 sigma=0.0");
                                         C.masterBonds().front()->setInteractionParameters("k=0.0 eq=1.0");
                                         C.masterAngles().front()->setInteractionParameters("k=0.0 eq=1.0");
                                     }));
    systemTest.setModuleEnabled("Energy01", false);
    ASSERT_TRUE(systemTest.dissolve().iterate(1));

    // Shifted coulomb sum
    auto shiftedForces = systemTest.dissolve().processingModuleData().value<std::vector<Vec3<double>>>("Forces01//Bulk//Forces");
    testForces(shiftedForces, {"dlpoly/water3000_energyForce/shifted.REVCON", ForceImportFileFormat::ForceImportFormat::DLPOLY}, 1.7e-2);

    // Straight Coulomb sum (no truncation)
    PairPotential::setCoulombTruncationScheme(PairPotential::CoulombTruncationScheme::NoCoulombTruncation);
    ASSERT_TRUE(systemTest.dissolve().iterate(1));
    auto &coulombForces = systemTest.dissolve().processingModuleData().value<std::vector<Vec3<double>>>("Forces01//Bulk//Forces");
    testForces(coulombForces, {"dlpoly/water3000_energyForce/coulomb.REVCON", ForceImportFileFormat::ForceImportFormat::DLPOLY}, 5.0e-3);
}

TEST_F(ForcesModuleTest, Water3000Bound)
{
    ASSERT_NO_THROW(systemTest.setUp("dissolve/energyForce-water3000.txt",
                                     [](Dissolve &D, CoreData &C)
                                     {
                                         D.setAtomTypeChargeSource(true);
                                         C.atomType(0)->interactionPotential().parseParameters("epsilon=0.0 sigma=0.0");
                                         C.atomType(1)->interactionPotential().parseParameters("epsilon=0.0 sigma=0.0");
                                     }));
    systemTest.setModuleEnabled("Energy01", false);
    ASSERT_TRUE(systemTest.dissolve().iterate(1));

    auto shiftedForces = systemTest.dissolve().processingModuleData().value<std::vector<Vec3<double>>>("Forces01//Bulk//Forces");
    testForces(shiftedForces, {"dlpoly/water3000_energyForce/intra.REVCON", ForceImportFileFormat::ForceImportFormat::DLPOLY}, 2.5e-2);
}

TEST_F(ForcesModuleTest, Hexane1Full)
{
    ASSERT_NO_THROW(systemTest.setUp("dissolve/energyForce-hexane1.txt"));
    systemTest.setModuleEnabled("Energy01", false);
    ASSERT_TRUE(systemTest.dissolve().iterate(1));

    auto &forces = systemTest.dissolve().processingModuleData().value<std::vector<Vec3<double>>>("Forces01//Liquid//Forces");
    testForces(forces, {"dlpoly/hexane1/REVCON", ForceImportFileFormat::ForceImportFormat::DLPOLY}, 0.03);
}

TEST_F(ForcesModuleTest, Hexane2Full)
{
    ASSERT_NO_THROW(systemTest.setUp("dissolve/energyForce-hexane2.txt"));
    systemTest.setModuleEnabled("Energy01", false);
    ASSERT_TRUE(systemTest.dissolve().iterate(1));

    auto &forces = systemTest.dissolve().processingModuleData().value<std::vector<Vec3<double>>>("Forces01//Liquid//Forces");
    testForces(forces, {"dlpoly/hexane2/REVCON", ForceImportFileFormat::ForceImportFormat::DLPOLY}, 3.0e-2);
}

TEST_F(ForcesModuleTest, Hexane200Full)
{
    ASSERT_NO_THROW(systemTest.setUp("dissolve/energyForce-hexane200.txt"));
    systemTest.setModuleEnabled("Energy01", false);
    auto *forcesModule = systemTest.getModule<ForcesModule>("Forces01");
    forcesModule->keywords().set("TestThreshold", 2.0e-5);
    ASSERT_TRUE(systemTest.dissolve().iterate(1));

    auto &forces = systemTest.dissolve().processingModuleData().value<std::vector<Vec3<double>>>("Forces01//Liquid//Forces");
    testForces(forces, {"dlpoly/hexane200/full.REVCON", ForceImportFileFormat::ForceImportFormat::DLPOLY}, 0.2);
}

TEST_F(ForcesModuleTest, Hexane200Unbound)
{
    ASSERT_NO_THROW(systemTest.setUp("dissolve/energyForce-hexane200.txt",
                                     [](Dissolve &D, CoreData &C)
                                     {
                                         for (auto b : C.masterBonds())
                                             b->setInteractionParameters("k=0.0 eq=0.0");
                                         for (auto a : C.masterAngles())
                                             a->setInteractionParameters("k=0.0 eq=0.0");
                                         for (auto t : C.masterTorsions())
                                             t->setInteractionParameters("k1=0.0 k2=0.0 k3=0.0");
                                     }));
    systemTest.setModuleEnabled("Energy01", false);
    auto *forcesModule = systemTest.getModule<ForcesModule>("Forces01");
    forcesModule->keywords().set("TestThreshold", 5.0e-2);
    ASSERT_TRUE(systemTest.dissolve().iterate(1));

    auto &forces = systemTest.dissolve().processingModuleData().value<std::vector<Vec3<double>>>("Forces01//Liquid//Forces");
    testForces(forces, {"dlpoly/hexane200/unbound.REVCON", ForceImportFileFormat::ForceImportFormat::DLPOLY}, 0.3);
}

TEST_F(ForcesModuleTest, Hexane200Bound)
{
    ASSERT_NO_THROW(systemTest.setUp("dissolve/energyForce-hexane200.txt",
                                     [](Dissolve &D, CoreData &C)
                                     {
                                         for (auto at : C.atomTypes())
                                             at->interactionPotential().parseParameters("epsilon=0.0 sigma=0.0");
                                         D.setAtomTypeChargeSource(true);
                                         D.setAutomaticChargeSource(false);
                                     }));
    systemTest.setModuleEnabled("Energy01", false);
    ASSERT_TRUE(systemTest.dissolve().iterate(1));

    auto &forces = systemTest.dissolve().processingModuleData().value<std::vector<Vec3<double>>>("Forces01//Liquid//Forces");
    testForces(forces, {"dlpoly/hexane200/bound.REVCON", ForceImportFileFormat::ForceImportFormat::DLPOLY}, 2.5e-4);
}

} // namespace UnitTest
