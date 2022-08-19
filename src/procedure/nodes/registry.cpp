// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#include "procedure/nodes/registry.h"
#include "procedure/nodes/nodes.h"

ProcedureNodeRegistry::ProcedureNodeRegistry()
{
    registerProducer<AddProcedureNode>(ProcedureNode::NodeType::Add, "Add molecules to a configuration", "Build");
    registerProducer<AddPairProcedureNode>(ProcedureNode::NodeType::AddPair,
                                           "Add a correlated molecule pair to a configuration", "Build");
    registerProducer<BoxProcedureNode>(ProcedureNode::NodeType::Box, "Define containing box for a configuration", "Build");
    registerProducer<CalculateAngleProcedureNode>(ProcedureNode::NodeType::CalculateAngle,
                                                  "Calculate angle between three sites", "Calculate");
    registerProducer<CalculateDistanceProcedureNode>(ProcedureNode::NodeType::CalculateDistance,
                                                     "Calculate distance between two sites", "Calculate");
    registerProducer<CalculateVectorProcedureNode>(ProcedureNode::NodeType::CalculateVector,
                                                   "Calculate vector between two sites", "Calculate");
    registerProducer<Collect1DProcedureNode>(ProcedureNode::NodeType::Collect1D, "Bin 1D quantity into a histogram", "Data");
    registerProducer<Collect2DProcedureNode>(ProcedureNode::NodeType::Collect2D, "Bin 2D quantity into a histogram", "Data");
    registerProducer<Collect3DProcedureNode>(ProcedureNode::NodeType::Collect3D, "Bin 3D quantity into a histogram", "Data");
    registerProducer<CoordinateSetsProcedureNode>(ProcedureNode::NodeType::CoordinateSets,
                                                  "Generate coordinate sets for a species", "Build");
    registerProducer<CopyProcedureNode>(ProcedureNode::NodeType::Copy, "Copy the contents of a configuration", "Build");
    registerProducer<CylindricalRegionProcedureNode>(ProcedureNode::NodeType::CylindricalRegion,
                                                     "Define a cylindrical region in a configuration", "Regions");
    registerProducer<GeneralRegionProcedureNode>(ProcedureNode::NodeType::GeneralRegion,
                                                 "Define a general region in a configuration", "Regions");
    registerProducer<OperateDivideProcedureNode>(ProcedureNode::NodeType::OperateDivide, "Perform a division on data",
                                                 "Operate");
    registerProducer<OperateExpressionProcedureNode>(ProcedureNode::NodeType::OperateExpression, "Apply an expression to data",
                                                     "Operate");
    registerProducer<OperateMultiplyProcedureNode>(ProcedureNode::NodeType::OperateMultiply, "Perform a multiplication on data",
                                                   "Operate");
    registerProducer<OperateNormaliseProcedureNode>(ProcedureNode::NodeType::OperateNormalise, "Normalise data to a value",
                                                    "Operate");
    registerProducer<OperateNumberDensityNormaliseProcedureNode>(ProcedureNode::NodeType::OperateNumberDensityNormalise,
                                                                 "Normalise data to a reference number density", "Operate");
    registerProducer<OperateSitePopulationNormaliseProcedureNode>(ProcedureNode::NodeType::OperateSitePopulationNormalise,
                                                                  "Normalise data to a reference site population", "Operate");
    registerProducer<OperateSphericalShellNormaliseProcedureNode>(ProcedureNode::NodeType::OperateSphericalShellNormalise,
                                                                  "Normalise data to spherical shell volumes", "Operate");
    registerProducer<ParametersProcedureNode>(ProcedureNode::NodeType::Parameters, "Define parameters for use in expressions",
                                              "General");
    registerProducer<PickProcedureNode>(ProcedureNode::NodeType::Pick, "Pick all molecules of a given species", "Pick");
    registerProducer<PickProximityProcedureNode>(ProcedureNode::NodeType::PickProximity,
                                                 "Pick molecules based on proximity to others", "Pick");
    registerProducer<PickRegionProcedureNode>(ProcedureNode::NodeType::PickRegion, "Pick molecules within a specific region",
                                              "Pick");
    registerProducer<Process1DProcedureNode>(ProcedureNode::NodeType::Process1D, "Process 1D histogram data", "Data");
    registerProducer<Process2DProcedureNode>(ProcedureNode::NodeType::Process2D, "Process 2D histogram data", "Data");
    registerProducer<Process3DProcedureNode>(ProcedureNode::NodeType::Process3D, "Process 3D histogram data", "Data");
    registerProducer<RemoveProcedureNode>(ProcedureNode::NodeType::Remove, "Remove molecules from a configuration", "Build");
    registerProducer<SelectProcedureNode>(ProcedureNode::NodeType::Select, "Select sites for consideration", "Calculate");
    registerProducer<TransmuteProcedureNode>(ProcedureNode::NodeType::Transmute,
                                             "Turn molecules from one species type into another", "Build");
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
