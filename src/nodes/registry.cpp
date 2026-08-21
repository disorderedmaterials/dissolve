// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodes/registry.h"
#include "nodes/add.h"
#include "nodes/angle.h"
#include "nodes/atomicMC.h"
#include "nodes/averageMolecule.h"
#include "nodes/axisAngle.h"
#include "nodes/bragg.h"
#include "nodes/calculateBonding.h"
#include "nodes/cif/importCIFStructure.h"
#include "nodes/clearBonding.h"
#include "nodes/configuration.h"
#include "nodes/dAngle.h"
#include "nodes/derivative.h"
#include "nodes/detectMolecules.h"
#include "nodes/dotProduct.h"
#include "nodes/edge.h"
#include "nodes/energy.h"
#include "nodes/epsr.h"
#include "nodes/exportBlockData.h"
#include "nodes/exportDLPOLYConfiguration.h"
#include "nodes/exportDLPUtilsPDensData.h"
#include "nodes/exportData.h"
#include "nodes/exportXYZConfiguration.h"
#include "nodes/exportXYZTrajectory.h"
#include "nodes/forcefield.h"
#include "nodes/gr.h"
#include "nodes/histogramCN.h"
#include "nodes/importDLPOLYStructure.h"
#include "nodes/importDLPOLYTrajectory.h"
#include "nodes/importDLPUtilsPDens.h"
#include "nodes/importDLPUtilsSurface.h"
#include "nodes/importEPSRAtoStructure.h"
#include "nodes/importMoscitoStructure.h"
#include "nodes/importXYData.h"
#include "nodes/importXYZStructure.h"
#include "nodes/importXYZTrajectory.h"
#include "nodes/insert.h"
#include "nodes/integrator.h"
#include "nodes/intraAngle.h"
#include "nodes/intraDistance.h"
#include "nodes/intraMC.h"
#include "nodes/iterableGraph.h"
#include "nodes/mc.h"
#include "nodes/md.h"
#include "nodes/modifierOSites.h"
#include "nodes/moleculeTorsion.h"
#include "nodes/multiply.h"
#include "nodes/neutronSQ.h"
#include "nodes/numberNode.h"
#include "nodes/orientedSDF.h"
#include "nodes/qSpecies.h"
#include "nodes/sdf.h"
#include "nodes/setBox.h"
#include "nodes/setCoordinates.h"
#include "nodes/siteRDF.h"
#include "nodes/species.h"
#include "nodes/sq.h"
#include "nodes/subtract.h"
#include "nodes/supercellConfiguration.h"
#include "nodes/vector3Assemble.h"
#include "nodes/vector3Decompose.h"
#include "nodes/voxelDensity.h"
#include "nodes/xRaySQ.h"
#include <memory>
#include <ranges>

// Makes unique pointer to derived node instance
template <typename T> NodeProducer makeDerivedNode()
{
    return [=](Graph *parent) -> std::unique_ptr<Node> { return std::make_unique<T>(parent); };
}

