// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

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
};

/*
 * Tests against forces calculated with DL_POLY Classic 2.19
 */

TEST_F(ForcesModuleTest, DLPOLYBenzene181Full)
{
    ASSERT_NO_THROW_VERBOSE(systemTest.setUp("dissolve/input/energyForce-benzene181.txt"));
    systemTest.setModuleEnabled("Energy01", false);
    ASSERT_TRUE(systemTest.dissolve().iterate(1));

    systemTest.checkVec3Vector("Forces01//Bulk//Forces",
                               {"dlpoly/benzene181/benzene181-full.REVCON", ForceImportFileFormat::ForceImportFormat::DLPOLY},
                               0.15);
}

TEST_F(ForcesModuleTest, DLPOLYBenzene181VanDerWaals)
{
    ASSERT_NO_THROW_VERBOSE(systemTest.setUp("dissolve/input/energyForce-benzene181.txt",
                                             [](Dissolve &D, CoreData &C)
                                             {
                                                 PairPotential::setChargeSource(PairPotential::ChargeSource::AtomTypes);
                                                 for (auto &b : C.masterBonds())
                                                     b->setInteractionForm(BondFunctions::Form::None);
                                                 for (auto &a : C.masterAngles())
                                                     a->setInteractionForm(AngleFunctions::Form::None);
                                                 for (auto &t : C.masterTorsions())
                                                     t->setInteractionForm(TorsionFunctions::Form::None);
                                             }));
    systemTest.setModuleEnabled("Energy01", false);
    ASSERT_TRUE(systemTest.dissolve().iterate(1));

    systemTest.checkVec3Vector("Forces01//Bulk//Forces",
                               {"dlpoly/benzene181/benzene181-vdw.REVCON", ForceImportFileFormat::ForceImportFormat::DLPOLY},
                               0.12);
}

TEST_F(ForcesModuleTest, DLPOLYBenzene181Electrostatics)
{
    ASSERT_NO_THROW_VERBOSE(systemTest.setUp("dissolve/input/energyForce-benzene181.txt",
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
    systemTest.setModuleEnabled("Energy01", false);
    ASSERT_TRUE(systemTest.dissolve().iterate(1));

    // Shifted coulomb sum
    systemTest.checkVec3Vector("Forces01//Bulk//Forces",
                               {"dlpoly/benzene181/benzene181-elec.REVCON", ForceImportFileFormat::ForceImportFormat::DLPOLY},
                               3.0e-4);
}

TEST_F(ForcesModuleTest, DLPOLYBenzene181Bound)
{
    ASSERT_NO_THROW_VERBOSE(systemTest.setUp("dissolve/input/energyForce-benzene181.txt",
                                             [](Dissolve &D, CoreData &C)
                                             {
                                                 PairPotential::setChargeSource(PairPotential::ChargeSource::AtomTypes);
                                                 C.atomType(0)->interactionPotential().parseParameters("epsilon=0.0 sigma=0.0");
                                                 C.atomType(1)->interactionPotential().parseParameters("epsilon=0.0 sigma=0.0");
                                             }));
    systemTest.setModuleEnabled("Energy01", false);
    ASSERT_TRUE(systemTest.dissolve().iterate(1));

    systemTest.checkVec3Vector("Forces01//Bulk//Forces",
                               {"dlpoly/benzene181/benzene181-intra.REVCON", ForceImportFileFormat::ForceImportFormat::DLPOLY},
                               1.0e-7);
}

// Tests against energies calculated with MOSCITO 4.180.

// TEST_F(ForcesModuleTest, MoscitoPOETorsions)
// {
//     ASSERT_NO_THROW_VERBOSE(systemTest.setUp("dissolve/input/energyForce-POE.txt"));
//     systemTest.setModuleEnabled("Energy01", false);
//     ASSERT_TRUE(systemTest.dissolve().iterate(1));
//
//     systemTest.checkVec3Vector("Forces01//POE//Forces",
//                                {"moscito/poe64_torsions/torsions-final.str",
//                                ForceImportFileFormat::ForceImportFormat::Moscito}, 8.0e-2);
// }
//
// TEST_F(ForcesModuleTest, MoscitoPy4OHNTf2Torsions)
// {
//     ASSERT_NO_THROW_VERBOSE(systemTest.setUp("dissolve/input/energyForce-Py4OH-NTf2.txt"));
//     systemTest.setModuleEnabled("Energy01", false);
//     ASSERT_TRUE(systemTest.dissolve().iterate(1));
//
//     systemTest.checkVec3Vector(
//         "Forces01//Py4OH-NTf2//Forces",
//         {"moscito/py4oh_torsions/py4oh-ntf2-final.str", ForceImportFileFormat::ForceImportFormat::Moscito}, 5.0e-2);
// }
//
// TEST_F(ForcesModuleTest, MoscitoPy4OHNTf2Impropers)
// {
//     ASSERT_NO_THROW_VERBOSE(systemTest.setUp("dissolve/input/energyForce-Py4OH-NTf2-impropers.txt"));
//     systemTest.setModuleEnabled("Energy01", false);
//     ASSERT_TRUE(systemTest.dissolve().iterate(1));
//
//     systemTest.checkVec3Vector(
//         "Forces01//Py4OH-NTf2//Forces",
//         {"moscito/py4oh_impropers/py4oh-ntf2-final.str", ForceImportFileFormat::ForceImportFormat::Moscito}, 5.0e-4);
// }
//
// TEST_F(ForcesModuleTest, MoscitoPy5NTf2Torsions)
// {
//     ASSERT_NO_THROW_VERBOSE(systemTest.setUp("dissolve/input/energyForce-Py5-NTf2.txt"));
//     systemTest.setModuleEnabled("Energy01", false);
//     ASSERT_TRUE(systemTest.dissolve().iterate(1));
//
//     systemTest.checkVec3Vector("Forces01//Py5-NTf2//Forces",
//                                {"moscito/py5_torsions/py5-ntf2-final.str",
//                                ForceImportFileFormat::ForceImportFormat::Moscito}, 0.1);
// }
//
// TEST_F(ForcesModuleTest, MoscitoPy5NTf2Impropers)
// {
//     ASSERT_NO_THROW_VERBOSE(systemTest.setUp("dissolve/input/energyForce-Py5-NTf2-impropers.txt"));
//     systemTest.setModuleEnabled("Energy01", false);
//     ASSERT_TRUE(systemTest.dissolve().iterate(1));
//
//     systemTest.checkVec3Vector("Forces01//Py5-NTf2//Forces",
//                                {"moscito/py5_impropers/py5-ntf2-final.str",
//                                ForceImportFileFormat::ForceImportFormat::Moscito}, 5.0e-3);
// }

} // namespace UnitTest
