// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include "generator/addPair.h"
#include "base/randomBuffer.h"
#include "classes/box.h"
#include "classes/configuration.h"
#include "classes/coreData.h"
#include "classes/species.h"
#include "generator/regionBase.h"
#include "keywords/bool.h"
#include "keywords/node.h"
#include "keywords/nodeValue.h"
#include "keywords/nodeValueEnumOptions.h"
#include "keywords/species.h"

AddPairGeneratorNode::AddPairGeneratorNode(const Species *spA, const Species *spB, const NodeValue &population,
                                           const NodeValue &density, Units::DensityUnits densityUnits)
    : AddGeneratorNodeBase(NodeType::AddPair, population, density, densityUnits), speciesA_(spA), speciesB_(spB)
{
    setUpKeywords();
}

// Set up keywords for node
void AddPairGeneratorNode::setUpKeywords()
{
    keywords_.setOrganisation("Options", "Targets");
    keywords_.add<SpeciesKeyword>("SpeciesA", "Target species A to add", speciesA_);
    keywords_.add<SpeciesKeyword>("SpeciesB", "Target species B to add", speciesB_);
    keywords_.add<BoolKeyword>("Rotate", "Whether to randomly rotate molecules on insertion", rotate_);

    setUpBaseKeywords();
}

/*
 * Execute
 */

// Prepare any necessary data, ready for execution
bool AddPairGeneratorNode::prepare(const GeneratorContext &generatorContext)
{
    if (!speciesA_ || !speciesB_)
        return Messenger::error("One or both target species not specified in Add node.\n");

    // Check if the Species itself is valid
    if (!speciesA_->checkSetUp() || !speciesB_->checkSetUp())
        return false;

    // Can't set box
    if (boxAction_ == AddGeneratorNodeBase::BoxActionStyle::Set)
        return Messenger::error("Can't set periodic box when using AddPair.\n");

    // Can't do this for periodic species
    if (speciesA_->box()->type() != Box::BoxType::NonPeriodic || speciesB_->box()->type() != Box::BoxType::NonPeriodic)
        return Messenger::error("Can't use periodic species in AddPair.\n");

    return prepareBase(generatorContext);
}

// Execute node
bool AddPairGeneratorNode::execute(const GeneratorContext &generatorContext)
{
    // Can't add the Species if it has any missing core information
    if (!speciesA_->checkSetUp())
        return Messenger::error("Can't add Species '{}' because it is not set up correctly.\n", speciesA_->name());
    if (!speciesB_->checkSetUp())
        return Messenger::error("Can't add Species '{}' because it is not set up correctly.\n", speciesA_->name());

    auto ipop = population_.asInteger();
    if (ipop > 0)
        Messenger::print("[AddPair] Adding species pair '{}/{}' - population is {}.\n", speciesA_->name(), speciesB_->name(),
                         ipop);
    else
    {
        Messenger::print("[AddPair] Population of species pair '{}/{}' is zero so it will not be added.\n", speciesA_->name(),
                         speciesB_->name());
        return true;
    }

    auto *cfg = generatorContext.configuration();

    // Set / adjust target box volume
    adjustBoxVolume(cfg, ipop,
                    speciesA_->nAtoms(SpeciesAtom::Presence::Physical) + speciesB_->nAtoms(SpeciesAtom::Presence::Physical),
                    speciesA_->mass() + speciesB_->mass());

    // Get the positioningType_ type and rotation flag
    Messenger::print("[AddPair] Positioning type is '{}' and rotation is {}.\n",
                     AddGeneratorNodeBase::positioningTypes().keyword(positioningType_), rotate_ ? "on" : "off");

    // Checks for regional positioning
    if (positioningType_ == AddGeneratorNodeBase::PositioningType::Region)
    {
        if (!region_->region().isValid())
            return Messenger::error("Region '{}' is invalid, probably because it contains no free space.\n", region_->name());

        Messenger::print("[AddPair] Target region ('{}') covers {:.2f}% of the box volume.\n", region_->name(),
                         region_->region().freeVoxelFraction() * 100.0);
    }

    // Now we add the molecules
    RandomBuffer randomBuffer(generatorContext.processPool(), ProcessPool::PoolProcessesCommunicator);
    Vec3<double> newCentre;
    Matrix3 transform;
    const auto *box = cfg->box();
    cfg->atoms().reserve(cfg->atoms().size() + ipop * (speciesA_->nAtoms() + speciesB_->nAtoms()));

    // Add all molecule pairs
    const auto currentSize = cfg->molecules().size();
    for (auto n = 0; n < ipop; ++n)
    {
        cfg->addMolecule(speciesA_);
        cfg->addMolecule(speciesB_);
    }

    // Position all molecule pairs
    for (auto n = currentSize; n < cfg->molecules().size(); n += 2)
    {
        auto &molA = cfg->molecules()[n];
        auto &molB = cfg->molecules()[n + 1];

        // Set / generate position of pair
        switch (positioningType_)
        {
            case (AddGeneratorNodeBase::PositioningType::Random):
                newCentre = box->getReal({randomBuffer.random(), randomBuffer.random(), randomBuffer.random()});
                break;
            case (AddGeneratorNodeBase::PositioningType::Region):
                newCentre = region_->region().randomCoordinate();
                break;
            case (AddGeneratorNodeBase::PositioningType::Central):
                newCentre = box->getReal({0.5, 0.5, 0.5});
                break;
            case (AddGeneratorNodeBase::PositioningType::Current):
                break;
            default:
                Messenger::exception("Positioning type {} not handled.\n",
                                     AddGeneratorNodeBase::positioningTypes().keyword(positioningType_));
        }

        // Move the molecule pair
        auto ref = (molA->centreOfGeometry(box) + molB->centreOfGeometry(box)) * 0.5;
        molA->translate(newCentre - ref);
        molB->translate(newCentre - ref);

        // Generate and apply a random rotation matrix
        if (rotate_)
        {
            transform.createRotationXY(randomBuffer.randomPlusMinusOne() * 180.0, randomBuffer.randomPlusMinusOne() * 180.0);
            molA->transform(box, transform, newCentre);
            molB->transform(box, transform, newCentre);
        }
    }

    Messenger::print("[AddPair] New box density is {:e} atoms/Angstrom**3 ({} g/cm3).\n", cfg->atomicDensity().value_or(0.0),
                     cfg->chemicalDensity().value_or(0.0));

    // We've added new content to the box, so Need to update our object relationships
    cfg->updateObjectRelationships();

    return true;
}
