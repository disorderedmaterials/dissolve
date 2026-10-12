// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "tests/testGraph.h"
#include "classes/isotopologueSet.h"
#include "main/dissolve.h"
#include "nodes/atomicSpecies.h"
#include "nodes/bragg.h"
#include "nodes/dissolve.h"
#include "nodes/forcefield.h"
#include "nodes/gr.h"
#include "nodes/importSpecies.h"
#include "nodes/insertRandom.h"
#include "nodes/iterableGraph.h"
#include "nodes/neutronSQ.h"
#include "nodes/setCoordinates.h"
#include "nodes/species.h"
#include "nodes/sq.h"
#include "nodes/xRaySQ.h"
#include <gtest/gtest.h>

namespace UnitTest
{
/*
 * Graph Creation Helpers
 */

// Returns pointer to current top node in graph
Node *TestGraph::fetchHead() const { return head_; }
// Returns the name of the current head node in the graph
std::string TestGraph::fetchHeadName() const { return head_ ? std::string(head_->name()) : "NO_NODE"; }
// Run the graph in a piecewise manner - initially from a specific node, then from the last node - in order to emplace a set
// of dynamic edges that we expect to exist at run time
NodeConstants::ProcessResult TestGraph::runDynamic(Node *startNode, std::vector<EdgeDefinition> edges) const
{
    currentGraph_->setUpdateRequired();

    auto result = NodeConstants::ProcessResult::Unchanged;
    result = startNode->run();
    if (result == NodeConstants::ProcessResult::Failed)
        return result;

    for (const auto &edge : edges)
        if (!currentGraph_->addEdge(edge) ||
            currentGraph_->findNode(edge.targetNode)->run() == NodeConstants::ProcessResult::Failed)
            return NodeConstants::ProcessResult::Failed;

    return result;
}
// Append new node to the graph
Node *TestGraph::appendNode(const std::string &nodeType, const std::optional<std::string> &name)
{
    auto node = name.has_value() ? currentGraph_->createNode(nodeType, *name) : currentGraph_->createNode(nodeType);

    if (!node)
        return nullptr;

    head_ = node;

    return node;
}
// Create species insertion node chain
Node *TestGraph::createAndInsertSpecies(Node *cfgSourceNode, std::string cfgSourceOutput,
                                        const std::vector<std::pair<const Species *, int>> &species, double rho,
                                        Units::DensityUnits rhoUnits, InsertRandomNode::BoxScalingType boxScalingType)
{
    // Add Species and Insert nodes
    for (auto &[speciesPtr, population] : species)
    {
        auto insertNodeName = std::format("Insert-{}", speciesPtr->name());
        EXPECT_TRUE(appendNode("InsertRandom", insertNodeName));
        EXPECT_TRUE(fetchHead()->setInput<Number>("Population", population));
        EXPECT_TRUE(fetchHead()->setInput<Number>("Density", rho));
        EXPECT_TRUE(fetchHead()->setOption("BoxScaling", boxScalingType));
        EXPECT_TRUE(fetchHead()->setOption<Units::DensityUnits>("DensityUnits", rhoUnits));
        EXPECT_TRUE(currentGraph_->addEdge({std::string(speciesPtr->name()), "Species", insertNodeName, "Species"}));
        EXPECT_TRUE(
            currentGraph_->addEdge({std::string(cfgSourceNode->name()), cfgSourceOutput, insertNodeName, "Configuration"}));

        cfgSourceNode = fetchHead();

        // After the first InsertNode addition the source output name reverts to "Configuration" (it may previously have
        // been Output from SetBox)
        cfgSourceOutput = "Configuration";
    }

    return fetchHead();
}
// Find a species corresponding to a species node belonging to the graph of any type (Species/ImportSpecies/AtomicSpecies)Node
const Species *TestGraph::findSpecies(std::string name)
{
    auto node = findNode(name);
    if (!node)
        return nullptr;

    return node->findOutput("Species")->get<const Species *>();
}
// Create atomic species
const Species *TestGraph::createAtomicSpecies(Elements::Element element, std::string interactionPotential)
{
    auto node = createNode("AtomicSpecies", Elements::symbol(element));
    node->setOption("Element", element);
    node->setOption("InteractionPotential", interactionPotential);
    node->run();
    return node->findOutput("Species")->get<const Species *>();
}
// Import species
const Species *TestGraph::importSpecies(std::filesystem::path tomlPath)
{
    auto node = std::make_unique<ImportSpeciesNode>(this);
    node->setOption("FilePath", tomlPath);
    node->run();
    auto species = node->findOutput("Species")->get<const Species *>();
    addNode(std::move(node), species->name());
    return species;
}
// Create a species node with structure and forcefield data sources
SpeciesNode *TestGraph::createSpeciesFromStructureAndForcefield(std::string name, std::string structureNodeType,
                                                                std::string structureFilePath, std::shared_ptr<Forcefield> ff,
                                                                bool calculateBonding)
{
    // Add species node
    auto speciesNodeUniquePtr = std::make_unique<SpeciesNode>(nullptr);
    EXPECT_TRUE(speciesNodeUniquePtr);
    auto speciesNodePtr = speciesNodeUniquePtr.get();
    EXPECT_TRUE(speciesNodePtr);
    auto &species = speciesNodePtr->species();
    species.setName(name);
    currentGraph_->addNode(std::move(speciesNodeUniquePtr), name);

    // Create structure import node
    auto structureNode = createNode(structureNodeType);
    EXPECT_TRUE(structureNode);
    structureNode->setOption("FilePath", std::filesystem::path(structureFilePath));

    // Create rebonding node?
    if (calculateBonding)
    {
        auto calculateBondingNode = createNode("CalculateBonding");
        EXPECT_TRUE(calculateBondingNode);
        EXPECT_TRUE(currentGraph_->addEdge({structureNodeType, "Structure", "CalculateBonding", "Structure"}));
        EXPECT_TRUE(currentGraph_->addEdge({"CalculateBonding", "Structure", name, "Structure"}));
    }
    else
        EXPECT_TRUE(currentGraph_->addEdge({structureNodeType, "Structure", name, "Structure"}));

    // Create forcefield node
    auto forcefieldNode = dynamic_cast<ForcefieldNode *>(createNode("Forcefield"));
    EXPECT_TRUE(forcefieldNode);
    EXPECT_TRUE(forcefieldNode->setOption<Forcefield *>("Forcefield", ff.get()));
    EXPECT_TRUE(currentGraph_->addEdge({"Forcefield", "Recipe", name, "Recipe"}));

    return speciesNodePtr;
}
// Create basic configuration graph, returning the last node
Node *TestGraph::createConfiguration(std::string name, const std::vector<std::pair<const Species *, int>> &species, double rho,
                                     Units::DensityUnits rhoUnits)
{
    // Create configuration
    EXPECT_TRUE(appendNode("Configuration", name));

    // Add Species and Insert nodes
    return createAndInsertSpecies(fetchHead(), "Configuration", species, rho, rhoUnits,
                                  InsertRandomNode::BoxScalingType::AddVolume);
}
// Create basic configuration graph, returning the last node
Node *TestGraph::createConfiguration(std::string name, const std::vector<std::pair<const Species *, int>> &species,
                                     const Vector3 &cellLengths, const Vector3 &cellAngles)
{
    // Create configuration and SetBox nodes
    EXPECT_TRUE(appendNode("Configuration", name));
    EXPECT_TRUE(appendNode("SetBox"));
    fetchHead()->setOption<Vector3>("Lengths", cellLengths);
    fetchHead()->setOption<Vector3>("Angles", cellAngles);
    EXPECT_TRUE(currentGraph_->addEdge({name, "Configuration", "SetBox", "Input"}));

    // Add Species and Insert nodes
    return createAndInsertSpecies(fetchHead(), "Output", species, 0.1, Units::DensityUnits::AtomsPerAngstromUnits,
                                  InsertRandomNode::BoxScalingType::None);
}
// Append a set coordinates node with a structure import input
Node *TestGraph::appendSetCoordinates(std::string_view importNodeType, std::string filePath, std::string sourceOutpuName)
{
    const auto cfgSourceNode = fetchHead();

    EXPECT_TRUE(appendNode("SetCoordinates"));
    auto structureNode = createNode(importNodeType);
    EXPECT_TRUE(structureNode);
    EXPECT_TRUE(structureNode->setOption("FilePath", std::filesystem::path(filePath)));

    EXPECT_TRUE(currentGraph_->addEdge({std::string(structureNode->name()), "Structure", "SetCoordinates", "Structure"}));

    EXPECT_TRUE(
        currentGraph_->addEdge({std::string(cfgSourceNode->name()), sourceOutpuName, "SetCoordinates", "Configuration"}));

    return head<SetCoordinatesNode>();
}
// Create a trajectory iterator subgraph
IterableGraph *TestGraph::appendTrajectoryIterator(std::string trajectoryImportNodeType, std::string filePath)
{
    // Get current head node before we create the IterableGraph
    auto oldHead = head_;
    auto oldGraph = currentGraph_;

    // Add iterator node and make it the current graph
    currentGraph_ = dynamic_cast<IterableGraph *>(appendNode("Iterator", "Iterator"));
    EXPECT_TRUE(currentGraph_);
    head_ = nullptr;

    // Create a dynamic input from the (assumed) "Configuration" output on the old head of the previous graph
    EXPECT_TRUE(oldGraph->addEdge({std::string(oldHead->name()), "Configuration", "Iterator", "Configuration"}));

    // Within the iterator graph create SetCoordinates and trajectory import nodes
    auto trajectoryNode = appendNode(trajectoryImportNodeType);
    EXPECT_TRUE(trajectoryNode);
    EXPECT_TRUE(trajectoryNode->setOption("FilePath", std::filesystem::path(filePath)));
    EXPECT_TRUE(appendNode("SetCoordinates"));

    EXPECT_TRUE(currentGraph_->addEdge({std::string(trajectoryNode->name()), "Structure", "SetCoordinates", "Structure"}));

    EXPECT_TRUE(currentGraph_->addEdge({"Inputs", "Configuration", "SetCoordinates", "Configuration"}));

    return dynamic_cast<IterableGraph *>(currentGraph_);
}
// Append GR and SQ nodes
std::pair<GRNode *, SQNode *> TestGraph::appendGRSQ(bool noAveraging, bool noIntraBroadening)
{
    auto oldHead = head_;

    // Create and setup the GR node
    auto grNode = dynamic_cast<GRNode *>(appendNode("GR"));
    EXPECT_TRUE(grNode);
    if (noAveraging)
        EXPECT_TRUE(grNode->setOption("Averaging", std::optional<Number>()));
    if (noIntraBroadening)
        EXPECT_TRUE(grNode->setOption("IntraBroadening", Function1DWrapper()));

    EXPECT_TRUE(currentGraph_->addEdge({std::string(oldHead->name()), "Configuration", "GR", "Configuration"}));

    // Create the SQ node
    auto sqNode = dynamic_cast<SQNode *>(appendNode("SQ"));
    EXPECT_TRUE(sqNode);
    EXPECT_TRUE(currentGraph_->addEdge({"GR", "UnweightedGR", "SQ", "UnweightedGR"}));

    return {grNode, sqNode};
}
// Create a NeutronSQ node with optional reference data
NeutronSQNode *TestGraph::appendNeutronSQ(SQNode *sqNode, std::string name,
                                          const std::vector<std::tuple<const Species *, std::string, double>> isotopologues,
                                          std::string referenceData, bool isHistogram)
{
    // Construct the isotopologue set
    IsotopologueSet isotopologueSet;
    for (auto &&[species, isotopologueName, relativeWeight] : isotopologues)
    {
        if (!species)
        {
            std::cout << std::format("Null species found - can't construct IsotopologueSet\n");
            return nullptr;
        }
        auto isotopologue = species->findIsotopologue(isotopologueName);
        if (!isotopologue)
        {
            std::cout << std::format("No isotopologue named '{}' exists in species '{}' - can't construct IsotopologueSet\n",
                                     isotopologueName, species->name());
            return nullptr;
        }
        isotopologueSet.add(isotopologue, relativeWeight);
    }

    EXPECT_TRUE(appendNode("NeutronSQ", name));
    EXPECT_TRUE(fetchHead()->setOption("Isotopologues", isotopologueSet));
    EXPECT_TRUE(currentGraph_->addEdge({std::string(sqNode->name()), "UnweightedGR", name, "UnweightedGR"}));
    EXPECT_TRUE(currentGraph_->addEdge({std::string(sqNode->name()), "UnweightedSQ", name, "UnweightedSQ"}));

    // Set reference F(Q) data
    if (!referenceData.empty())
    {
        auto data1DImportNode = createNode("ImportXYData", std::format("Reference-{}", name));
        EXPECT_TRUE(data1DImportNode);
        EXPECT_TRUE(data1DImportNode->setOption("FilePath", std::filesystem::path(referenceData)));
        EXPECT_TRUE(data1DImportNode->setOption<bool>("Histogram", isHistogram));
        EXPECT_TRUE(currentGraph_->addEdge({std::format("Reference-{}", name), "Data", name, "ReferenceData"}));
    }

    return head<NeutronSQNode>();
}
// Create an XRaySQ node with optional reference data
XRaySQNode *TestGraph::appendXRaySQ(SQNode *sqNode, std::string name, std::string referenceData, bool isHistogram)
{
    EXPECT_TRUE(appendNode("XRaySQ", name));
    EXPECT_TRUE(currentGraph_->addEdge({std::string(sqNode->name()), "UnweightedGR", name, "UnweightedGR"}));
    EXPECT_TRUE(currentGraph_->addEdge({std::string(sqNode->name()), "UnweightedSQ", name, "UnweightedSQ"}));

    // Set reference F(Q) data
    if (!referenceData.empty())
    {
        auto data1DImportNode = createNode("ImportXYData", std::format("Reference-{}", name));
        EXPECT_TRUE(data1DImportNode);
        EXPECT_TRUE(data1DImportNode->setOption("FilePath", std::filesystem::path(referenceData)));
        EXPECT_TRUE(data1DImportNode->setOption<bool>("Histogram", isHistogram));
        EXPECT_TRUE(currentGraph_->addEdge({std::format("Reference-{}", name), "Data", name, "ReferenceData"}));
    }
    return head<XRaySQNode>();
}
}; // namespace UnitTest
