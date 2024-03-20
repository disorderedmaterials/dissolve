// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "tests/testData.h"
#include <gtest/gtest.h>
#include <vector>

namespace UnitTest
{
class PairPotentialOverridesTest : public ::testing::Test
{
    protected:
    DissolveSystemTest systemTest;
};

// Calculate the van der Waals energy of our water test system 3000
TEST_F(PairPotentialOverridesTest, Water3000)
{
    const auto expectedVanDerWaalsEnergy = 1770.1666370083758;
    ASSERT_NO_THROW(systemTest.setUp("dissolve/input/energyForce-water3000.txt",
                                     [](Dissolve &D, CoreData &C)
                                     {
                                         D.setAtomTypeChargeSource(true);
                                         D.setAutomaticChargeSource(false);
                                         C.masterBonds().front()->setInteractionParameters("k=0.0 eq=1.0");
                                         C.masterAngles().front()->setInteractionParameters("k=0.0 eq=1.0");
                                     }));

    // Disable all modules except Energy01
    systemTest.disableAllModules("Energy01");
    ASSERT_TRUE(systemTest.dissolve().iterate(1));

    auto &interEnergy = systemTest.dissolve().processingModuleData().value<Data1D>("Energy01//Bulk//PairPotential");
    EXPECT_TRUE(systemTest.checkDouble("interatomic van der Waals energy", interEnergy.values().back(),
                                       expectedVanDerWaalsEnergy, 6.0e-2));

    // Get the two atom types
    auto hw = systemTest.coreData().findAtomType("HW");
    ASSERT_TRUE(hw);
    auto ow = systemTest.coreData().findAtomType("OW");
    ASSERT_TRUE(ow);

    // Set the potentials to zero and check that the new energy is zero
    EXPECT_TRUE(ow->interactionPotential().parseParameters("epsilon=0.0 sigma=0.0"));
    EXPECT_TRUE(systemTest.dissolve().regeneratePairPotentials());
    ASSERT_TRUE(systemTest.dissolve().iterate(1));
    EXPECT_TRUE(systemTest.checkDouble("zeroed van der Waals energy", interEnergy.values().back(), 0.0, 6.0e-2));

    // Reinstate our only potential (OW-OW) with an override ("Off" to begin with)
    auto *owOverride = systemTest.dissolve().addPairPotentialOverride(
        "OW", "OW", PairPotentialOverride::PairPotentialOverrideType::Off,
        {ShortRangeFunctions::Form::LennardJones, "epsilon=0.6503 sigma=3.165492"});
    EXPECT_TRUE(systemTest.dissolve().regeneratePairPotentials());
    ASSERT_TRUE(systemTest.dissolve().iterate(1));
    EXPECT_TRUE(systemTest.checkDouble("overridden (Off) van der Waals energy", interEnergy.values().back(), 0.0, 6.0e-2));

    // Turn it on to Add
    owOverride->setType(PairPotentialOverride::PairPotentialOverrideType::Add);
    EXPECT_TRUE(systemTest.dissolve().regeneratePairPotentials());
    ASSERT_TRUE(systemTest.dissolve().iterate(1));
    EXPECT_TRUE(systemTest.checkDouble("overridden (Add) van der Waals energy", interEnergy.values().back(),
                                       expectedVanDerWaalsEnergy, 6.0e-2));

    // And now to Replace
    owOverride->setType(PairPotentialOverride::PairPotentialOverrideType::Replace);
    EXPECT_TRUE(systemTest.dissolve().regeneratePairPotentials());
    ASSERT_TRUE(systemTest.dissolve().iterate(1));
    EXPECT_TRUE(systemTest.checkDouble("overridden (Replace) van der Waals energy", interEnergy.values().back(),
                                       expectedVanDerWaalsEnergy, 6.0e-2));
}

} // namespace UnitTest
