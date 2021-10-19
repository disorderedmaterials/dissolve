// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "classes/atomtype.h"
#include "classes/species.h"
#include "modules/geomopt/geomopt.h"
#include <gtest/gtest.h>

namespace UnitTest
{

TEST(GeometryMinimisationTest, Water)
{
    // Set up CoreData
    CoreData coreData;
    auto atO = coreData.addAtomType(Elements::O);
    atO->setShortRangeType(Forcefield::ShortRangeType::NoInteraction);
    auto atH = coreData.addAtomType(Elements::H);
    atH->setShortRangeType(Forcefield::ShortRangeType::NoInteraction);
    std::vector<std::unique_ptr<PairPotential>> pairPotentials;
    pairPotentials.emplace_back(std::make_unique<PairPotential>())->setUp(atO, atO);
    pairPotentials.emplace_back(std::make_unique<PairPotential>())->setUp(atH, atH);
    pairPotentials.emplace_back(std::make_unique<PairPotential>())->setUp(atO, atH);
    PotentialMap potMap;
    potMap.initialise(coreData.atomTypes(), pairPotentials, 15.0);

    Species water;
    auto &i = water.addAtom(Elements::H, {0.5, 0.1, 0.0});
    i.setAtomType(atH);
    auto &j = water.addAtom(Elements::O, {0.0, 0.0, 0.0});
    j.setAtomType(atO);
    auto &k = water.addAtom(Elements::H, {0.0, 0.5, -0.06});
    k.setAtomType(atH);
    water.addBond(&i, &j).setFormAndParameters(SpeciesBond::HarmonicForm, {4000.0, 1.0});
    water.addBond(&j, &k).setFormAndParameters(SpeciesBond::HarmonicForm, {4000.0, 1.2});
    water.addAngle(&i, &j, &k).setFormAndParameters(SpeciesBond::HarmonicForm, {4000.0, 123.45});

    // Run the geometry optimisation
    GeometryOptimisationModule geomOpt;
    geomOpt.keywords().set("Tolerance", 1.0e-6);
    ProcessPool processPool;
    processPool.setUp("Dummy", {0});
    geomOpt.optimiseSpecies(potMap, processPool, &water);

    // Check final geometry
    EXPECT_NEAR(1.0, (i.r() - j.r()).magnitude(), 1.0e-4);
    EXPECT_NEAR(1.2, (j.r() - k.r()).magnitude(), 1.0e-4);
    EXPECT_NEAR(123.45, NonPeriodicBox::literalAngleInDegrees(i.r(), j.r(), k.r()), 1.0e-3);
}

} // namespace UnitTest
