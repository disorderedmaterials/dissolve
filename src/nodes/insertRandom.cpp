// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include "nodes/insertRandom.h"
#include "base/randomBuffer.h"
#include "classes/box.h"
#include "classes/configuration.h"
#include "classes/species.h"

InsertRandomNode::InsertRandomNode(Graph *parentGraph) : InsertNodeBase(parentGraph)
{
    addInput<const Species *>("Species", "Species to add - all resulting molecules will have identical geometry", species_);
    addInput<const MoleculeSet *>("MoleculeSet", "MoleculeSet to use as the source", moleculeSet_);
}

/*
 * Processing
 */

// Run main processing
NodeConstants::ProcessResult InsertRandomNode::process()
{
    // Get target MoleculeSet
    MoleculeSet speciesMoleculeSet;
    if (species_)
        speciesMoleculeSet.addMolecule(species_);
    const MoleculeSet &targetMoleculeSet = species_ ? speciesMoleculeSet : *moleculeSet_;

    auto ipop = population_.asInteger();
    if (ipop <= 0)
    {
        Messenger::warn("[InsertRandom] Population is zero so nothing will be added.\n");
        return NodeConstants::ProcessResult::Unchanged;
    }

    // Determine total number of atoms and mass to be added
    auto [nPhysicalAtoms, nAnyAtoms, massToBeAdded] = getPopulationTotals(ipop, targetMoleculeSet);

    // Set / adjust target box volume
    adjustBoxVolume(configuration_, nPhysicalAtoms, massToBeAdded);

    // Now we add the molecules
    RandomBuffer randomBuffer(processPool(), ProcessPool::PoolProcessesCommunicator);

    Matrix3 transform;
    const auto *box = configuration_->box();
    configuration_->atoms().reserve(configuration_->atoms().size() + nAnyAtoms);
    for (auto n = 0; n < ipop; ++n)
    {
        // Add the Molecule
        auto mol = configuration_->copyMolecule(targetMoleculeSet.localMolecule(n));

        // Randomise position of Molecule over the whole box
        auto newCentre = box->getReal({randomBuffer.random(), randomBuffer.random(), randomBuffer.random()});
        mol->setCentreOfGeometry(box, newCentre);

        // Generate and apply a random rotation matrix
        if (rotate_)
        {
            transform.createRotationXY(randomBuffer.randomPlusMinusOne() * 180.0, randomBuffer.randomPlusMinusOne() * 180.0);
            mol->transform(box, transform);
        }
    }

    Messenger::print("[InsertRandom] New box density is {:e} atoms/Angstrom**3 ({} g/cm3).\n",
                     configuration_->atomicDensity().value_or(0.0), configuration_->chemicalDensity().value_or(0.0));

    // We've added new content to the box, so Need to update our object relationships
    configuration_->updateObjectRelationships();

    return NodeConstants::ProcessResult::Success;
}
