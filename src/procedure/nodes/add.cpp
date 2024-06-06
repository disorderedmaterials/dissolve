// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "procedure/nodes/add.h"
#include "base/randomBuffer.h"
#include "classes/box.h"
#include "classes/configuration.h"
#include "classes/species.h"
#include "keywords/bool.h"
#include "keywords/node.h"
#include "keywords/nodeValue.h"
#include "keywords/nodeValueEnumOptions.h"
#include "keywords/species.h"
#include "procedure/nodes/coordinateSets.h"
#include "procedure/nodes/regionBase.h"

AddProcedureNode::AddProcedureNode(const Species *sp, const NodeValue &population, const NodeValue &density,
                                   Units::DensityUnits densityUnits)
    : ProcedureNode(ProcedureNode::NodeType::Add, {ProcedureNode::GenerationContext}), density_{density, densityUnits},
      population_(population), species_(sp)
{
    setUpKeywords();
}

AddProcedureNode::AddProcedureNode(std::shared_ptr<const CoordinateSetsProcedureNode> sets, const NodeValue &population,
                                   const NodeValue &density, Units::DensityUnits densityUnits)
    : ProcedureNode(ProcedureNode::NodeType::Add, {ProcedureNode::GenerationContext}),
      coordinateSets_(std::move(sets)), density_{density, densityUnits}, population_(population)
{
    setUpKeywords();
}

// Set up keywords for node
void AddProcedureNode::setUpKeywords()
{
    keywords_.setOrganisation("Options", "Target");
    keywords_.add<SpeciesKeyword>("Species", "Target species to add", species_);
    keywords_.add<NodeKeyword<CoordinateSetsProcedureNode>>("CoordinateSets", "Target coordinate sets to add", coordinateSets_,
                                                            this, NodeTypeVector{NodeType::CoordinateSets}, true);
    keywords_.add<NodeValueKeyword>("Population", "Population of the target species to add", population_, this);
    keywords_.add<NodeValueEnumOptionsKeyword<Units::DensityUnits>>("Density", "Density at which to add the target species",
                                                                    density_, this, Units::densityUnits());

    keywords_.setOrganisation("Options", "Box Modification");
    keywords_.add<EnumOptionsKeyword<AddProcedureNode::BoxActionStyle>>(
        "BoxAction", "Action to take on the Box geometry / volume on addition of the species", boxAction_, boxActionStyles());
    keywords_.add<BoolKeyword>("ScaleA", "Scale box length A when modifying volume", scaleA_);
    keywords_.add<BoolKeyword>("ScaleB", "Scale box length B when modifying volume", scaleB_);
    keywords_.add<BoolKeyword>("ScaleC", "Scale box length C when modifying volume", scaleC_);

    keywords_.setOrganisation("Options", "Target");
    keywords_.add<EnumOptionsKeyword<AddProcedureNode::PositioningType>>(
        "Positioning", "Positioning type for individual molecules", positioningType_, positioningTypes());
    keywords_.add<NodeKeyword<RegionProcedureNodeBase>>(
        "Region", "Region into which to add the species", region_, this,
        NodeTypeVector{NodeType::CustomRegion, NodeType::CylindricalRegion, NodeType::GeneralRegion}, true);
    keywords_.add<BoolKeyword>("Rotate", "Whether to randomly rotate molecules on insertion", rotate_);
}

/*
 * Identity
 */

// Return whether a name for the node must be provided
bool AddProcedureNode::mustBeNamed() const { return false; }

/*
 * Node Data
 */

// Return enum option info for PositioningType
EnumOptions<AddProcedureNode::BoxActionStyle> AddProcedureNode::boxActionStyles()
{
    return EnumOptions<AddProcedureNode::BoxActionStyle>("BoxAction",
                                                         {{AddProcedureNode::BoxActionStyle::None, "None"},
                                                          {AddProcedureNode::BoxActionStyle::AddVolume, "AddVolume"},
                                                          {AddProcedureNode::BoxActionStyle::ScaleVolume, "ScaleVolume"},
                                                          {AddProcedureNode::BoxActionStyle::Set, "Set"}});
}

