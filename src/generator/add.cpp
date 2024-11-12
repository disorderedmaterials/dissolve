// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "generator/add.h"
#include "base/randomBuffer.h"
#include "classes/box.h"
#include "classes/configuration.h"
#include "classes/species.h"
#include "generator/coordinateSets.h"
#include "generator/regionBase.h"
#include "keywords/bool.h"
#include "keywords/node.h"
#include "keywords/nodeValue.h"
#include "keywords/nodeValueEnumOptions.h"
#include "keywords/species.h"

AddGeneratorNode::AddGeneratorNode(const Species *sp, const NodeValue &population, const NodeValue &density,
                                   Units::DensityUnits densityUnits)
    : AddGeneratorNodeBase(NodeType::Add, population, density, densityUnits), species_(sp)
{
    setUpKeywords();
}

AddGeneratorNode::AddGeneratorNode(std::shared_ptr<const CoordinateSetsGeneratorNode> sets, const NodeValue &population,
                                   const NodeValue &density, Units::DensityUnits densityUnits)
    : AddGeneratorNodeBase(NodeType::Add, population, density, densityUnits), coordinateSets_(std::move(sets))
{
    setUpKeywords();
}

// Set up keywords for node
void AddGeneratorNode::setUpKeywords()
{
    keywords_.setOrganisation("Options", "Target");
    keywords_.add<SpeciesKeyword>("Species", "Target species to add", species_);
    keywords_.add<NodeKeyword<CoordinateSetsGeneratorNode>>("CoordinateSets", "Target coordinate sets to add", coordinateSets_,
                                                            this, NodeTypeVector{NodeType::CoordinateSets});
    keywords_.add<BoolKeyword>("Rotate", "Whether to randomly rotate molecules on insertion", rotate_);

    setUpBaseKeywords();
}

/*
 * Execute
 */

// Prepare any necessary data, ready for execution
bool AddGeneratorNode::prepare(const GeneratorContext &generatorContext)
{
    if (!species_ && !coordinateSets_)
        return Messenger::error("No target Species or coordinate sets specified in Add node.\n");

    if (species_ && coordinateSets_)
        return Messenger::error("Specify either target Species or target coordinate sets, but not both.\n");

    // Check for a periodic species in the case of boxAction_ == Set
    if (boxAction_ == AddGeneratorNodeBase::BoxActionStyle::Set)
    {
        if (species_->box()->type() == Box::BoxType::NonPeriodic)
            return Messenger::error("Target species '{}' is not periodic!.\n", species_->name());

        if (population_.asInteger() != 1)
            return Messenger::error("Target population must be set to 1.\n");
    }

    return prepareBase(generatorContext);
}

// Execute node
bool AddGeneratorNode::execute(const GeneratorContext &generatorContext)
{
    // Get target species
    auto *sp = species_ ? species_ : coordinateSets_->keywords().getSpecies("Species");

    auto ipop = population_.asInteger();
    if (ipop > 0)
        Messenger::print("[Add] Adding species '{}' - population is {}.\n", sp->name(), ipop);
    else
    {
        Messenger::print("[Add] Population of species '{}' is zero so it will not be added.\n", sp->name());
        return true;
    }

    auto *cfg = generatorContext.configuration();

    // Set / adjust target box volume
    if (boxAction_ == AddGeneratorNodeBase::BoxActionStyle::Set)
    {
        Messenger::print("[Add}] Box geometry will be set from the species box definition.\n");

        cfg->createBox(sp->box()->axisLengths(), sp->box()->axisAngles());
        auto *box = cfg->box();

        Messenger::print("[Add] Box type is now {}: A = {:10.4e} B = {:10.4e} C = {:10.4e}, alpha = {:10.4e} beta = "
                         "{:10.4e} gamma = {:10.4e}\n",
                         Box::boxTypes().keyword(box->type()), box->axisLengths().x, box->axisLengths().y, box->axisLengths().z,
                         box->axisAngles().x, box->axisAngles().y, box->axisAngles().z);
    }
    else
        adjustBoxVolume(cfg, ipop, sp->nAtoms(SpeciesAtom::Presence::Physical), sp->mass());

    // Get the positioningType_ type and rotation flag
    Messenger::print("[Add] Positioning type is '{}' and rotation is {}.\n",
                     AddGeneratorNodeBase::positioningTypes().keyword(positioningType_), rotate_ ? "on" : "off");

    // Checks for regional positioning
    if (positioningType_ == AddGeneratorNodeBase::PositioningType::Region)
    {
        if (!region_->region().isValid())
            return Messenger::error("Region '{}' is invalid, probably because it contains no free space.\n", region_->name());

        Messenger::print("[Add] Target region ('{}') covers {:.2f}% of the box volume.\n", region_->name(),
                         region_->region().freeVoxelFraction() * 100.0);
    }

    // Now we add the molecules
    RandomBuffer randomBuffer(generatorContext.processPool(), ProcessPool::PoolProcessesCommunicator);
    Vec3<double> newCentre, fr;
    auto coordinateSetIndex = 0;
    auto hasCoordinateSets = false;
    if (coordinateSets_)
    {
        if (coordinateSets_->nSets() == 0)
            return Messenger::error("Coordinate set source appears to be empty.");

        hasCoordinateSets = true;
    }
    Matrix3 transform;
    const auto *box = cfg->box();
    cfg->atoms().reserve(cfg->atoms().size() + ipop * sp->nAtoms());
    for (auto n = 0; n < ipop; ++n)
    {
        // Add the Molecule - use coordinate set if one is available
        std::shared_ptr<Molecule> mol;
        if (hasCoordinateSets)
        {
            mol = cfg->addMolecule(sp, coordinateSets_->set(coordinateSetIndex));

            // Move to next coordinate set
            ++coordinateSetIndex;
            if (coordinateSetIndex == coordinateSets_->nSets())
                coordinateSetIndex = 0;
        }
        else
            mol = cfg->addMolecule(sp);

        // Set / generate position of Molecule
        switch (positioningType_)
        {
            case (AddGeneratorNodeBase::PositioningType::Random):
                fr.set(randomBuffer.random(), randomBuffer.random(), randomBuffer.random());
                newCentre = box->getReal(fr);
                mol->setCentreOfGeometry(box, newCentre);
                break;
            case (AddGeneratorNodeBase::PositioningType::Region):
                mol->setCentreOfGeometry(box, region_->region().randomCoordinate());
                break;
            case (AddGeneratorNodeBase::PositioningType::Central):
                fr.set(0.5, 0.5, 0.5);
                newCentre = box->getReal(fr);
                mol->setCentreOfGeometry(box, newCentre);
                break;
            case (AddGeneratorNodeBase::PositioningType::Current):
                break;
            default:
                throw(std::runtime_error(
                    fmt::format("Positioning type {} not handled.\n", positioningTypes().keyword(positioningType_))));
        }

        // Generate and apply a random rotation matrix
        if (rotate_)
        {
            transform.createRotationXY(randomBuffer.randomPlusMinusOne() * 180.0, randomBuffer.randomPlusMinusOne() * 180.0);
            mol->transform(box, transform);
        }
    }

    Messenger::print("[Add] New box density is {:e} atoms/Angstrom**3 ({} g/cm3).\n", cfg->atomicDensity().value_or(0.0),
                     cfg->chemicalDensity().value_or(0.0));

    // We've added new content to the box, so Need to update our object relationships
    cfg->updateObjectRelationships();

    return true;
}
