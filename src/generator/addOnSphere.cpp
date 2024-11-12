// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "generator/addOnSphere.h"
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
#include "keywords/speciesSite.h"

AddOnSphereGeneratorNode::AddOnSphereGeneratorNode(const SpeciesSite *site, const NodeValue &population,
                                                   const NodeValue &density, Units::DensityUnits densityUnits)
    : AddGeneratorNodeBase(NodeType::AddOnSphere, population, density, densityUnits), speciesSite_(site)
{
    setUpKeywords();
}

// Set up keywords for node
void AddOnSphereGeneratorNode::setUpKeywords()
{
    keywords_.setOrganisation("Options", "Target");
    keywords_.add<SpeciesSiteKeyword>("Site", "Target species site to use as anchor point", speciesSite_, false);

    setUpBaseKeywords();
}

/*
 * Execute
 */

// Prepare any necessary data, ready for execution
bool AddOnSphereGeneratorNode::prepare(const GeneratorContext &generatorContext)
{
    if (!speciesSite_)
        return Messenger::error("No target species site specified in AddOnSphere node.\n");

    // If positioningType_ type is 'Region', must have a suitable node defined
    if (positioningType_ == AddGeneratorNode::PositioningType::Region && !region_)
        return Messenger::error("A valid region must be specified with the 'Region' keyword.\n");
    else if (positioningType_ != AddGeneratorNode::PositioningType::Region && region_)
        Messenger::warn(
            "A region has been specified ({}) but the positioning type is set to '{}' (rather than targetting the region).\n",
            region_->name(), AddGeneratorNode::positioningTypes().keyword(positioningType_));

    // Check scalable axes definitions
    if (!scaleA_ && !scaleB_ && !scaleC_)
        return Messenger::error("Must have at least one scalable box axis!\n");

    return true;
}

// Execute node
bool AddOnSphereGeneratorNode::execute(const GeneratorContext &generatorContext)
{
    // Get target species
    auto *sp = speciesSite_->parent();

    auto ipop = population_.asInteger();
    if (ipop > 0)
        Messenger::print("[AddOnSphere] Adding species '{}' - population is {}.\n", sp->name(), ipop);
    else
    {
        Messenger::print("[AddOnSphere] Population of species '{}' is zero so it will not be added.\n", sp->name());
        return true;
    }

    auto *cfg = generatorContext.configuration();
    const auto *box = cfg->box();

    // Set / adjust target box volume
    adjustBoxVolume(cfg, ipop, sp->nAtoms(SpeciesAtom::Presence::Physical) + sp->nAtoms(SpeciesAtom::Presence::Physical),
                    sp->mass() + sp->mass());

    // Get the positioningType_ type and rotation flag
    Messenger::print("[AddOnSphere] Positioning type is '{}'.\n",
                     AddOnSphereGeneratorNode::positioningTypes().keyword(positioningType_));

    // Checks for regional positioning
    if (positioningType_ == AddGeneratorNode::PositioningType::Region)
    {
        if (!region_->region().isValid())
            return Messenger::error("Region '{}' is invalid, probably because it contains no free space.\n", region_->name());

        Messenger::print("[AddOnSphere] Target region ('{}') covers {:.2f}% of the box volume.\n", region_->name(),
                         region_->region().freeVoxelFraction() * 100.0);
    }

    RandomBuffer randomBuffer(generatorContext.processPool(), ProcessPool::PoolProcessesCommunicator);

    // Set / generate position of sphere centre
    Vec3<double> sphereCentre, fr;
    switch (positioningType_)
    {
        case (AddGeneratorNode::PositioningType::Random):
            sphereCentre = box->getReal({randomBuffer.random(), randomBuffer.random(), randomBuffer.random()});
            break;
        case (AddGeneratorNode::PositioningType::Region):
            sphereCentre = region_->region().randomCoordinate();
            break;
        case (AddGeneratorNode::PositioningType::Central):
            sphereCentre = box->getReal({0.5, 0.5, 0.5});
            break;
        case (AddGeneratorNode::PositioningType::Current):
            break;
        default:
            throw(std::runtime_error(
                fmt::format("Positioning type {} not handled.\n", positioningTypes().keyword(positioningType_))));
    }

    // Add space for the new molecules
    cfg->atoms().reserve(cfg->atoms().size() + ipop * sp->nAtoms());

    // Now we add the molecules at points on the sphere
    auto sphereRadius = 5.0;
    Vec3<double> rLocal;
    for (auto n = 0; n < ipop; ++n)
    {
        // Add the Molecule
        auto mol = cfg->addMolecule(sp);

        // Generate a point on the sphere
        if (true)
        {
            auto theta = randomBuffer.random() * M_PI;
            auto psi = randomBuffer.random() * 2.0 * M_PI;
            rLocal.set(sin(theta) * cos(psi), sin(theta) * sin(psi), cos(theta));
            rLocal *= sphereRadius;

            mol->setCentreOfGeometry(box, sphereCentre + rLocal);
        }

        //        // Generate and apply a random rotation matrix
        //        if (rotate_)
        //        {
        //            transform.createRotationXY(randomBuffer.randomPlusMinusOne() * 180.0, randomBuffer.randomPlusMinusOne() *
        //            180.0); mol->transform(box, transform);
        //        }
    }

    Messenger::print("[AddOnSphere] New box density is {:e} atoms/Angstrom**3 ({} g/cm3).\n",
                     cfg->atomicDensity().value_or(0.0), cfg->chemicalDensity().value_or(0.0));

    // We've added new content to the box, so need to update our object relationships
    cfg->updateObjectRelationships();

    return true;
}
