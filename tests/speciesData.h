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

    // Add species node
    auto speciesNodeUniquePtr = std::make_unique<SpeciesNode>(parentGraph);
    auto speciesNodePtr = speciesNodeUniquePtr.get();
    auto species = &(speciesNodePtr->species());
    parentGraph->addNode(std::move(speciesNodeUniquePtr), name);
    species->setName(name);

    // Set up atom types
    auto oW = species->addAtomType(Elements::Element::O, "OW");
    oW->interactionPotential().setFormAndParameters(ShortRangeFunctions::Form::LennardJones, "epsilon=0.6503 sigma=3.165492");
    oW->setCharge(-0.82);
    species->addAtom(Elements::Element::O, {}, -0.82, oW);
    auto hW = species->addAtomType(Elements::Element::H, "HW");
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

    return speciesNodePtr;
}

// Create and return methanol test species in the specified graph
inline SpeciesNode *createMethanol(Graph *parentGraph)
{
    const auto name = "Methanol";

    // Add methanol species node
    auto speciesNodeUniquePtr = std::make_unique<SpeciesNode>(parentGraph);
    auto speciesNodePtr = speciesNodeUniquePtr.get();
    auto species = &(speciesNodePtr->species());
    parentGraph->addNode(std::move(speciesNodeUniquePtr), name);
    species->setName(name);

    // Create atom types
    auto CT = species->addAtomType(Elements::Element::C, "CT");
    CT->interactionPotential().setFormAndParameters(ShortRangeFunctions::Form::LennardJonesGeometric,
                                                    "epsilon=0.276 sigma=3.55");
    CT->setCharge(-0.18);
    auto HC = species->addAtomType(Elements::Element::H, "HC");
    HC->interactionPotential().setFormAndParameters(ShortRangeFunctions::Form::LennardJonesGeometric,
                                                    "epsilon=0.126 sigma=2.5");
    HC->setCharge(0.06);
    auto OH = species->addAtomType(Elements::Element::O, "OH");
    OH->interactionPotential().setFormAndParameters(ShortRangeFunctions::Form::LennardJonesGeometric,
                                                    "epsilon=0.711 sigma=3.12");
    OH->setCharge(-0.68);
    auto HO = species->addAtomType(Elements::Element::H, "HO");
    HO->interactionPotential().setFormAndParameters(ShortRangeFunctions::Form::LennardJonesGeometric,
                                                    "epsilon=0.126 sigma=2.4");
    HO->setCharge(0.68);

    // Add atoms
    species->addAtom(Elements::Element::C, {0.0, 0.0, 0.0}, -0.18, CT);
    species->addAtom(Elements::Element::H, {1.1187, 0.0, 0.0}, 0.06, HC);
    species->addAtom(Elements::Element::O, {-0.3683, 1.3617, 0.0}, -0.68, OH);
    species->addAtom(Elements::Element::H, {-0.3834, -0.5181, -0.9144}, 0.06, HC);
    species->addAtom(Elements::Element::H, {-0.3834, -0.5177, 0.9146}, 0.06, HC);
    species->addAtom(Elements::Element::H, {-1.3318, 1.3955, -0.17}, 0.68, HO);

    // Apply intramolecular terms
    species->addBond(0, 1).setInteractionFormAndParameters(BondFunctions::Form::Harmonic, "k=3000.0 eq=1.12");
    species->addBond(0, 2).setInteractionFormAndParameters(BondFunctions::Form::Harmonic, "k=3000.0 eq=1.41");
    species->addBond(0, 3).setInteractionFormAndParameters(BondFunctions::Form::Harmonic, "k=3000.0 eq=1.12");
    species->addBond(0, 4).setInteractionFormAndParameters(BondFunctions::Form::Harmonic, "k=3000.0 eq=1.12");
    species->addBond(2, 5).setInteractionFormAndParameters(BondFunctions::Form::Harmonic, "k=3000.0 eq=0.964");
    species->addAngle(1, 0, 2).setInteractionFormAndParameters(AngleFunctions::Form::Harmonic, "k=300.0 eq=109.5");
    species->addAngle(1, 0, 3).setInteractionFormAndParameters(AngleFunctions::Form::Harmonic, "k=300.0 eq=109.5");
    species->addAngle(1, 0, 4).setInteractionFormAndParameters(AngleFunctions::Form::Harmonic, "k=300.0 eq=109.5");
    species->addAngle(2, 0, 3).setInteractionFormAndParameters(AngleFunctions::Form::Harmonic, "k=300.0 eq=109.5");
    species->addAngle(2, 0, 4).setInteractionFormAndParameters(AngleFunctions::Form::Harmonic, "k=300.0 eq=109.5");
    species->addAngle(3, 0, 4).setInteractionFormAndParameters(AngleFunctions::Form::Harmonic, "k=300.0 eq=109.5");
    species->addAngle(5, 2, 0).setInteractionFormAndParameters(AngleFunctions::Form::Harmonic, "k=300.0 eq=109.5");

    // Create isotopologues
    auto D = species->addIsotopologue("Deuteriated");
    D->setAtomTypeIsotope(HO.get(), Sears91::H_2);
    D->setAtomTypeIsotope(HC.get(), Sears91::H_2);
    auto MeD = species->addIsotopologue("MethylD-OH");
    D->setAtomTypeIsotope(HC.get(), Sears91::H_2);
    auto MeH = species->addIsotopologue("MethylH-OD");
    D->setAtomTypeIsotope(HO.get(), Sears91::H_2);

    return speciesNodePtr;
}

