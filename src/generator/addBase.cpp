// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

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
