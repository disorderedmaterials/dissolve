// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodes/registry.h"
#include "math/averaging.h"
#include "nodes/add.h"
#include "nodes/angle.h"
#include "nodes/atomicMC/atomicMC.h"
#include "nodes/averageMolecule.h"
#include "nodes/axisAngle.h"
#include "nodes/bragg.h"
#include "nodes/calculateBonding.h"
#include "nodes/cif/importCIFStructure.h"
#include "nodes/clearBonding.h"
#include "nodes/configuration.h"
#include "nodes/dAngle.h"
#include "nodes/data1DImport.h"
#include "nodes/derivative.h"
#include "nodes/dotProduct.h"
#include "nodes/edge.h"
#include "nodes/energy.h"
#include "nodes/epsr.h"
#include "nodes/exportDLPOLYConfiguration.h"
#include "nodes/exportXYZConfiguration.h"
#include "nodes/exportXYZTrajectory.h"
#include "nodes/forcefield.h"
#include "nodes/gr/gr.h"
#include "nodes/histogramCN.h"
#include "nodes/importDLPOLYStructure.h"
#include "nodes/importDLPOLYTrajectory.h"
#include "nodes/importEPSRAtoStructure.h"
#include "nodes/importMoscitoStructure.h"
#include "nodes/importXYZStructure.h"
#include "nodes/importXYZTrajectory.h"
#include "nodes/insert.h"
#include "nodes/integrator.h"
#include "nodes/intraAngle.h"
#include "nodes/intraDistance.h"
#include "nodes/iterableGraph.h"
#include "nodes/md.h"
#include "nodes/modifierOSites.h"
#include "nodes/moleculeTorsion.h"
#include "nodes/multiply.h"
#include "nodes/neutronSQ/neutronSQ.h"
#include "nodes/numberNode.h"
#include "nodes/orientedSDF.h"
#include "nodes/qSpecies.h"
#include "nodes/sdf.h"
#include "nodes/setBox.h"
#include "nodes/setCoordinates.h"
#include "nodes/siteRDF.h"
#include "nodes/species.h"
#include "nodes/sq/sq.h"
#include "nodes/subtract.h"
#include "nodes/vec3Assembly.h"
#include "nodes/vec3Decomposition.h"
#include "nodes/voxelDensity.h"
#include "nodes/xRaySQ/xRaySQ.h"
#include <memory>
#include <ranges>

// Static Singletons
std::map<std::string_view, NodeProducer> NodeRegistry::producers_;

// Makes unique pointer to derived node instance
template <typename T> NodeProducer makeDerivedNode()
{
    return [=](Graph *parent) -> std::unique_ptr<Node> { return std::make_unique<T>(parent); };
}

