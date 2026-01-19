// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "classes/species.h"
#include "data/elements.h"
#include "math/mathFunc.h"
#include "nodes/graph.h"
#include "nodes/species.h"
#include <gtest/gtest.h>

namespace UnitTest
{
// Create and return water test species in the specified graph
inline SpeciesNode *createWater(Graph *parentGraph)
{
    const auto name = "Water";

    // Add water species node
    auto speciesNode = std::make_unique<SpeciesNode>(parentGraph);
    auto species = &(speciesNode.get()->species());
    parentGraph->addNode(std::move(speciesNode), name);

    // Set up water species and atom types
    species->clear();
    species->setName(name);

    auto oW = std::make_shared<AtomType>(Elements::Element::O, "OW");
    oW->interactionPotential().setFormAndParameters(ShortRangeFunctions::Form::LennardJones, "epsilon=0.6503 sigma=3.165492");
    oW->setCharge(-0.82);
    species->addAtom(Elements::Element::O, {}, -0.82, oW);
    auto hW = std::make_shared<AtomType>(Elements::Element::H, "HW");
    hW->interactionPotential().setFormAndParameters(ShortRangeFunctions::Form::LennardJones, "epsilon=0.0 sigma=0.0");
    hW->setCharge(0.41);
    species->addAtom(Elements::Element::H, {1, 0, 0}, 0.41, hW);
    species->addAtom(Elements::Element::H, {cos(DissolveMath::toRadians(113.24)), sin(DissolveMath::toRadians(113.24)), 0.0},
                     0.41, hW);

    // Apply intramolecular terms
    species->addBond(0, 1).setInteractionFormAndParameters(BondFunctions::Form::Harmonic, "k=4431.53 eq=1.0");
    species->addBond(0, 2).setInteractionFormAndParameters(BondFunctions::Form::Harmonic, "k=4431.53 eq=1.0");
    species->addAngle(1, 0, 2).setInteractionFormAndParameters(AngleFunctions::Form::Harmonic, "k=317.5656 eq=113.24");

    // Create isotopologue
    auto iso = species->addIsotopologue("D2O");
    iso->setAtomTypeIsotope(hW.get(), Sears91::H_2);

    return speciesNode.get();
}

} // namespace UnitTest