// Static Singletons
const std::map<NodeRegistry::Category, ProducerMap> NodeRegistry::categoricalProducers_{
    {Action,
     {
         {"AtomicMC", makeDerivedNode<AtomicMCNode>()},
         {"GR", makeDerivedNode<GRNode>()},
         {"Insert", makeDerivedNode<InsertNode>()},
         {"MD", makeDerivedNode<MDNode>()},
         {"SQ", makeDerivedNode<SQNode>()},
     }},
    {Data,
     {{"Configuration", makeDerivedNode<ConfigurationNode>()},
      {"Forcefield", makeDerivedNode<ForcefieldNode>()},
      {"Species", makeDerivedNode<SpeciesNode>()}}},
    {Export,
     {{"ExportBlockData", makeDerivedNode<ExportBlockDataNode>()},
      {"ExportData", makeDerivedNode<ExportDataNode>()},
      {"ExportDLPUtilsPDensData", makeDerivedNode<ExportDLPUtilsPDensDataNode>()},
      {"ExportDLPOLYConfiguration", makeDerivedNode<ExportDLPOLYConfigurationNode>()},
      {"ExportXYZTrajectory", makeDerivedNode<ExportXYZTrajectoryNode>()},
      {"ExportXYZConfiguration", makeDerivedNode<ExportXYZConfigurationNode>()}}},
    {Graphs, {{"Iterator", makeDerivedNode<IterableGraph>()}, {"Graph", makeDerivedNode<Graph>()}}},
    {Import,
     {{"ImportCIFStructure", makeDerivedNode<ImportCIFStructureNode>()},
      {"ImportDLPOLYStructure", makeDerivedNode<ImportDLPOLYStructureNode>()},
      {"ImportDLPOLYTrajectory", makeDerivedNode<ImportDLPOLYTrajectoryNode>()},
      {"ImportDLPUtilsPDens", makeDerivedNode<ImportDLPUtilsPDensNode>()},
      {"ImportDLPUtilsSurface", makeDerivedNode<ImportDLPUtilsSurfaceNode>()},
      {"ImportEPSRAtoStructure", makeDerivedNode<ImportEPSRAtoStructureNode>()},
      {"ImportMoscitoStructure", makeDerivedNode<ImportMoscitoStructureNode>()},
      {"ImportXYData", makeDerivedNode<ImportXYDataNode>()},
      {"ImportXYZStructure", makeDerivedNode<ImportXYZStructureNode>()},
      {"ImportXYZTrajectory", makeDerivedNode<ImportXYZTrajectoryNode>()}}},
    {Math,
     {{"Add", makeDerivedNode<AddNode>()},
      {"Derivative", makeDerivedNode<DerivativeNode>()},
      {"DotProduct", makeDerivedNode<DotProductNode>()},
      {"Integrator", makeDerivedNode<Integrator1DNode>()},
      {"Multiply", makeDerivedNode<MultiplyNode>()},
      {"Number", makeDerivedNode<NumberNode>()},
      {"Subtract", makeDerivedNode<SubtractNode>()},
      {"Vector3Assemble", makeDerivedNode<Vector3AssembleNode>()},
      {"Vector3Decompose", makeDerivedNode<Vector3DecomposeNode>()}}},
    {Other,
     {{"Angle", makeDerivedNode<AngleNode>()},
      {"AxisAngle", makeDerivedNode<AxisAngleNode>()},
      {"AverageMolecule", makeDerivedNode<AverageMoleculeNode>()},
      {"Bragg", makeDerivedNode<BraggNode>()},
      {"CalculateBonding", makeDerivedNode<CalculateBondingNode>()},
      {"ClearBonding", makeDerivedNode<ClearBondingNode>()},
      {"DAngle", makeDerivedNode<DAngleNode>()},
      {"DetectMolecules", makeDerivedNode<DetectMoleculesNode>()},
      {"Energy", makeDerivedNode<EnergyNode>()},
      {"EPSR", makeDerivedNode<EPSRNode>()},
      {"HistogramCN", makeDerivedNode<HistogramCNNode>()},
      {"IntraAngle", makeDerivedNode<IntraAngleNode>()},
      {"IntraDistance", makeDerivedNode<IntraDistanceNode>()},
      {"IntraMC", makeDerivedNode<IntraMCNode>()},
      {"MC", makeDerivedNode<MCNode>()},
      {"ModifierOSites", makeDerivedNode<ModifierOSitesNode>()},
      {"MoleculeTorsion", makeDerivedNode<MoleculeTorsionNode>()},
      {"NeutronSQ", makeDerivedNode<NeutronSQNode>()},
      {"OrientedSDF", makeDerivedNode<OrientedSDFNode>()},
      {"QSpecies", makeDerivedNode<QSpeciesNode>()},
      {"SDF", makeDerivedNode<SDFNode>()},
      {"SetBox", makeDerivedNode<SetBoxNode>()},
      {"SetCoordinates", makeDerivedNode<SetCoordinatesNode>()},
      {"SiteRDF", makeDerivedNode<SiteRDFNode>()},
      {"SupercellConfiguration", makeDerivedNode<SupercellConfigurationNode>()},
      {"XRaySQ", makeDerivedNode<XRaySQNode>()},
      {"VoxelDensity", makeDerivedNode<VoxelDensityNode>()}}}};

ProducerMap NodeRegistry::producers_;

// Return enum option info for Category
EnumOptions<NodeRegistry::Category> NodeRegistry::category()
{
    return EnumOptions<NodeRegistry::Category>("Category", {
                                                               {Category::Action, "Action"},
                                                               {Category::Data, "Data"},
                                                               {Category::Export, "Export"},
                                                               {Category::Graphs, "Graphs"},
                                                               {Category::Import, "Import"},
                                                               {Category::Math, "Math"},
                                                               {Category::Other, "Other (TODO: finish categories)"},
                                                           });
}
EnumOptions<NodeRegistry::Category> getEnumOptions(NodeRegistry::Category) { return NodeRegistry::category(); }

// Remove categories from categorial node producer map, returning a 'flat' producer map
ProducerMap NodeRegistry::decategoriseProducers()
{
    ProducerMap producers;
    for (const auto &[_, map] : categoricalProducers_)
        producers.insert(map.begin(), map.end());

    return producers;
}

// Instantiate Node Producers
void NodeRegistry::instantiateNodeProducers()
{
    // Only need to do this once
    if (!producers_.empty())
        return;

    producers_ = decategoriseProducers();
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

// Return producers
const std::map<std::string_view, NodeProducer> &NodeRegistry::producers() { return producers_; }
