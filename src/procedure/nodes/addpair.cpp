// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "procedure/nodes/addpair.h"
#include "base/randombuffer.h"
#include "classes/atomchangetoken.h"
#include "classes/box.h"
#include "classes/configuration.h"
#include "classes/coredata.h"
#include "classes/species.h"
#include "keywords/bool.h"
#include "keywords/node.h"
#include "keywords/nodevalue.h"
#include "keywords/nodevalueenumoptions.h"
#include "keywords/species.h"
#include "procedure/nodes/regionbase.h"

AddPairProcedureNode::AddPairProcedureNode(const Species *spA, const Species *spB, const NodeValue &population,
                                           const NodeValue &density, Units::DensityUnits densityUnits)
    : ProcedureNode(ProcedureNode::NodeType::AddPair), density_{density, densityUnits}, population_(population), speciesA_(spA),
      speciesB_(spB)
{
    setUpKeywords();
}

// Set up keywords for node
void AddPairProcedureNode::setUpKeywords()
{
    // Set up keywords
    keywords_.add<SpeciesKeyword>("Control", "SpeciesA", "Target species A to add", speciesA_);
    keywords_.add<SpeciesKeyword>("Control", "SpeciesB", "Target species B to add", speciesB_);
    keywords_.add<NodeValueKeyword>("Control", "Population", "Population of the target species to add", population_, this);
    keywords_.add<EnumOptionsKeyword<AddPairProcedureNode::BoxActionStyle>>(
        "Control", "BoxAction", "Action to take on the Box geometry / volume on addition of the species", boxAction_,
        boxActionStyles());
    keywords_.add<BoolKeyword>("Control", "ScaleA", "Scale box length A when modifying volume", scaleA_);
    keywords_.add<BoolKeyword>("Control", "ScaleB", "Scale box length B when modifying volume", scaleB_);
    keywords_.add<BoolKeyword>("Control", "ScaleC", "Scale box length C when modifying volume", scaleC_);
    keywords_.add<NodeValueEnumOptionsKeyword<Units::DensityUnits>>(
        "Control", "Density", "Density at which to add the target species", density_, this, Units::densityUnits());
    keywords_.add<BoolKeyword>("Control", "Rotate", "Whether to randomly rotate molecules on insertion", rotate_);
    keywords_.add<EnumOptionsKeyword<AddPairProcedureNode::PositioningType>>(
        "Control", "Positioning", "Positioning type for individual molecules", positioningType_, positioningTypes());
    keywords_.add<NodeKeyword<RegionProcedureNodeBase>>("Control", "Region", "Region into which to add the species", region_,
                                                        this, ProcedureNode::NodeClass::Region, true);
}

/*
 * Identity
 */

// Return whether specified context is relevant for this node type
bool AddPairProcedureNode::isContextRelevant(ProcedureNode::NodeContext context)
{
    return (context == ProcedureNode::GenerationContext);
}

// Return whether a name for the node must be provided
bool AddPairProcedureNode::mustBeNamed() const { return false; }

/*
 * Node Data
 */

// Return enum option info for PositioningType
EnumOptions<AddPairProcedureNode::BoxActionStyle> AddPairProcedureNode::boxActionStyles()
{
    return EnumOptions<AddPairProcedureNode::BoxActionStyle>(
        "BoxAction", {{AddPairProcedureNode::BoxActionStyle::None, "None"},
                      {AddPairProcedureNode::BoxActionStyle::AddVolume, "AddVolume"},
                      {AddPairProcedureNode::BoxActionStyle::ScaleVolume, "ScaleVolume"},
                      {AddPairProcedureNode::BoxActionStyle::Set, "Set"}});
}

// Return enum option info for PositioningType
EnumOptions<AddPairProcedureNode::PositioningType> AddPairProcedureNode::positioningTypes()
{
    return EnumOptions<AddPairProcedureNode::PositioningType>("PositioningType",
                                                              {{AddPairProcedureNode::PositioningType::Central, "Central"},
                                                               {AddPairProcedureNode::PositioningType::Current, "Current"},
                                                               {AddPairProcedureNode::PositioningType::Random, "Random"},
                                                               {AddPairProcedureNode::PositioningType::Region, "Region"}});
}