// Return enum option info for PositioningType
EnumOptions<AddProcedureNode::PositioningType> AddProcedureNode::positioningTypes()
{
    return EnumOptions<AddProcedureNode::PositioningType>("PositioningType",
                                                          {{AddProcedureNode::PositioningType::Central, "Central"},
                                                           {AddProcedureNode::PositioningType::Current, "Current"},
                                                           {AddProcedureNode::PositioningType::Random, "Random"},
                                                           {AddProcedureNode::PositioningType::Region, "Region"}});
}

/*
 * Execute
 */

// Prepare any necessary data, ready for execution
bool AddProcedureNode::prepare(const ProcedureContext &procedureContext)
{
    if (!species_ && !coordinateSets_)
        return Messenger::error("No target Species or coordinate sets specified in Add node.\n");

    if (species_ && coordinateSets_)
        return Messenger::error("Specify either target Species or target coordinate sets, but not both.\n");

    // If positioningType_ type is 'Region', must have a suitable node defined
    if (positioningType_ == AddProcedureNode::PositioningType::Region && !region_)
        return Messenger::error("A valid region must be specified with the 'Region' keyword.\n");
    else if (positioningType_ != AddProcedureNode::PositioningType::Region && region_)
        Messenger::warn(
            "A region has been specified ({}) but the positioning type is set to '{}' (rather than targetting the region).\n",
            region_->name(), AddProcedureNode::positioningTypes().keyword(positioningType_));

    // Check scalable axes definitions
    if (!scaleA_ && !scaleB_ && !scaleC_)
        return Messenger::error("Must have at least one scalable box axis!\n");

    // If the positioning type is 'Central', don't allow more than one molecule to be added
    if (positioningType_ == AddProcedureNode::PositioningType::Central && population_.asInteger() > 1)
        return Messenger::error(
            "Positioning type is set to be the centre of the box, but the requested population is greater than 1 ({}).\n",
            population_.asInteger());

    return true;
}

