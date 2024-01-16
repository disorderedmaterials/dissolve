// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "procedure/nodes/registry.h"
#include "procedure/nodes/add.h"
#include "procedure/nodes/addPair.h"
#include "procedure/nodes/box.h"
#include "procedure/nodes/calculateAngle.h"
#include "procedure/nodes/calculateAxisAngle.h"
#include "procedure/nodes/calculateDistance.h"
#include "procedure/nodes/calculateExpression.h"
#include "procedure/nodes/calculateVector.h"
#include "procedure/nodes/collect1D.h"
#include "procedure/nodes/collect2D.h"
#include "procedure/nodes/collect3D.h"
#include "procedure/nodes/coordinateSets.h"
#include "procedure/nodes/copy.h"
#include "procedure/nodes/customRegion.h"
#include "procedure/nodes/cylindricalRegion.h"
#include "procedure/nodes/directionalGlobalPotential.h"
#include "procedure/nodes/generalRegion.h"
#include "procedure/nodes/ifValueInRange.h"
#include "procedure/nodes/importCoordinates.h"
#include "procedure/nodes/integerCollect1D.h"
#include "procedure/nodes/integrate1D.h"
#include "procedure/nodes/operateDivide.h"
#include "procedure/nodes/operateExpression.h"
#include "procedure/nodes/operateGridNormalise.h"
#include "procedure/nodes/operateMultiply.h"
#include "procedure/nodes/operateNormalise.h"
#include "procedure/nodes/operateNumberDensityNormalise.h"
#include "procedure/nodes/operateSitePopulationNormalise.h"
#include "procedure/nodes/operateSphericalShellNormalise.h"
#include "procedure/nodes/parameters.h"
#include "procedure/nodes/pick.h"
#include "procedure/nodes/pickProximity.h"
#include "procedure/nodes/pickRegion.h"
#include "procedure/nodes/process1D.h"
#include "procedure/nodes/process2D.h"
#include "procedure/nodes/process3D.h"
#include "procedure/nodes/regionalGlobalPotential.h"
#include "procedure/nodes/remove.h"
#include "procedure/nodes/restraintPotential.h"
#include "procedure/nodes/rotateFragment.h"
#include "procedure/nodes/runLayer.h"
#include "procedure/nodes/select.h"
#include "procedure/nodes/simpleGlobalPotential.h"
#include "procedure/nodes/sizeFactor.h"
#include "procedure/nodes/sum1D.h"
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

    // Calculate
    registerProducer<CalculateAngleProcedureNode>(ProcedureNode::NodeType::CalculateAngle,
                                                  "Calculate angle between three sites", "Calculate");
    registerProducer<CalculateAxisAngleProcedureNode>(ProcedureNode::NodeType::CalculateAxisAngle,
                                                      "Calculate angle between specified axes of two oriented sites",
                                                      "Calculate");
    registerProducer<CalculateDistanceProcedureNode>(ProcedureNode::NodeType::CalculateDistance,
                                                     "Calculate distance between two sites", "Calculate");
    registerProducer<CalculateExpressionProcedureNode>(ProcedureNode::NodeType::CalculateExpression,
                                                       "Evaluate an arbitrary expression", "Calculate");
    registerProducer<CalculateVectorProcedureNode>(ProcedureNode::NodeType::CalculateVector,
                                                   "Calculate vector between two sites", "Calculate");
    registerProducer<SelectProcedureNode>(ProcedureNode::NodeType::Select, "Select sites for consideration", "Calculate");

    // Control
    registerProducer<IfValueInRangeProcedureNode>(ProcedureNode::NodeType::IfValueInRange,
                                                  "Conditionally execute other nodes if a value is within range", "Control");

    // Data
    registerProducer<Collect1DProcedureNode>(ProcedureNode::NodeType::Collect1D, "Bin 1D quantity into a histogram", "Data");
    registerProducer<Collect2DProcedureNode>(ProcedureNode::NodeType::Collect2D, "Bin 2D quantity into a histogram", "Data");
    registerProducer<Collect3DProcedureNode>(ProcedureNode::NodeType::Collect3D, "Bin 3D quantity into a histogram", "Data");
    registerProducer<IntegerCollect1DProcedureNode>(ProcedureNode::NodeType::IntegerCollect1D,
                                                    "Bin integer 1D value into a histogram", "Data");
    registerProducer<Integrate1DProcedureNode>(ProcedureNode::NodeType::Integrate1D, "Integrate the data in a 1D dataset",
                                               "Data");
    registerProducer<Process1DProcedureNode>(ProcedureNode::NodeType::Process1D, "Process 1D histogram data", "Data");
    registerProducer<Process2DProcedureNode>(ProcedureNode::NodeType::Process2D, "Process 2D histogram data", "Data");
    registerProducer<Process3DProcedureNode>(ProcedureNode::NodeType::Process3D, "Process 3D histogram data", "Data");
    registerProducer<Sum1DProcedureNode>(ProcedureNode::NodeType::Sum1D, "Sum the data in a 1D dataset", "Data");

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

    // Operate
    registerProducer<OperateDivideProcedureNode>(ProcedureNode::NodeType::OperateDivide, "Perform a division on data",
                                                 "Operate");
    registerProducer<OperateExpressionProcedureNode>(ProcedureNode::NodeType::OperateExpression, "Apply an expression to data",
                                                     "Operate");
    registerProducer<OperateGridNormaliseProcedureNode>(ProcedureNode::NodeType::OperateGridNormalise,
                                                        "Normalise data by bin widths, areas, or volumes", "Operate");
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

    // Pick
    registerProducer<PickProcedureNode>(ProcedureNode::NodeType::Pick, "Pick all molecules of a given species", "Pick");
    registerProducer<PickProximityProcedureNode>(ProcedureNode::NodeType::PickProximity,
                                                 "Pick molecules based on proximity to others", "Pick");
    registerProducer<PickRegionProcedureNode>(ProcedureNode::NodeType::PickRegion, "Pick molecules within a specific region",
                                              "Pick");

    // Potentials
    registerProducer<DirectionalGlobalPotentialProcedureNode>(ProcedureNode::NodeType::DirectionalGlobalPotential,
                                                              "Add a directional global potential affecting all atoms",
                                                              "Potentials");
    registerProducer<RegionalGlobalPotentialProcedureNode>(
        ProcedureNode::NodeType::RegionalGlobalPotential, "Add a voxelised global potential affecting all atoms", "Potentials");
    registerProducer<RestraintPotentialProcedureNode>(ProcedureNode::NodeType::RestraintPotential,
                                                      "Restraint atoms of molecules to their current positions", "Potentials");
    registerProducer<SimpleGlobalPotentialProcedureNode>(ProcedureNode::NodeType::SimpleGlobalPotential,
                                                         "Add a global potential affecting all atoms", "Potentials");

    // Sites
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