/*
 * Execute
 */

// Prepare any necessary data, ready for execution
bool AddPairProcedureNode::prepare(const ProcedureContext &procedureContext)
{
    if (!speciesA_ || !speciesB_)
        return Messenger::error("One or both target species not specified in Add node.\n");

    // If positioningType_ type is 'Region', must have a suitable node defined
    if (positioningType_ == AddPairProcedureNode::PositioningType::Region && !region_)
        return Messenger::error("A valid region must be specified with the 'Region' keyword.\n");
    else if (positioningType_ != AddPairProcedureNode::PositioningType::Region && region_)
        Messenger::warn("A region has been specified ({}) but the positioningType_ type is set to '{}'\n", region_->name(),
                        AddPairProcedureNode::positioningTypes().keyword(positioningType_));

    // Can't set box
    if (boxAction_ == AddPairProcedureNode::BoxActionStyle::Set)
        return Messenger::error("Can't set periodic box when using AddPair.\n");

    // Can't do this for periodic species
    if (speciesA_->box()->type() != Box::BoxType::NonPeriodic || speciesB_->box()->type() != Box::BoxType::NonPeriodic)
        return Messenger::error("Can't use periodic species in AddPair.\n");

    // Check scalable axes definitions
    if (!scaleA_ && !scaleB_ && !scaleC_)
        return Messenger::error("Must have at least one scalable box axis!\n");

    return true;
}

