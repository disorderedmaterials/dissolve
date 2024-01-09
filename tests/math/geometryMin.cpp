// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "classes/atomType.h"
#include "classes/species.h"
#include "modules/geomOpt/geomOpt.h"
#include <gtest/gtest.h>

namespace UnitTest
{

TEST(GeometryMinimisationTest, Water)
{
    // Set up CoreData
    CoreData coreData;
    auto atO = coreData.addAtomType(Elements::O);
    atO->interactionPotential().setForm(ShortRangeFunctions::Form::None);
    auto atH = coreData.addAtomType(Elements::H);
    atH->interactionPotential().setForm(ShortRangeFunctions::Form::None);
    std::vector<std::unique_ptr<PairPotential>> pairPotentials;
    pairPotentials.emplace_back(std::make_unique<PairPotential>())->setUp(atO, atO, false);
    pairPotentials.emplace_back(std::make_unique<PairPotential>())->setUp(atH, atH, false);
    pairPotentials.emplace_back(std::make_unique<PairPotential>())->setUp(atO, atH, false);
    PotentialMap potMap;
    potMap.initialise(coreData.atomTypes(), pairPotentials, 15.0);

    Species water;
    water.addAtom(Elements::H, {0.5, 0.1, 0.0});
    water.addAtom(Elements::O, {0.0, 0.0, 0.0});
    water.addAtom(Elements::H, {0.0, 0.5, -0.06});
    water.atom(0).setAtomType(atH);
    water.atom(1).setAtomType(atO);
    water.atom(2).setAtomType(atH);
    water.addBond(0, 1).setInteractionFormAndParameters(BondFunctions::Form::Harmonic, "k=4000.0 eq=1.0");
    water.addBond(1, 2).setInteractionFormAndParameters(BondFunctions::Form::Harmonic, "k=4000.0 eq=1.2");
    water.addAngle(0, 1, 2).setInteractionFormAndParameters(AngleFunctions::Form::Harmonic, "k=4000.0 eq=123.45");

    // Run the geometry optimisation
    GeometryOptimisationModule geomOpt;
    geomOpt.keywords().set("Tolerance", 1.0e-6);
    ProcessPool processPool;
    processPool.setUp("Dummy", {0});
    geomOpt.optimiseSpecies(potMap, processPool, &water);

    // Check final geometry
    auto &b01 = water.getBond(0, 1)->get();
    auto &b12 = water.getBond(1, 2)->get();
    auto &a = water.getAngle(0, 1, 2)->get();
    EXPECT_NEAR(1.0, (b01.i()->r() - b01.j()->r()).magnitude(), 1.0e-4);
    EXPECT_NEAR(1.2, (b12.i()->r() - b12.j()->r()).magnitude(), 1.0e-4);
    EXPECT_NEAR(123.45, NonPeriodicBox::literalAngleInDegrees(a.i()->r(), a.j()->r(), a.k()->r()), 1.0e-3);
}

} // namespace UnitTest
