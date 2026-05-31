// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "classes/isotopologueSet.h"
#include "data/structureFactors.h"
#include "io/import/coordinates.h"
#include "io/import/data1D.h"
#include "main/dissolve.h"
#include "nodes/bragg.h"
#include "nodes/configuration.h"
#include "nodes/dissolve.h"
#include "nodes/forcefield.h"
#include "nodes/insert.h"
#include "nodes/iterableGraph.h"
#include "nodes/neutronSQ/neutronSQ.h"
#include "nodes/setCoordinates.h"
#include "nodes/species.h"
#include "nodes/sq/sq.h"
#include "nodes/xRaySQ/xRaySQ.h"
#include <gtest/gtest.h>

namespace UnitTest
{
// Basic object setup for any Graph-based test
class TestGraph : public DissolveGraph
{
    public:
    TestGraph() : currentGraph_(this)
    {
        Node::echo_ = true;
        PairPotential::setChargeSource(PairPotential::ChargeSource::Automatic);
    }
    ~TestGraph() { exportMermaidGraph(); }

    public:
    // Container for data 1D import filename and whether or not it is a histogram
    struct Data1DImportFileFormat
    {
        std::string filename;
        bool histogram;

        Data1DImportFileFormat(std::string filename = "", bool histogram = false) : filename(filename), histogram(histogram) {}
    };

    private:
    // Current graph target
    Graph *currentGraph_{nullptr};
    // Most recently appended node in the current graph
    Node *head_{nullptr};

    /*
     * Graph Creation Helpers
     */
    public:
    // Returns pointer to current top node in graph
    Node *fetchHead() const { return head_; }
    // Returns the name of the current head node in the graph
    std::string fetchHeadName() const { return head_ ? std::string(head_->name()) : "NO_NODE"; }
    // Returns reference to current top node in graph, cast to the known node type
    template <class NodeType> NodeType *head() const { return static_cast<NodeType *>(head_); }
    // Append new node to the graph
    Node *appendNode(const std::string &nodeType, const std::optional<std::string> &name = {})
    {
        auto node = name.has_value() ? currentGraph_->createNode(nodeType, *name) : currentGraph_->createNode(nodeType);

        if (!node)
            return nullptr;

        head_ = node;

        return node;
    }
    // Create species insertion node chain
    Node *createAndInsertSpecies(Node *cfgSourceNode, std::string cfgSourceOutput,
                                 const std::vector<std::pair<std::string, int>> &species, double rho,
                                 Units::DensityUnits rhoUnits = Units::DensityUnits::AtomsPerAngstromUnits,
                                 InsertNode::BoxActionStyle boxActionStyle = InsertNode::BoxActionStyle::AddVolume)
    {
        // Add Species and Insert nodes
        for (auto &[speciesString, population] : species)
        {
            // Create the species node and get the species pointer
            std::unique_ptr<SpeciesNode> speciesUnique;
            if (speciesString.ends_with(".toml"))
                speciesUnique = loadTOMLSpecies(speciesString);
            else
            {
                if (speciesString.find('|') == std::string::npos)
                    speciesUnique =
                        createAtomicSpecies(Elements::element(speciesString), {ShortRangeFunctions::Form::Undefined});
                else
                    speciesUnique = createAtomicSpecies(
                        Elements::element(DissolveSys::beforeChar(speciesString, '|')),
                        {ShortRangeFunctions::Form::LennardJones, DissolveSys::afterChar(speciesString, '|')});
            }
            EXPECT_TRUE(speciesUnique);
            auto &speciesNode = speciesUnique->species();

            // Move the species node into the graph
            currentGraph_->addNode(std::move(speciesUnique), speciesNode.name());

            auto insertNodeName = std::format("Insert-{}", speciesNode.name());
            EXPECT_TRUE(appendNode("Insert", insertNodeName));
            EXPECT_TRUE(fetchHead()->setInput<Number>("Population", population));
            EXPECT_TRUE(fetchHead()->setInput<Number>("Density", rho));
            EXPECT_TRUE(fetchHead()->setOption("BoxAction", boxActionStyle));
            EXPECT_TRUE(fetchHead()->setOption<Units::DensityUnits>("DensityUnits", rhoUnits));
            EXPECT_TRUE(currentGraph_->addEdge({std::string(speciesNode.name()), "Species", insertNodeName, "Species"}));
            EXPECT_TRUE(
                currentGraph_->addEdge({std::string(cfgSourceNode->name()), cfgSourceOutput, insertNodeName, "Configuration"}));

            cfgSourceNode = fetchHead();

            // After the first InsertNode addition the source output name reverts to "Configuration" (it may previously have
            // been Output from SetCell)
            cfgSourceOutput = "Configuration";
        }

        return fetchHead();
    }

