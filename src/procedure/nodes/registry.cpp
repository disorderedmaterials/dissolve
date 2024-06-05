// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "procedure/nodes/registry.h"
#include "procedure/nodes/add.h"
#include "procedure/nodes/addPair.h"
#include "procedure/nodes/box.h"
#include "procedure/nodes/coordinateSets.h"
#include "procedure/nodes/copy.h"
#include "procedure/nodes/customRegion.h"
#include "procedure/nodes/cylindricalGlobalPotential.h"
#include "procedure/nodes/cylindricalRegion.h"
#include "procedure/nodes/generalRegion.h"
#include "procedure/nodes/importCoordinates.h"
#include "procedure/nodes/parameters.h"
#include "procedure/nodes/pick.h"
#include "procedure/nodes/pickProximity.h"
#include "procedure/nodes/pickRegion.h"
#include "procedure/nodes/regionalGlobalPotential.h"
#include "procedure/nodes/remove.h"
#include "procedure/nodes/restraintPotential.h"
#include "procedure/nodes/rotateFragment.h"
#include "procedure/nodes/runLayer.h"
#include "procedure/nodes/select.h"
#include "procedure/nodes/sizeFactor.h"
#include "procedure/nodes/sphericalGlobalPotential.h"
#include "procedure/nodes/temperature.h"
#include "procedure/nodes/transmute.h"

ProcedureNodeRegistry::ProcedureNodeRegistry()
{
    // Build
    registerProducer<AddProcedureNode>(ProcedureNode::NodeType::Add, "Add molecules to a configuration", "Build");
    registerProducer<AddPairProcedureNode>(ProcedureNode::NodeType::AddPair,
                                           "Add a correlated molecule pair to a configuration", "Build");
    registerProducer<BoxProcedureNode>(ProcedureNode::NodeType::Box, "Define containing box for a configuration", "Build");
    registerProducer<CoordinateSetsProcedureNode>(ProcedureNode::NodeType::CoordinateSets,
                                                  "Generate coordinate sets for a species", "Build");
    registerProducer<CopyProcedureNode>(ProcedureNode::NodeType::Copy, "Copy the contents of a configuration", "Build");
    registerProducer<ImportCoordinatesProcedureNode>(ProcedureNode::NodeType::ImportCoordinates,
                                                     "Import coordinates into a configuration", "Build");
    registerProducer<RemoveProcedureNode>(ProcedureNode::NodeType::Remove, "Remove molecules from a configuration", "Build");
    registerProducer<SizeFactorProcedureNode>(ProcedureNode::NodeType::SizeFactor,
                                              "Scale a configuration's contents with a size factor", "Build");
    registerProducer<TemperatureProcedureNode>(ProcedureNode::NodeType::Temperature, "Set temperature for configuration",
                                               "Build");
    registerProducer<TransmuteProcedureNode>(ProcedureNode::NodeType::Transmute,
                                             "Turn molecules from one species type into another", "Build");

    // General
    registerProducer<ParametersProcedureNode>(ProcedureNode::NodeType::Parameters, "Define parameters for use in expressions",
                                              "General");

    // Regions
    registerProducer<CustomRegionProcedureNode>(ProcedureNode::NodeType::CustomRegion,
                                                "Generate a custom region based on an expression", "Regions");
    registerProducer<CylindricalRegionProcedureNode>(ProcedureNode::NodeType::CylindricalRegion,
                                                     "Define a cylindrical region in a configuration", "Regions");
    registerProducer<GeneralRegionProcedureNode>(ProcedureNode::NodeType::GeneralRegion,
                                                 "Define a general region in a configuration", "Regions");

    // Pick
    registerProducer<PickProcedureNode>(ProcedureNode::NodeType::Pick, "Pick all molecules of a given species", "Pick");
    registerProducer<PickProximityProcedureNode>(ProcedureNode::NodeType::PickProximity,
                                                 "Pick molecules based on proximity to others", "Pick");
    registerProducer<PickRegionProcedureNode>(ProcedureNode::NodeType::PickRegion, "Pick molecules within a specific region",
                                              "Pick");

    // Potentials
    registerProducer<CylindricalGlobalPotentialProcedureNode>(ProcedureNode::NodeType::CylindricalGlobalPotential,
                                                              "Add a cylindrical global potential affecting all atoms",
                                                              "Potentials");
    registerProducer<RegionalGlobalPotentialProcedureNode>(
        ProcedureNode::NodeType::RegionalGlobalPotential, "Add a voxelised global potential affecting all atoms", "Potentials");
    registerProducer<RestraintPotentialProcedureNode>(ProcedureNode::NodeType::RestraintPotential,
                                                      "Restraint atoms of molecules to their current positions", "Potentials");
    registerProducer<SphericalGlobalPotentialProcedureNode>(ProcedureNode::NodeType::SphericalGlobalPotential,
                                                            "Add a spherical global potential affecting all atoms",
                                                            "Potentials");

    // Sites
    registerProducer<SelectProcedureNode>(ProcedureNode::NodeType::Select, "Rotate fragment sites", "Sites");
    registerProducer<RotateFragmentProcedureNode>(ProcedureNode::NodeType::RotateFragment, "Rotate fragment sites", "Sites");

    // Control
    registerProducer<RunLayerNode>(ProcedureNode::NodeType::RunLayer, "Run layer", "Control");
}

/*
 * Producers
 */

// Produce node of specified type
std::shared_ptr<ProcedureNode> ProcedureNodeRegistry::produce(ProcedureNode::NodeType nodeType) const
{
    auto it = producers_.find(nodeType);
    if (it == producers_.end())
        throw(std::runtime_error(
            fmt::format("A producer has not been registered for node type '{}', so a new instance cannot be created.\n",
                        ProcedureNode::nodeTypes().keyword(nodeType))));

    return (it->second.first)();
}

// Return categorised map of nodes
const std::map<std::string, std::vector<ProcedureNodeRegistry::ProcedureNodeInfoData>> &
ProcedureNodeRegistry::categories() const
{
    return categories_;
}

/*
 * Instance
 */

// Return the producer instance
const ProcedureNodeRegistry &ProcedureNodeRegistry::instance()
{
    static ProcedureNodeRegistry instance;

    return instance;
}

/*
 * ProcedureNode Management
 */

// Return category map
const std::map<std::string, std::vector<ProcedureNodeRegistry::ProcedureNodeInfoData>> &ProcedureNodeRegistry::categoryMap()
{
    return instance().categories();
}

// Create a ProcedureNode instance for the named ProcedureNode type
std::shared_ptr<ProcedureNode> ProcedureNodeRegistry::create(ProcedureNode::NodeType nodeType)
{
    return std::shared_ptr<ProcedureNode>(instance().produce(nodeType));
}
