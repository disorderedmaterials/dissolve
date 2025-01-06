// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include "generator/addBase.h"
#include "classes/box.h"
#include "generator/regionBase.h"
#include "keywords/bool.h"
#include "keywords/node.h"
#include "keywords/nodeValue.h"
#include "keywords/nodeValueEnumOptions.h"

AddGeneratorNodeBase::AddGeneratorNodeBase(GeneratorNode::NodeType nodeType, const NodeValue &population,
                                           const NodeValue &density, Units::DensityUnits densityUnits)
    : GeneratorNode(nodeType), density_{density, densityUnits}, population_(population)
{
}

// Set up base keywords
void AddGeneratorNodeBase::setUpBaseKeywords()
{
    keywords_.setOrganisation("Options", "Target");
    keywords_.add<NodeValueKeyword>("Population", "Population of the target to add", population_, this);
    keywords_.add<NodeValueEnumOptionsKeyword<Units::DensityUnits>>("Density", "Density at which to add the target", density_,
                                                                    this, Units::densityUnits());
    keywords_.add<EnumOptionsKeyword<AddGeneratorNodeBase::PositioningType>>("Positioning", "Positioning type",
                                                                             positioningType_, positioningTypes());
    keywords_.add<NodeKeyword<RegionGeneratorNodeBase>>(
        "Region", "Target region into which to add", region_, this,
        NodeTypeVector{NodeType::CustomRegion, NodeType::CylindricalRegion, NodeType::GeneralRegion});

    keywords_.setOrganisation("Options", "Box Modification");
    keywords_.add<EnumOptionsKeyword<AddGeneratorNodeBase::BoxActionStyle>>(
        "BoxAction", "Action to take on the Box geometry / volume on addition of the species", boxAction_, boxActionStyles());
    keywords_.add<BoolKeyword>("ScaleA", "Scale box length A when modifying volume", scaleA_);
    keywords_.add<BoolKeyword>("ScaleB", "Scale box length B when modifying volume", scaleB_);
    keywords_.add<BoolKeyword>("ScaleC", "Scale box length C when modifying volume", scaleC_);
}

/*
 * Identity
 */

// Return whether a name for the node must be provided
bool AddGeneratorNodeBase::mustBeNamed() const { return false; }

/*
 * Node Data
 */

// Return enum option info for PositioningType
EnumOptions<AddGeneratorNodeBase::BoxActionStyle> AddGeneratorNodeBase::boxActionStyles()
{
    return EnumOptions<AddGeneratorNodeBase::BoxActionStyle>(
        "BoxAction", {{AddGeneratorNodeBase::BoxActionStyle::None, "None"},
                      {AddGeneratorNodeBase::BoxActionStyle::AddVolume, "AddVolume"},
                      {AddGeneratorNodeBase::BoxActionStyle::ScaleVolume, "ScaleVolume"},
                      {AddGeneratorNodeBase::BoxActionStyle::Set, "Set"}});
}

// Return enum option info for PositioningType
EnumOptions<AddGeneratorNodeBase::PositioningType> AddGeneratorNodeBase::positioningTypes()
{
    return EnumOptions<AddGeneratorNodeBase::PositioningType>("PositioningType",
                                                              {{AddGeneratorNodeBase::PositioningType::Central, "Central"},
                                                               {AddGeneratorNodeBase::PositioningType::Current, "Current"},
                                                               {AddGeneratorNodeBase::PositioningType::Random, "Random"},
                                                               {AddGeneratorNodeBase::PositioningType::Region, "Region"}});
}

/*
 * DSDSD
 */