    public:
    // Create and return atomic SpeciesNode
    static std::unique_ptr<SpeciesNode> createAtomicSpecies(Elements::Element element,
                                                            InteractionPotential<ShortRangeFunctions> potential = {
                                                                ShortRangeFunctions::Form::Undefined, ""})
    {
        // Add species node
        auto speciesNodeUniquePtr = std::make_unique<SpeciesNode>(nullptr);
        auto speciesNodePtr = speciesNodeUniquePtr.get();
        auto species = &speciesNodePtr->species();
        species->setName(Elements::symbol(element));
        species->createAtomic(element, potential);

        return speciesNodeUniquePtr;
    }
    // Create species from TOML file
    static std::unique_ptr<SpeciesNode> loadTOMLSpecies(std::string_view path)
    {
        // Add species node
        auto speciesNodeUniquePtr = std::make_unique<SpeciesNode>(nullptr);
        auto speciesNodePtr = speciesNodeUniquePtr.get();
        auto &species = speciesNodePtr->species();
        species.load(path);

        return speciesNodeUniquePtr;
    }
    // Create a species node with structure and forcefield data sources
    SpeciesNode *createSpeciesFromStructureAndForcefield(std::string name, std::string structureNodeType,
                                                         std::string structureFilePath, std::shared_ptr<Forcefield> ff,
                                                         bool calculateBonding = true)
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
        structureNode->setOption<std::string>("FilePath", structureFilePath);

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
    Node *createConfiguration(std::string name, const std::vector<std::pair<std::string, int>> &species, double rho,
                              Units::DensityUnits rhoUnits = Units::DensityUnits::AtomsPerAngstromUnits)
    {
        // Create configuration
        EXPECT_TRUE(appendNode("Configuration", name));

        // Add Species and Insert nodes
        return createAndInsertSpecies(fetchHead(), "Configuration", species, rho, rhoUnits,
                                      InsertNode::BoxActionStyle::AddVolume);
    }
    // Create basic configuration graph, returning the last node
    Node *createConfiguration(std::string name, const std::vector<std::pair<std::string, int>> &species,
                              const Vector3 &cellLengths, const Vector3 &cellAngles = {90.0, 90.0, 90.0})
    {
        // Create configuration and SetCell nodes
        EXPECT_TRUE(appendNode("Configuration", name));
        EXPECT_TRUE(appendNode("SetCell"));
        fetchHead()->setOption<Vector3>("Lengths", cellLengths);
        fetchHead()->setOption<Vector3>("Angles", cellAngles);
        EXPECT_TRUE(currentGraph_->addEdge({name, "Configuration", "SetCell", "Input"}));

        // Add Species and Insert nodes
        return createAndInsertSpecies(fetchHead(), "Output", species, 0.1, Units::DensityUnits::AtomsPerAngstromUnits,
                                      InsertNode::BoxActionStyle::None);
    }
    // Append a set coordinates node with a structure import input
    Node *appendSetCoordinates(std::string_view importNodeType, std::string filePath,
                               std::string sourceOutpuName = "Configuration")
    {
        const auto cfgSourceNode = fetchHead();

        EXPECT_TRUE(appendNode("SetCoordinates"));
        auto structureNode = createNode(importNodeType);
        EXPECT_TRUE(structureNode);
        EXPECT_TRUE(structureNode->setOption<std::string>("FilePath", filePath));

        EXPECT_TRUE(currentGraph_->addEdge({std::string(structureNode->name()), "Structure", "SetCoordinates", "Structure"}));

        EXPECT_TRUE(
            currentGraph_->addEdge({std::string(cfgSourceNode->name()), sourceOutpuName, "SetCoordinates", "Configuration"}));

        return head<SetCoordinatesNode>();
    }
    // Create a trajectory iterator subgraph
    IterableGraph *appendTrajectoryIterator(std::string trajectoryImportNodeType, std::string filePath)
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
        EXPECT_TRUE(trajectoryNode->setOption<std::string>("FilePath", filePath));
        EXPECT_TRUE(appendNode("SetCoordinates"));

        EXPECT_TRUE(currentGraph_->addEdge({std::string(trajectoryNode->name()), "Structure", "SetCoordinates", "Structure"}));

        EXPECT_TRUE(currentGraph_->addEdge({"Inputs", "Configuration", "SetCoordinates", "Configuration"}));