// Create and return benzene test species in the specified graph
inline SpeciesNode *createBenzene(Graph *parentGraph)
{
    const auto name = "Benzene";

    // Add species node
    auto speciesNodeUniquePtr = std::make_unique<SpeciesNode>(parentGraph);
    auto speciesNodePtr = speciesNodeUniquePtr.get();
    auto species = &(speciesNodePtr->species());
    parentGraph->addNode(std::move(speciesNodeUniquePtr), name);
    species->setName(name);

    // Set up atom types
    auto CA = species->addAtomType(Elements::Element::C, "CA");
    CA->interactionPotential().setFormAndParameters(ShortRangeFunctions::Form::LennardJonesGeometric,
                                                    "epsilon=0.29288 sigma=3.55");
    CA->setCharge(-0.115);
    auto HA = species->addAtomType(Elements::Element::H, "HA");
    HA->interactionPotential().setFormAndParameters(ShortRangeFunctions::Form::LennardJonesGeometric,
                                                    "epsilon=0.12552 sigma=2.42");
    HA->setCharge(0.115);

    // Add atoms
    species->addAtom(Elements::Element::C, {-1.203775, 0.695, 0.0}, -0.115, CA);
    species->addAtom(Elements::Element::H, {-2.069801, 1.195, 0.0}, -0.115, HA);
    species->addAtom(Elements::Element::C, {-0.000000, 1.390, 0.0}, -0.115, CA);
    species->addAtom(Elements::Element::H, {-0.000000, 2.390, 0.0}, -0.115, HA);
    species->addAtom(Elements::Element::C, {1.203775, 0.695, 0.0}, -0.115, CA);
    species->addAtom(Elements::Element::H, {2.069801, 1.195, 0.0}, -0.115, HA);
    species->addAtom(Elements::Element::C, {1.203775, -0.695, 0.0}, -0.115, CA);
    species->addAtom(Elements::Element::H, {2.069801, -1.195, 0.0}, -0.115, HA);
    species->addAtom(Elements::Element::C, {-0.000000, -1.390, 0.0}, -0.115, CA);
    species->addAtom(Elements::Element::H, {-0.000000, -2.390, 0.0}, -0.115, HA);
    species->addAtom(Elements::Element::C, {-1.203775, -0.695, 0.0}, -0.115, CA);
    species->addAtom(Elements::Element::H, {-2.069801, -1.195, 0.0}, -0.115, HA);

    // Add intramolecular terms
    for (auto i = 0; i < 12; i += 2)
    {
        // C-H bond: (i , i+1)
        species->addBond(i, i + 1).setInteractionFormAndParameters(BondFunctions::Form::Harmonic, "k=3071.056 eq=1.08");
        // C-C bond: (i, i+2)
        species->addBond(i, (i + 2) % 12).setInteractionFormAndParameters(BondFunctions::Form::Harmonic, "k=3924.592 eq=1.4");
        // H-C-C angles: (i+1, i, i+2) and (i+1, i, i-2)
        species->addAngle(i + 1, i, (i + 2) % 12)
            .setInteractionFormAndParameters(AngleFunctions::Form::Harmonic, "k=292.88 eq=120.0");
        species->addAngle(i + 1, i, DissolveMath::wrap(i - 2, 0, 11))
            .setInteractionFormAndParameters(AngleFunctions::Form::Harmonic, "k=292.88 eq=120.0");
        // C-C-C angle: (i-2, i, i+2)
        species->addAngle(DissolveMath::wrap(i - 2, 0, 11), i, (i + 2) % 12)
            .setInteractionFormAndParameters(AngleFunctions::Form::Harmonic, "k=527.184 eq=120.0");
        // H-C-C-H torsion: (i+1, i, i+2, i+3)
        species->addTorsion(i + 1, i, DissolveMath::wrap(i + 2, 0, 11), DissolveMath::wrap(i + 3, 0, 11))
            .setInteractionFormAndParameters(TorsionFunctions::Form::Cos3, "k1=0.0 k2=30.334 k3=0.0");
        // H-C-C-C torsion: (i+1, i, i+2, i+4)
        species->addTorsion(i + 1, i, DissolveMath::wrap(i + 2, 0, 11), DissolveMath::wrap(i + 4, 0, 11))
            .setInteractionFormAndParameters(TorsionFunctions::Form::Cos3, "k1=0.0 k2=30.334 k3=0.0");
        // C-C-C-C torsion: (i, i+2, i+4, i+6)
        species
            ->addTorsion(i, DissolveMath::wrap(i + 2, 0, 11), DissolveMath::wrap(i + 4, 0, 11),
                         DissolveMath::wrap(i + 6, 0, 11))
            .setInteractionFormAndParameters(TorsionFunctions::Form::Cos3, "k1=0.0 k2=30.334 k3=0.0");
    }
    species->print();

    // Create isotopologue
    auto iso = species->addIsotopologue("C6D6");
    iso->setAtomTypeIsotope(HA.get(), Sears91::H_2);

    return speciesNodePtr;
}
} // namespace UnitTest
