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

// Tests against energies calculated with MOSCITO 4.180.

TEST_F(ForcesModuleTest, MoscitoPOETorsions)
{
    ASSERT_NO_THROW_VERBOSE(systemTest.setUp("dissolve/input/energyForce-POE.txt"));
    systemTest.setModuleEnabled("Energy01", false);
    ASSERT_TRUE(systemTest.dissolve().iterate(1));

    systemTest.checkVec3Vector("Forces01//POE//Forces",
                               {"moscito/poe64_torsions/torsions-final.str", ForceImportFileFormat::ForceImportFormat::Moscito},
                               8.0e-2);
}

TEST_F(ForcesModuleTest, MoscitoPy4OHNTf2Torsions)
{
    ASSERT_NO_THROW_VERBOSE(systemTest.setUp("dissolve/input/energyForce-Py4OH-NTf2.txt"));
    systemTest.setModuleEnabled("Energy01", false);
    ASSERT_TRUE(systemTest.dissolve().iterate(1));

    systemTest.checkVec3Vector(
        "Forces01//Py4OH-NTf2//Forces",
        {"moscito/py4oh_torsions/py4oh-ntf2-final.str", ForceImportFileFormat::ForceImportFormat::Moscito}, 5.0e-2);
}

TEST_F(ForcesModuleTest, MoscitoPy4OHNTf2Impropers)
{
    ASSERT_NO_THROW_VERBOSE(systemTest.setUp("dissolve/input/energyForce-Py4OH-NTf2-impropers.txt"));
    systemTest.setModuleEnabled("Energy01", false);
    ASSERT_TRUE(systemTest.dissolve().iterate(1));

    systemTest.checkVec3Vector(
        "Forces01//Py4OH-NTf2//Forces",
        {"moscito/py4oh_impropers/py4oh-ntf2-final.str", ForceImportFileFormat::ForceImportFormat::Moscito}, 5.0e-4);
}

TEST_F(ForcesModuleTest, MoscitoPy5NTf2Torsions)
{
    ASSERT_NO_THROW_VERBOSE(systemTest.setUp("dissolve/input/energyForce-Py5-NTf2.txt"));
    systemTest.setModuleEnabled("Energy01", false);
    ASSERT_TRUE(systemTest.dissolve().iterate(1));

    systemTest.checkVec3Vector("Forces01//Py5-NTf2//Forces",
                               {"moscito/py5_torsions/py5-ntf2-final.str", ForceImportFileFormat::ForceImportFormat::Moscito},
                               0.1);
}

TEST_F(ForcesModuleTest, MoscitoPy5NTf2Impropers)
{
    ASSERT_NO_THROW_VERBOSE(systemTest.setUp("dissolve/input/energyForce-Py5-NTf2-impropers.txt"));
    systemTest.setModuleEnabled("Energy01", false);
    ASSERT_TRUE(systemTest.dissolve().iterate(1));

    systemTest.checkVec3Vector("Forces01//Py5-NTf2//Forces",
                               {"moscito/py5_impropers/py5-ntf2-final.str", ForceImportFileFormat::ForceImportFormat::Moscito},
                               5.0e-3);
}

} // namespace UnitTest