        return dynamic_cast<IterableGraph *>(currentGraph_);
    }
    // Append GR and SQ nodes
    std::pair<GRNode *, SQNode *> appendGRSQ(bool noAveraging = false, bool noIntraBroadening = false)
    {
        // Create and setup the GR node
        auto grNode = dynamic_cast<GRNode *>(createNode("GR"));
        EXPECT_TRUE(grNode);
        if (noAveraging)
            EXPECT_TRUE(grNode->setOption("Averaging", std::optional<Number>()));
        if (noIntraBroadening)
            EXPECT_TRUE(grNode->setOption("IntraBroadening", Function1DWrapper()));

        EXPECT_TRUE(currentGraph_->addEdge({std::string(fetchHead()->name()), "Configuration", "GR", "Configuration"}));

        // Create the SQ node
        auto sqNode = dynamic_cast<SQNode *>(createNode("SQ"));
        EXPECT_TRUE(sqNode);
        EXPECT_TRUE(currentGraph_->addEdge({"GR", "UnweightedGR", "SQ", "UnweightedGR"}));

        return {grNode, sqNode};
    }
    // Create a NeutronSQ node with optional reference data
    NeutronSQNode *appendNeutronSQ(SQNode *sqNode, std::string name,
                                   const std::vector<std::tuple<std::string, std::string, double>> isotopologues = {},
                                   TestGraph::Data1DImportFileFormat referenceData = TestGraph::Data1DImportFileFormat{})
    {
        // Construct the isotopologue set
        IsotopologueSet isotopologueSet;
        for (auto &&[speciesName, isotopologueName, relativeWeight] : isotopologues)
        {
            // Find the named species node
            auto speciesNode = dynamic_cast<SpeciesNode *>(findNode(speciesName));
            if (!speciesNode)
            {
                std::cout << std::format("No species named '{}' exists in the graph - can't construct IsotopologueSet\n",
                                         speciesName);
                return nullptr;
            }
            auto &species = speciesNode->species();
            auto isotopologue = species.findIsotopologue(isotopologueName);
            if (!isotopologue)
            {
                std::cout << std::format(
                    "No isotopologue named '{}' exists in species '{}' - can't construct IsotopologueSet\n", isotopologueName,
                    speciesName);
                return nullptr;
            }
            isotopologueSet.add(isotopologue, relativeWeight);
        }

        EXPECT_TRUE(appendNode("NeutronSQ", name));
        EXPECT_TRUE(fetchHead()->setOption("Isotopologues", isotopologueSet));
        EXPECT_TRUE(currentGraph_->addEdge({std::string(sqNode->name()), "UnweightedGR", name, "UnweightedGR"}));
        EXPECT_TRUE(currentGraph_->addEdge({std::string(sqNode->name()), "UnweightedSQ", name, "UnweightedSQ"}));

        // Set reference F(Q) data
        if (!referenceData.filename.empty())
        {
            auto data1DImportNode = createNode("Data1DImport", std::format("Reference-{}", name));
            EXPECT_TRUE(data1DImportNode);
            EXPECT_TRUE(data1DImportNode->setOption<std::string>("FilePath", std::string(referenceData.filename)));
            EXPECT_TRUE(data1DImportNode->setOption<bool>("Histogram", referenceData.histogram));
            EXPECT_TRUE(currentGraph_->addEdge({std::format("Reference-{}", name), "Data", name, "ReferenceData"}));
        }

        return head<NeutronSQNode>();
    }
    // Create an XRaySQ node with optional reference data
    XRaySQNode *appendXRaySQ(SQNode *sqNode, std::string name,
                             TestGraph::Data1DImportFileFormat referenceData = TestGraph::Data1DImportFileFormat{})
    {
        EXPECT_TRUE(appendNode("XRaySQ", name));
        EXPECT_TRUE(currentGraph_->addEdge({std::string(sqNode->name()), "UnweightedGR", name, "UnweightedGR"}));
        EXPECT_TRUE(currentGraph_->addEdge({std::string(sqNode->name()), "UnweightedSQ", name, "UnweightedSQ"}));

        // Set reference F(Q) data
        if (!referenceData.filename.empty())
        {
            auto data1DImportNode = createNode("Data1DImport", std::format("Reference-{}", name));
            EXPECT_TRUE(data1DImportNode);
            EXPECT_TRUE(data1DImportNode->setOption<std::string>("FilePath", std::string(referenceData.filename)));
            EXPECT_TRUE(data1DImportNode->setOption<bool>("Histogram", referenceData.histogram));
            EXPECT_TRUE(currentGraph_->addEdge({std::format("Reference-{}", name), "Data", name, "ReferenceData"}));
        }
        return head<XRaySQNode>();
    }

    /*
     * Utility Functions
     */
    public:
    // Save the specified graph in Mermaid format to a file named after the unit test
    static void exportMermaidGraph(Graph &graph)
    {
        auto suite = ::testing::UnitTest::GetInstance()->current_test_info()->test_suite_name();
        auto name = ::testing::UnitTest::GetInstance()->current_test_info()->name();
        std::string filename = std::format("{}-{}.mermaid", suite, name);

        std::ofstream myfile;
        myfile.open(filename);
        myfile << graph;
        myfile.close();
    }
    // Save the current graph in Mermaid format to a file named after the unit test
    void exportMermaidGraph() { exportMermaidGraph(*this); }
};
} // namespace UnitTest