// Adjust or set box volume ready for addition
void AddGeneratorNodeBase::adjustBoxVolume(Configuration *cfg, int nCopies, int nAtomsPerCopy, double massPerCopy) const
{
    auto rho = std::get<0>(density_).asDouble();
    auto rhoUnits = std::get<1>(density_);

    // Determine volume required to contain the population of the specified Species at the requested density
    auto requiredVolume = 0.0;
    if (rhoUnits == Units::AtomsPerAngstromUnits)
        requiredVolume = (nCopies * nAtomsPerCopy) / rho;
    else
        requiredVolume = ((nCopies * massPerCopy) / AVOGADRO) / (rho / 1.0E24);

    // If a density was not given, just add new molecules to the current box without adjusting its size
    if (boxAction_ == AddGeneratorNodeBase::BoxActionStyle::None)
        Messenger::print("[{}] Current box geometry / volume will remain as-is.\n", GeneratorNode::nodeTypes().keyword(type_));
    else if (boxAction_ == AddGeneratorNodeBase::BoxActionStyle::AddVolume)
    {
        Messenger::print("[{}] Current box volume will be increased to accommodate volume of new species.\n",
                         GeneratorNode::nodeTypes().keyword(type_));

        // Get current cell volume
        auto currentVolume = cfg->box()->volume();

        Messenger::print("[{}] Density for new molecule(s) is {} {}.\n", GeneratorNode::nodeTypes().keyword(type_), rho,
                         Units::densityUnits().keyword(rhoUnits));
        Messenger::print("[{}] Required volume for new molecule(s) is {} cubic Angstroms.\n",
                         GeneratorNode::nodeTypes().keyword(type_), requiredVolume);

        // If the current box has no atoms in it, absorb the current volume rather than adding to it
        if (cfg->nAtoms() > 0)
            requiredVolume += currentVolume;
        else
            Messenger::print("[{}] Current box is empty, so new volume will be set to exact ly {} cubic Angstroms.\n",
                             GeneratorNode::nodeTypes().keyword(type_), requiredVolume);

        auto scaleFactors = cfg->box()->scaleFactors(requiredVolume, {scaleA_, scaleB_, scaleC_});

        // Scale existing contents
        cfg->scaleContents(scaleFactors);

        // Scale the current Box so there is enough space for our new species
        cfg->scaleBox(scaleFactors);

        Messenger::print("[{}] New box volume is {:e} cubic Angstroms - scale factors were ({},{},{}).\n",
                         GeneratorNode::nodeTypes().keyword(type_), cfg->box()->volume(), scaleFactors.x, scaleFactors.y,
                         scaleFactors.z);
    }
    else if (boxAction_ == AddGeneratorNodeBase::BoxActionStyle::ScaleVolume)
    {
        Messenger::print("[{}] Box volume will be set to give supplied density.\n", GeneratorNode::nodeTypes().keyword(type_));

        // Get volume required to hold current cell contents at the requested density
        auto existingRequiredVolume = 0.0;
        if (rhoUnits == Units::AtomsPerAngstromUnits)
            existingRequiredVolume = cfg->nAtoms() / rho;
        else
            existingRequiredVolume = cfg->atomicMass() / (rho / 1.0E24);
        Messenger::print("[{}] Existing contents requires volume of {} cubic Angstroms at specified density.\n",
                         GeneratorNode::nodeTypes().keyword(type_), existingRequiredVolume);

        Messenger::print("[{}] Required volume for new species is {} cubic Angstroms.\n",
                         GeneratorNode::nodeTypes().keyword(type_), requiredVolume);

        // Add on required volume for existing box contents
        if (cfg->nAtoms() > 0)
            requiredVolume += existingRequiredVolume;

        auto scaleFactors = cfg->box()->scaleFactors(requiredVolume, {scaleA_, scaleB_, scaleC_});

        // Scale existing contents
        cfg->scaleContents(scaleFactors);

        // Scale the current Box so there is enough space for our new species
        cfg->scaleBox(scaleFactors);

        Messenger::print("[{}] Current box scaled by ({},{},{}) - new volume is {:e} cubic Angstroms.\n",
                         GeneratorNode::nodeTypes().keyword(type_), scaleFactors.x, scaleFactors.y, scaleFactors.z,
                         cfg->box()->volume());
    }
    else if (boxAction_ == AddGeneratorNodeBase::BoxActionStyle::Set)
    {
        throw(
            std::runtime_error("A box action of 'Set' must be handled outside of AddGeneratorNodeBase::adjustBoxVolume().\n"));
    }
}

/*
 * Execute
 */

// Prepare any necessary data, ready for execution in the base class
bool AddGeneratorNodeBase::prepareBase(const GeneratorContext &generatorContext)
{
    // If positioningType_ type is 'Region', must have a suitable node defined
    if (positioningType_ == AddGeneratorNodeBase::PositioningType::Region && !region_)
        return Messenger::error("A valid region must be specified with the 'Region' keyword.\n");
    else if (positioningType_ != AddGeneratorNodeBase::PositioningType::Region && region_)
        Messenger::warn(
            "A region has been specified ({}) but the positioning type is set to '{}' (rather than targetting the region).\n",
            region_->name(), AddGeneratorNodeBase::positioningTypes().keyword(positioningType_));

    // Check scalable axes definitions
    if (!scaleA_ && !scaleB_ && !scaleC_)
        return Messenger::error("Must have at least one scalable box axis!\n");

    // If the positioning type is 'Central', don't allow more than one molecule to be added
    if (positioningType_ == AddGeneratorNodeBase::PositioningType::Central && population_.asInteger() > 1)
        return Messenger::error(
            "Positioning type is set to be the centre of the box, but the requested population is greater than 1 ({}).\n",
            population_.asInteger());

    return true;
}
