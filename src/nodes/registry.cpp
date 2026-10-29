// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodes/registry.h"
#include "nodes/analysis/angle.h"
#include "nodes/analysis/averageMolecule.h"
#include "nodes/analysis/axisAngle.h"
#include "nodes/analysis/dAngle.h"
#include "nodes/analysis/histogramCN.h"
#include "nodes/analysis/intraAngle.h"
#include "nodes/analysis/intraDistance.h"
#include "nodes/analysis/modifierOSites.h"
#include "nodes/analysis/moleculeTorsion.h"
#include "nodes/analysis/orientedSDF.h"
#include "nodes/analysis/qSpecies.h"
#include "nodes/analysis/sdf.h"
#include "nodes/analysis/siteRDF.h"
#include "nodes/analysis/voxelDensity.h"
#include "nodes/correlation/bragg.h"
#include "nodes/correlation/gr.h"
#include "nodes/correlation/neutronSQ.h"
#include "nodes/correlation/sq.h"
#include "nodes/correlation/xRaySQ.h"
#include "nodes/edge.h"
#include "nodes/energy.h"
#include "nodes/epsr.h"
#include "nodes/evolution/atomicMC.h"
#include "nodes/evolution/intraMC.h"
#include "nodes/evolution/mc.h"
#include "nodes/evolution/md.h"
#include "nodes/export/blockData.h"
#include "nodes/export/data.h"
#include "nodes/export/dlpolyConfiguration.h"
#include "nodes/export/dlputilsPDensData.h"
#include "nodes/export/xyzConfiguration.h"
#include "nodes/export/xyzTrajectory.h"
#include "nodes/forcefield.h"
#include "nodes/import/cifStructure.h"
#include "nodes/import/dlpolyStructure.h"
#include "nodes/import/dlpolyTrajectory.h"
#include "nodes/import/dlputilsPDens.h"
#include "nodes/import/dlputilsSurface.h"
#include "nodes/import/epsrAtoStructure.h"
#include "nodes/import/moscitoStructure.h"
#include "nodes/import/species.h"
#include "nodes/import/xyData.h"
#include "nodes/import/xyzStructure.h"
#include "nodes/import/xyzTrajectory.h"
#include "nodes/iterableGraph.h"
#include "nodes/math/add.h"
#include "nodes/math/derivative.h"
#include "nodes/math/dotProduct.h"
#include "nodes/math/integrator.h"
#include "nodes/math/multiply.h"
#include "nodes/math/number.h"
#include "nodes/math/subtract.h"
#include "nodes/math/vector3Assemble.h"
#include "nodes/math/vector3Decompose.h"
#include "nodes/structure/atomicSpecies.h"
#include "nodes/structure/calculateBonding.h"
#include "nodes/structure/clearBonding.h"
#include "nodes/structure/configuration.h"
#include "nodes/structure/detectMolecules.h"
#include "nodes/structure/insertRandom.h"
#include "nodes/structure/instantiate.h"
#include "nodes/structure/replicatedConfiguration.h"
#include "nodes/structure/setBox.h"
#include "nodes/structure/setCoordinates.h"
#include "nodes/structure/species.h"
#include <memory>
#include <ranges>

// Makes unique pointer to derived node instance
template <typename T> NodeProducer makeDerivedNode()
{
    return [=](Graph *parent) -> std::unique_ptr<Node> { return std::make_unique<T>(parent); };
}

// Static Singletons
const std::map<NodeRegistry::Category, ProducerMap> NodeRegistry::categoricalProducers_{
    {Analysis,
     {{"Angle", makeDerivedNode<AngleNode>()},
      {"AverageMolecule", makeDerivedNode<AverageMoleculeNode>()},
      {"AxisAngle", makeDerivedNode<AxisAngleNode>()},
      {"DAngle", makeDerivedNode<DAngleNode>()},
      {"EPSR", makeDerivedNode<EPSRNode>()},
      {"HistogramCN", makeDerivedNode<HistogramCNNode>()},
      {"IntraAngle", makeDerivedNode<IntraAngleNode>()},
      {"IntraDistance", makeDerivedNode<IntraDistanceNode>()},
      {"ModifierOSites", makeDerivedNode<ModifierOSitesNode>()},
      {"MoleculeTorsion", makeDerivedNode<MoleculeTorsionNode>()},
      {"OrientedSDF", makeDerivedNode<OrientedSDFNode>()},
      {"QSpecies", makeDerivedNode<QSpeciesNode>()},
      {"SDF", makeDerivedNode<SDFNode>()},
      {"SiteRDF", makeDerivedNode<SiteRDFNode>()},
      {"VoxelDensity", makeDerivedNode<VoxelDensityNode>()}}},
    {Correlation,
     {
         {"Bragg", makeDerivedNode<BraggNode>()},
         {"GR", makeDerivedNode<GRNode>()},
         {"NeutronSQ", makeDerivedNode<NeutronSQNode>()},
         {"SQ", makeDerivedNode<SQNode>()},
         {"XRaySQ", makeDerivedNode<XRaySQNode>()},
     }},
    {Evolution,
     {
         {"AtomicMC", makeDerivedNode<AtomicMCNode>()},
         {"IntraMC", makeDerivedNode<IntraMCNode>()},
         {"MC", makeDerivedNode<MCNode>()},
         {"MD", makeDerivedNode<MDNode>()},
     }},
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
      {"ImportSpecies", makeDerivedNode<ImportSpeciesNode>()},
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
    {Structure,
     {{"AtomicSpecies", makeDerivedNode<AtomicSpeciesNode>()},
      {"CalculateBonding", makeDerivedNode<CalculateBondingNode>()},
      {"ClearBonding", makeDerivedNode<ClearBondingNode>()},
      {"Configuration", makeDerivedNode<ConfigurationNode>()},
      {"DetectMolecules", makeDerivedNode<DetectMoleculesNode>()},
      {"Forcefield", makeDerivedNode<ForcefieldNode>()},
      {"InsertRandom", makeDerivedNode<InsertRandomNode>()},
      {"Instantiate", makeDerivedNode<InstantiateNode>()},
      {"ReplicatedConfiguration", makeDerivedNode<ReplicatedConfigurationNode>()},
      {"SetBox", makeDerivedNode<SetBoxNode>()},
      {"SetCoordinates", makeDerivedNode<SetCoordinatesNode>()},
      {"Species", makeDerivedNode<SpeciesNode>()}}}};

ProducerMap NodeRegistry::producers_;

// Return enum option info for Category
EnumOptions<NodeRegistry::Category> NodeRegistry::category()
{
    return EnumOptions<NodeRegistry::Category>("Category", {
                                                               {Category::Analysis, "Analysis"},
                                                               {Category::Correlation, "Correlation"},
                                                               {Category::Evolution, "Evolution"},
                                                               {Category::Export, "Export"},
                                                               {Category::Graphs, "Graphs"},
                                                               {Category::Import, "Import"},
                                                               {Category::Math, "Math"},
                                                               {Category::Structure, "Structure"},
                                                           });
}
EnumOptions<NodeRegistry::Category> getEnumOptions(NodeRegistry::Category) { return NodeRegistry::category(); }

// Remove categories from categorical node producer map, returning a 'flat' producer map
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