// Execute node
bool AddProcedureNode::execute(const ProcedureContext &procedureContext)
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

    auto *cfg = procedureContext.configuration();
    const auto nAtomsToAdd = ipop * sp->nAtoms();
    auto rho = std::get<0>(density_).asDouble();
    auto rhoUnits = std::get<1>(density_);

    // If a density was not given, just add new molecules to the current box without adjusting its size
    Vec3<bool> scalableAxes(scaleA_, scaleB_, scaleC_);
    if (boxAction_ == AddProcedureNode::BoxActionStyle::None)
        Messenger::print("[Add] Current box geometry / volume will remain as-is.\n");
    else if (boxAction_ == AddProcedureNode::BoxActionStyle::AddVolume)
    {
        Messenger::print("[Add] Current box volume will be increased to accommodate volume of new species.\n");

        // Get current cell volume
        auto currentVolume = cfg->box()->volume();

        // Determine volume required to contain the population of the specified Species at the requested density
        auto requiredVolume = 0.0;
        if (rhoUnits == Units::AtomsPerAngstromUnits)
            requiredVolume = nAtomsToAdd / rho;
        else
            requiredVolume = ((sp->mass() * ipop) / AVOGADRO) / (rho / 1.0E24);

        Messenger::print("[Add] Density for new species is {} {}.\n", rho, Units::densityUnits().keyword(rhoUnits));
        Messenger::print("[Add] Required volume for new species is {} cubic Angstroms.\n", requiredVolume);

        // If the current box has no atoms in it, absorb the current volume rather than adding to it
        if (cfg->nAtoms() > 0)
            requiredVolume += currentVolume;
        else
            Messenger::print("[Add] Current box is empty, so new volume will be set to exactly {} cubic Angstroms.\n",
                             requiredVolume);

        auto scaleFactors = cfg->box()->scaleFactors(requiredVolume, scalableAxes);

        // Scale existing contents
        cfg->scaleContents(scaleFactors);

        // Scale the current Box so there is enough space for our new species
        cfg->scaleBox(scaleFactors);

        Messenger::print("[Add] New box volume is {:e} cubic Angstroms - scale factors were ({},{},{}).\n",
                         cfg->box()->volume(), scaleFactors.x, scaleFactors.y, scaleFactors.z);
    }
    else if (boxAction_ == AddProcedureNode::BoxActionStyle::ScaleVolume)
    {
        Messenger::print("[Add] Box volume will be set to give supplied density.\n");

        // Get volume required to hold current cell contents at the requested density
        double existingRequiredVolume = 0.0;
        if (rhoUnits == Units::AtomsPerAngstromUnits)
            existingRequiredVolume = cfg->nAtoms() / rho;
        else
            existingRequiredVolume = cfg->atomicMass() / (rho / 1.0E24);
        Messenger::print("[Add] Existing contents requires volume of {} cubic Angstroms at specified density.\n",
                         existingRequiredVolume);

        // Determine volume required to contain the population of the specified Species at the requested density
        auto requiredVolume = 0.0;
        if (rhoUnits == Units::AtomsPerAngstromUnits)
            requiredVolume = nAtomsToAdd / rho;
        else
            requiredVolume = ((sp->mass() * ipop) / AVOGADRO) / (rho / 1.0E24);

        Messenger::print("[Add] Required volume for new species is {} cubic Angstroms.\n", requiredVolume);

        // Add on required volume for existing box contents
        if (cfg->nAtoms() > 0)
            requiredVolume += existingRequiredVolume;

        auto scaleFactors = cfg->box()->scaleFactors(requiredVolume, scalableAxes);

        // Scale existing contents
        cfg->scaleContents(scaleFactors);

        // Scale the current Box so there is enough space for our new species
        cfg->scaleBox(scaleFactors);

        Messenger::print("[Add] Current box scaled by ({},{},{}) - new volume is {:e} cubic Angstroms.\n", scaleFactors.x,
                         scaleFactors.y, scaleFactors.z, cfg->box()->volume());
    }
    else if (boxAction_ == AddProcedureNode::BoxActionStyle::Set)
    {
        Messenger::print("[Add] Box geometry will be set from the species box definition.\n");
        if (sp->box()->type() == Box::BoxType::NonPeriodic)
            return Messenger::error("Target species '{}' is not periodic!.\n", sp->name());

        cfg->createBox(sp->box()->axisLengths(), sp->box()->axisAngles());
        auto *box = cfg->box();

        Messenger::print("[Add] Box type is now {}: A = {:10.4e} B = {:10.4e} C = {:10.4e}, alpha = {:10.4e} beta = "
                         "{:10.4e} gamma = {:10.4e}\n",
                         Box::boxTypes().keyword(box->type()), box->axisLengths().x, box->axisLengths().y, box->axisLengths().z,
                         box->axisAngles().x, box->axisAngles().y, box->axisAngles().z);

        // Check on the requestedPopulation - we can have exactly one copy and no more
        if (ipop > 1)
        {
            Messenger::warn("Population for species '{}' reset to 1.\n", sp->name());
            ipop = 0;
        }
    }

    // Get the positioningType_ type and rotation flag
    Messenger::print("[Add] Positioning type is '{}' and rotation is {}.\n",
                     AddProcedureNode::positioningTypes().keyword(positioningType_), rotate_ ? "on" : "off");

    // Checks for regional positioning
    if (positioningType_ == AddProcedureNode::PositioningType::Region)
    {
        if (!region_)
            return Messenger::error("Positioning type set to '{}' but no region was given.\n",
                                    AddProcedureNode::positioningTypes().keyword(positioningType_));

        if (!region_->region().isValid())
            return Messenger::error("Region '{}' is invalid, probably because it contains no free space.\n", region_->name());

        Messenger::print("[Add] Target region ('{}') covers {:.2f}% of the box volume.\n", region_->name(),
                         region_->region().freeVoxelFraction() * 100.0);
    }

    // Now we add the molecules
    RandomBuffer randomBuffer(procedureContext.processPool(), ProcessPool::PoolProcessesCommunicator);
    Vec3<double> r, cog, newCentre, fr;
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

        // The atom pointers need to be updated before
        // setCentreOfGeometry is called, or else there can be a
        // segfault due to pointer invalidation.  It would be nice if
        // we could have a single lock for the whole loop, but that
        // will require some thought.
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
            case (AddProcedureNode::PositioningType::Random):
                fr.set(randomBuffer.random(), randomBuffer.random(), randomBuffer.random());
                newCentre = box->getReal(fr);
                mol->setCentreOfGeometry(box, newCentre);
                break;
            case (AddProcedureNode::PositioningType::Region):
                mol->setCentreOfGeometry(box, region_->region().randomCoordinate());
                break;
            case (AddProcedureNode::PositioningType::Central):
                fr.set(0.5, 0.5, 0.5);
                newCentre = box->getReal(fr);
                mol->setCentreOfGeometry(box, newCentre);
                break;
            case (AddProcedureNode::PositioningType::Current):
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
