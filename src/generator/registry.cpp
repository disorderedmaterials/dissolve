// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "generator/registry.h"
#include "generator/add.h"
#include "generator/addPair.h"
#include "generator/box.h"
#include "generator/coordinateSets.h"
#include "generator/copy.h"
#include "generator/customRegion.h"
#include "generator/cylindricalGlobalPotential.h"
#include "generator/cylindricalRegion.h"
#include "generator/generalRegion.h"
#include "generator/importCoordinates.h"
#include "generator/parameters.h"
#include "generator/pick.h"
#include "generator/pickProximity.h"
#include "generator/pickRegion.h"
#include "generator/regionalGlobalPotential.h"
#include "generator/remove.h"
#include "generator/restraintPotential.h"
#include "generator/rotateFragment.h"
#include "generator/select.h"
#include "generator/sizeFactor.h"
#include "generator/sphericalGlobalPotential.h"
#include "generator/temperature.h"
#include "generator/transmute.h"

GeneratorNodeRegistry::GeneratorNodeRegistry()
{
    // Build
    registerProducer<AddGeneratorNode>(GeneratorNode::NodeType::Add, "Add molecules to a configuration", "Build");
    registerProducer<AddPairGeneratorNode>(GeneratorNode::NodeType::AddPair,
                                           "Add a correlated molecule pair to a configuration", "Build");
    registerProducer<BoxGeneratorNode>(GeneratorNode::NodeType::Box, "Define containing box for a configuration", "Build");
    registerProducer<CoordinateSetsGeneratorNode>(GeneratorNode::NodeType::CoordinateSets,
                                                  "Generate coordinate sets for a species", "Build");
    registerProducer<CopyGeneratorNode>(GeneratorNode::NodeType::Copy, "Copy the contents of a configuration", "Build");
    registerProducer<ImportCoordinatesGeneratorNode>(GeneratorNode::NodeType::ImportCoordinates,
                                                     "Import coordinates into a configuration", "Build");
    registerProducer<RemoveGeneratorNode>(GeneratorNode::NodeType::Remove, "Remove molecules from a configuration", "Build");
    registerProducer<SizeFactorGeneratorNode>(GeneratorNode::NodeType::SizeFactor,
                                              "Scale a configuration's contents with a size factor", "Build");
    registerProducer<TemperatureGeneratorNode>(GeneratorNode::NodeType::Temperature, "Set temperature for configuration",
                                               "Build");
    registerProducer<TransmuteGeneratorNode>(GeneratorNode::NodeType::Transmute,
                                             "Turn molecules from one species type into another", "Build");

    // General
    registerProducer<ParametersGeneratorNode>(GeneratorNode::NodeType::Parameters, "Define parameters for use in expressions",
                                              "General");

    // Regions
    registerProducer<CustomRegionGeneratorNode>(GeneratorNode::NodeType::CustomRegion,
                                                "Generate a custom region based on an expression", "Regions");
    registerProducer<CylindricalRegionGeneratorNode>(GeneratorNode::NodeType::CylindricalRegion,
                                                     "Define a cylindrical region in a configuration", "Regions");
    registerProducer<GeneralRegionGeneratorNode>(GeneratorNode::NodeType::GeneralRegion,
                                                 "Define a general region in a configuration", "Regions");

    // Pick
    registerProducer<PickGeneratorNode>(GeneratorNode::NodeType::Pick, "Pick all molecules of a given species", "Pick");
    registerProducer<PickProximityGeneratorNode>(GeneratorNode::NodeType::PickProximity,
                                                 "Pick molecules based on proximity to others", "Pick");
    registerProducer<PickRegionGeneratorNode>(GeneratorNode::NodeType::PickRegion, "Pick molecules within a specific region",
                                              "Pick");

    // Potentials
    registerProducer<CylindricalGlobalPotentialGeneratorNode>(GeneratorNode::NodeType::CylindricalGlobalPotential,
                                                              "Add a cylindrical global potential affecting all atoms",
                                                              "Potentials");
    registerProducer<RegionalGlobalPotentialGeneratorNode>(
        GeneratorNode::NodeType::RegionalGlobalPotential, "Add a voxelised global potential affecting all atoms", "Potentials");
    registerProducer<RestraintPotentialGeneratorNode>(GeneratorNode::NodeType::RestraintPotential,
                                                      "Restraint atoms of molecules to their current positions", "Potentials");
    registerProducer<SphericalGlobalPotentialGeneratorNode>(GeneratorNode::NodeType::SphericalGlobalPotential,
                                                            "Add a spherical global potential affecting all atoms",
                                                            "Potentials");

    // Sites
    registerProducer<SelectGeneratorNode>(GeneratorNode::NodeType::Select, "Rotate fragment sites", "Sites");
    registerProducer<RotateFragmentGeneratorNode>(GeneratorNode::NodeType::RotateFragment, "Rotate fragment sites", "Sites");
}

/*
 * Producers
 */

// Produce node of specified type
std::shared_ptr<GeneratorNode> GeneratorNodeRegistry::produce(GeneratorNode::NodeType nodeType) const
{
    auto it = producers_.find(nodeType);
    if (it == producers_.end())
        throw(std::runtime_error(
            fmt::format("A producer has not been registered for node type '{}', so a new instance cannot be created.\n",
                        GeneratorNode::nodeTypes().keyword(nodeType))));

    return (it->second.first)();
}

// Return categorised map of nodes
const std::map<std::string, std::vector<GeneratorNodeRegistry::GeneratorNodeInfoData>> &
GeneratorNodeRegistry::categories() const
{
    return categories_;
}

/*
 * Instance
 */

// Return the producer instance
const GeneratorNodeRegistry &GeneratorNodeRegistry::instance()
{
    static GeneratorNodeRegistry instance;

    return instance;
}

/*
 * GeneratorNode Management
 */

// Return category map
const std::map<std::string, std::vector<GeneratorNodeRegistry::GeneratorNodeInfoData>> &GeneratorNodeRegistry::categoryMap()
{
    return instance().categories();
}

// Create a GeneratorNode instance for the named GeneratorNode type
std::shared_ptr<GeneratorNode> GeneratorNodeRegistry::create(GeneratorNode::NodeType nodeType)
{
    return std::shared_ptr<GeneratorNode>(instance().produce(nodeType));
}