// Execute node
bool AddPairProcedureNode::execute(const ProcedureContext &procedureContext)
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

    auto *cfg = procedureContext.configuration();

    const auto nAtomsToAdd = ipop * (speciesA_->nAtoms() + speciesB_->nAtoms());
    auto [rho, rhoUnits] = density_;

    // If a density was not given, just add new molecules to the current box without adjusting its size
    Vec3<bool> scalableAxes(scaleA_, scaleB_, scaleC_);
    if (boxAction_ == AddPairProcedureNode::BoxActionStyle::None)
        Messenger::print("[AddPair] Current box geometry / volume will remain as-is.\n");
    else if (boxAction_ == AddPairProcedureNode::BoxActionStyle::AddVolume)
    {
        Messenger::print("[AddPair] Current box volume will be increased to accommodate volume of new species.\n");

        // Get current cell volume
        auto currentVolume = cfg->box()->volume();

        // Determine volume required to contain the population of the specified Species at the requested density
        auto requiredVolume = 0.0;
        if (rhoUnits == Units::AtomsPerAngstromUnits)
            requiredVolume = nAtomsToAdd / rho;
        else
            requiredVolume = (((speciesA_->mass() + speciesB_->mass()) * ipop) / AVOGADRO) / (rho / 1.0E24);

        Messenger::print("[AddPair] Density for new species is {} {}.\n", rho.asDouble(),
                         Units::densityUnits().keyword(rhoUnits));
        Messenger::print("[AddPair] Required volume for new species is {} cubic Angstroms.\n", requiredVolume);

        // If the current box has no atoms in it, absorb the current volume rather than adding to it
        if (cfg->nAtoms() > 0)
            requiredVolume += currentVolume;
        else
            Messenger::print("[AddPair] Current box is empty, so new volume will be set to exactly {} cubic Angstroms.\n",
                             requiredVolume);

        auto scaleFactors = cfg->box()->scaleFactors(requiredVolume, scalableAxes);

        // Scale existing contents
        cfg->scaleContents(scaleFactors);

        // Scale the current Box so there is enough space for our new species
        cfg->scaleBox(scaleFactors);

        Messenger::print("[AddPair] New box volume is {:e} cubic Angstroms - scale factors were ({},{},{}).\n",
                         cfg->box()->volume(), scaleFactors.x, scaleFactors.y, scaleFactors.z);
    }
    else if (boxAction_ == AddPairProcedureNode::BoxActionStyle::ScaleVolume)
    {
        Messenger::print("[AddPair] Box volume will be set to give supplied density.\n");

        // Get volume required to hold current cell contents at the requested density
        double existingRequiredVolume = 0.0;
        if (rhoUnits == Units::AtomsPerAngstromUnits)
            existingRequiredVolume = cfg->nAtoms() / rho;
        else
            existingRequiredVolume = cfg->atomicMass() / (rho / 1.0E24);
        Messenger::print("[AddPair] Existing contents requires volume of {} cubic Angstroms at specified density.\n",
                         existingRequiredVolume);

        // Determine volume required to contain the population of the specified Species at the requested density
        auto requiredVolume = 0.0;
        if (rhoUnits == Units::AtomsPerAngstromUnits)
            requiredVolume = nAtomsToAdd / rho;
        else
            requiredVolume = (((speciesA_->mass() + speciesB_->mass()) * ipop) / AVOGADRO) / (rho / 1.0E24);

        Messenger::print("[AddPair] Required volume for new species is {} cubic Angstroms.\n", requiredVolume);

        // Add on required volume for existing box contents
        if (cfg->nAtoms() > 0)
            requiredVolume += existingRequiredVolume;

        auto scaleFactors = cfg->box()->scaleFactors(requiredVolume, scalableAxes);

        // Scale existing contents
        cfg->scaleContents(scaleFactors);

        // Scale the current Box so there is enough space for our new species
        cfg->scaleBox(scaleFactors);

        Messenger::print("[AddPair] Current box scaled by ({},{},{}) - new volume is {:e} cubic Angstroms.\n", scaleFactors.x,
                         scaleFactors.y, scaleFactors.z, cfg->box()->volume());
    }

    // Get the positioningType_ type and rotation flag
    Region region;

    Messenger::print("[AddPair] Positioning type is '{}' and rotation is {}.\n",
                     AddPairProcedureNode::positioningTypes().keyword(positioningType_), rotate_ ? "on" : "off");
    if (positioningType_ == AddPairProcedureNode::PositioningType::Region)
    {
        if (!region_)
            return Messenger::error("Positioning type set to '{}' but no region was given.\n",
                                    AddPairProcedureNode::positioningTypes().keyword(positioningType_));

        region = region_->generateRegion(procedureContext.configuration());
        if (!region.isValid())
            return Messenger::error("Region '{}' is invalid, probably because it contains no free space.\n", region_->name());

        Messenger::print("[AddPair] Target region ('{}') covers {:.2f}% of the box volume.\n", region_->name(),
                         region.freeVoxelFraction() * 100.0);
    }

    // Now we add the molecules
    RandomBuffer randomBuffer(procedureContext.processPool(), ProcessPool::PoolProcessesCommunicator);
    Vec3<double> r, cog, newCentre;
    Matrix3 transform;
    const auto *box = cfg->box();
    cfg->atoms().reserve(cfg->atoms().size() + ipop * (speciesA_->nAtoms() + speciesB_->nAtoms()));

    // Add all molecule pairs
    const auto currentSize = cfg->molecules().size();
    {
        AtomChangeToken lock(*cfg);
        for (auto n = 0; n < ipop; ++n)
        {
            cfg->addMolecule(lock, speciesA_);
            cfg->addMolecule(lock, speciesB_);
        }
    }

    // Position all molecule pairs
    for (auto n = currentSize; n < cfg->molecules().size(); n += 2)
    {
        auto &molA = cfg->molecules()[n];
        auto &molB = cfg->molecules()[n + 1];

        // Set / generate position of pair
        switch (positioningType_)
        {
            case (AddPairProcedureNode::PositioningType::Random):
                newCentre = box->getReal({randomBuffer.random(), randomBuffer.random(), randomBuffer.random()});
                break;
            case (AddPairProcedureNode::PositioningType::Region):
                newCentre = region.randomCoordinate();
                break;
            case (AddPairProcedureNode::PositioningType::Central):
                newCentre = box->getReal({0.5, 0.5, 0.5});
                break;
            case (AddPairProcedureNode::PositioningType::Current):
                break;
            default:
                Messenger::error("Unrecognised positioning type.\n");
                break;
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

    return true;
}
