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
        speciesMoleculeSet = species_->getMoleculeSet();
    const MoleculeSet &targetMoleculeSet = species_ ? speciesMoleculeSet : *moleculeSet_;

    auto ipop = population_.asInteger();
    if (ipop > 0)
        Messenger::print("[InsertRandom] Adding species '{}' - population is {}.\n", sp->name(), ipop);
    else
    {
        Messenger::warn("[InsertRandom] Population is zero so nothing will be added.\n", sp->name());
        return NodeConstants::ProcessResult::Unchanged;
    }

    // Set / adjust target box volume
    if (boxAction_ == InsertRandomNodeBase::BoxActionStyle::Set)
    {
        Messenger::print("[InsertRandom] Box geometry will be set from the species box definition.\n");

        configuration_->createBox(sp->box()->axisLengths(), sp->box()->axisAngles());
        auto *box = configuration_->box();

        Messenger::print("[InsertRandom] Box type is now {}: A = {:10.4e} B = {:10.4e} C = {:10.4e}, alpha = {:10.4e} beta = "
                         "{:10.4e} gamma = {:10.4e}\n",
                         Box::boxTypes().keyword(box->type()), box->axisLengths().x, box->axisLengths().y, box->axisLengths().z,
                         box->axisAngles().x, box->axisAngles().y, box->axisAngles().z);
    }
    else
        adjustBoxVolume(cfg, ipop, sp->nAtoms(SpeciesAtom::Presence::Physical), sp->mass());

    // Now we add the molecules
    RandomBuffer randomBuffer(generatorContext.processPool(), ProcessPool::PoolProcessesCommunicator);
    Vector3 newCentre, fr;
    auto coordinateSetIndex = 0;
    auto hasCoordinateSets = false;
    if (coordinateSets_)
    {
        if (coordinateSets_->nSets() == 0)
            return Messenger::error("Coordinate set source appears to be empty.");

        hasCoordinateSets = true;
    }
    Matrix3 transform;
    const auto *box = configuration_->box();
    configuration_->atoms().reserve(configuration_->atoms().size() + ipop * sp->nAtoms());
    for (auto n = 0; n < ipop; ++n)
    {
        // Add the Molecule - use coordinate set if one is available
        std::shared_ptr<Molecule> mol;
        if (hasCoordinateSets)
        {
            mol = configuration_->addMolecule(sp, coordinateSets_->set(coordinateSetIndex));

            // Move to next coordinate set
            ++coordinateSetIndex;
            if (coordinateSetIndex == coordinateSets_->nSets())
                coordinateSetIndex = 0;
        }
        else
            mol = configuration_->addMolecule(sp);

        // Set / generate position of Molecule
        switch (positioningType_)
        {
            case (InsertRandomNodeBase::PositioningType::Random):
                fr.set(randomBuffer.random(), randomBuffer.random(), randomBuffer.random());
                newCentre = box->getReal(fr);
                mol->setCentreOfGeometry(box, newCentre);
                break;
            case (InsertRandomNodeBase::PositioningType::Region):
                mol->setCentreOfGeometry(box, region_->region().randomCoordinate());
                break;
            case (InsertRandomNodeBase::PositioningType::Central):
                fr.set(0.5, 0.5, 0.5);
                newCentre = box->getReal(fr);
                mol->setCentreOfGeometry(box, newCentre);
                break;
            case (InsertRandomNodeBase::PositioningType::Current):
                break;
            default:
                Messenger::exception("Positioning type {} not handled.\n", positioningTypes().keyword(positioningType_));
        }

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

    return true;
}