// Instantiate Node Producers
void NodeRegistry::instantiateNodeProducers()
{
    // Only need to do this once
    if (!producers_.empty())
        return;

    producers_ = {{"Add", makeDerivedNode<AddNode>()},
                  {"Angle", makeDerivedNode<AngleNode>()},
                  {"AxisAngle", makeDerivedNode<AxisAngleNode>()},
                  {"AtomicMC", makeDerivedNode<AtomicMCNode>()},
                  {"AverageMolecule", makeDerivedNode<AverageMoleculeNode>()},
                  {"Bragg", makeDerivedNode<BraggNode>()},
                  {"CalculateBonding", makeDerivedNode<CalculateBondingNode>()},
                  {"ClearBonding", makeDerivedNode<ClearBondingNode>()},
                  {"Configuration", makeDerivedNode<ConfigurationNode>()},
                  {"ImportCIFStructure", makeDerivedNode<ImportCIFStructureNode>()},
                  {"DAngle", makeDerivedNode<DAngleNode>()},
                  {"Data1DImport", makeDerivedNode<Data1DImportNode>()},
                  {"Derivative", makeDerivedNode<DerivativeNode>()},
                  {"DotProduct", makeDerivedNode<DotProductNode>()},
                  {"Energy", makeDerivedNode<EnergyNode>()},
                  {"EPSR", makeDerivedNode<EPSRNode>()},
                  {"ExportDLPOLYConfiguration", makeDerivedNode<ExportDLPOLYConfigurationNode>()},
                  {"ExportXYZTrajectory", makeDerivedNode<ExportXYZTrajectoryNode>()},
                  {"ExportXYZConfiguration", makeDerivedNode<ExportXYZConfigurationNode>()},
                  {"Forcefield", makeDerivedNode<ForcefieldNode>()},
                  {"Graph", makeDerivedNode<Graph>()},
                  {"GR", makeDerivedNode<GRNode>()},
                  {"HistogramCN", makeDerivedNode<HistogramCNNode>()},
                  {"ImportDLPOLYStructure", makeDerivedNode<ImportDLPOLYStructureNode>()},
                  {"ImportDLPOLYTrajectory", makeDerivedNode<ImportDLPOLYTrajectoryNode>()},
                  {"ImportEPSRAtoStructure", makeDerivedNode<ImportEPSRAtoStructureNode>()},
                  {"ImportMoscitoStructure", makeDerivedNode<ImportMoscitoStructureNode>()},
                  {"ImportXYZStructure", makeDerivedNode<ImportXYZStructureNode>()},
                  {"ImportXYZTrajectory", makeDerivedNode<ImportXYZTrajectoryNode>()},
                  {"Insert", makeDerivedNode<InsertNode>()},
                  {"Integrator", makeDerivedNode<Integrator1DNode>()},
                  {"IntraAngle", makeDerivedNode<IntraAngleNode>()},
                  {"IntraDistance", makeDerivedNode<IntraDistanceNode>()},
                  {"Iterator", makeDerivedNode<IterableGraph>()},
                  {"MD", makeDerivedNode<MDNode>()},
                  {"ModifierOSites", makeDerivedNode<ModifierOSitesNode>()},
                  {"MoleculeTorsion", makeDerivedNode<MoleculeTorsionNode>()},
                  {"Multiply", makeDerivedNode<MultiplyNode>()},
                  {"NeutronSQ", makeDerivedNode<NeutronSQNode>()},
                  {"Number", makeDerivedNode<NumberNode>()},
                  {"OrientedSDF", makeDerivedNode<OrientedSDFNode>()},
                  {"QSpecies", makeDerivedNode<QSpeciesNode>()},
                  {"SDF", makeDerivedNode<SDFNode>()},
                  {"SetBox", makeDerivedNode<SetBoxNode>()},
                  {"SetCoordinates", makeDerivedNode<SetCoordinatesNode>()},
                  {"SiteRDF", makeDerivedNode<SiteRDFNode>()},
                  {"SQ", makeDerivedNode<SQNode>()},
                  {"Species", makeDerivedNode<SpeciesNode>()},
                  {"Subtract", makeDerivedNode<SubtractNode>()},
                  {"Vec3Assembly", makeDerivedNode<Vec3AssemblyNode>()},
                  {"Vec3Decomposition", makeDerivedNode<Vec3DecompositionNode>()},
                  {"XRaySQ", makeDerivedNode<XRaySQNode>()},
                  {"VoxelDensity", makeDerivedNode<VoxelDensityNode>()}};
}

// Check whether the supplied node type is known
bool NodeRegistry::hasNodeType(std::string_view nodeType)
{
    instantiateNodeProducers();
    return producers_.contains(nodeType);
}

// Search for the supplied node type, returning strict node type if found
std::vector<std::string_view> NodeRegistry::getNodeTypesFuzzy(std::string_view weakNodeType)
{
    instantiateNodeProducers();

    using namespace std::string_literals;

    auto predicate = [weakNodeType](const auto nodeType)
    { return DissolveSys::sameWildString("*"s + std::string(weakNodeType) + "*"s, nodeType); };

    // Iterate over the keys of the map and include only the names
    // which match our fuzzy match
    auto range = producers_ | std::views::keys | std::views::filter(predicate);

    // Create a vector from the range
    return {range.begin(), range.end()};
}

// Produce a node of the given type with the specified Graph parent
std::unique_ptr<Node> NodeRegistry::produce(Graph *parent, std::string_view nodeType)
{
    instantiateNodeProducers();

    // Check for valid node type and produce
    if (!producers_.contains(nodeType))
        Messenger::exception("Attempted to create node of unknown type: {}\n", nodeType);

    return producers_.at(nodeType)(parent);
}
