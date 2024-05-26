// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "classes/coreData.h"
#include "classes/species.h"
#include "classes/potentialMap.h"
#include <gtest/gtest.h>

namespace UnitTest
{
class PairPotentialsScaleFactorsTest : public ::testing::Test
{
    public:
    PairPotentialsScaleFactorsTest()
    {
        // Set up atom types
        auto atC1 = coreData_.addAtomType(Elements::C);
        atC1->setCharge(0.1);
        atC1->interactionPotential().setFormAndParameters(ShortRangeFunctions::Form::LennardJones, "epsilon=1.0 sigma=3.0");
        auto atC2 = coreData_.addAtomType(Elements::C);
        atC2->setCharge(-0.1);
        atC2->interactionPotential().setFormAndParameters(ShortRangeFunctions::Form::LennardJones, "epsilon=1.0 sigma=3.0");
        std::vector<PairPotential::Definition> pairPotentials;
        pairPotentials.emplace_back(atC1, atC1, std::make_unique<PairPotential>("C1", "C1"));
        pairPotentials.emplace_back(atC1, atC2, std::make_unique<PairPotential>("C1", "C2"));
        pairPotentials.emplace_back(atC2, atC2, std::make_unique<PairPotential>("C2", "C2"));
        
        // Create PotentialMap
        potentialMap_.initialise(coreData_.atomTypes(), pairPotentials, 15.0);
        
        // Create Species
        torsion_.addAtom(Elements::C, {-1.390000, 0.000000, 0.000000}, -0.2, atC1);
        torsion_.addAtom(Elements::C, {-0.695000, 1.203775, 0.000000}, 0.2, atC2);
        torsion_.addAtom(Elements::C, {0.695000, 1.203775, 0.000000}, 0.2, atC2);
        torsion_.addAtom(Elements::C, {1.390000, 0.000000, 0.000000}, -0.2, atC1);
        torsion_.addMissingBonds();
    }

    protected:
    CoreData coreData_;
    PotentialMap potentialMap_;
    Species torsion_;
};

TEST_F(PairPotentialsScaleFactorsTest, NoneForm)
{

}

} // namespace UnitTest
