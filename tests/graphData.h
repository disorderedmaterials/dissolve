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
#include "nodes/insert.h"
#include "nodes/neutronSQ/neutronSQ.h"
#include "nodes/sq/sq.h"
#include "nodes/xRaySQ/xRaySQ.h"
#include "tests/speciesData.h"
#include <gtest/gtest.h>

namespace UnitTest
{
// Basic object setup for any Graph-based test
class TestGraph : public DissolveGraph
{
    public:
    TestGraph() : DissolveGraph(dissolve), dissolve(coreData) { Node::echo_ = true; }
    ~TestGraph() { exportMermaidGraph(); }
    CoreData coreData;
    Dissolve dissolve;

    /*
     * Graph Creation Helpers
     */
    private:
    // Create species insertion node chain
    Node *insertSpecies(Node *lastNode,
                        const std::vector<std::pair<std::function<std::unique_ptr<SpeciesNode>()>, int>> &species, double rho,
                        Units::DensityUnits rhoUnits = Units::DensityUnits::AtomsPerAngstromUnits,
                        InsertNode::BoxActionStyle boxActionStyle = InsertNode::BoxActionStyle::AddVolume)
    {
        for (auto &[speciesCreator, population] : species)
        {
            // Create the species node and get the species pointer
            auto speciesUnique = speciesCreator();
            auto &speciesNode = speciesUnique->species();

            // Move the species node into the graph
            addNode(std::move(speciesUnique), speciesNode.name());

            auto insertNodeName = std::format("Insert-{}", speciesNode.name());
            auto insertNode = createNode("Insert", insertNodeName);

            EXPECT_TRUE(insertNode);
            EXPECT_TRUE(insertNode->setInput<Number>("Population", population));
            EXPECT_TRUE(insertNode->setInput<Number>("Density", rho));
            EXPECT_TRUE(insertNode->setOption("BoxAction", boxActionStyle));
            EXPECT_TRUE(insertNode->setOption<Units::DensityUnits>("DensityUnits", rhoUnits));
            EXPECT_TRUE(addEdge({std::string(speciesNode.name()), "Species", insertNodeName, "Species"}));
            EXPECT_TRUE(addEdge({std::string(lastNode->name()), "Configuration", insertNodeName, "Configuration"}));

            lastNode = insertNode;
        }

        return lastNode;
    }

    public:
    // Create basic configuration graph, returning the last node
    Node *createConfiguration(std::string name,
                              const std::vector<std::pair<std::function<std::unique_ptr<SpeciesNode>()>, int>> &species,
                              double rho, Units::DensityUnits rhoUnits = Units::DensityUnits::AtomsPerAngstromUnits)
    {
        // Create configuration and SetCell nodes
        auto lastNode = createNode("Configuration", name);
        EXPECT_TRUE(lastNode);
        lastNode = createNode("SetCell");
        EXPECT_TRUE(lastNode);
        EXPECT_TRUE(addEdge({name, "Configuration", "SetCell", "Configuration"}));

        // Add Species and Insert nodes
        return insertSpecies(lastNode, species, rho, rhoUnits, InsertNode::BoxActionStyle::AddVolume);
    }

    // Create basic configuration graph, returning the last node
    Node *createConfiguration(std::string name,
                              const std::vector<std::pair<std::function<std::unique_ptr<SpeciesNode>()>, int>> &species,
                              const Vector3 &cellLengths, const Vector3 &cellAngles = {90.0, 90.0, 90.0})
    {
        // Create configuration and SetCell nodes
        auto lastNode = createNode("Configuration", name);
        EXPECT_TRUE(lastNode);
        lastNode = createNode("SetCell");
        lastNode->setOption<Vector3>("Lengths", cellLengths);
        lastNode->setOption<Vector3>("Angles", cellAngles);
        EXPECT_TRUE(lastNode);
        EXPECT_TRUE(addEdge({name, "Configuration", "SetCell", "Configuration"}));

        // Add Species and Insert nodes
        return insertSpecies(lastNode, species, 0.1, Units::DensityUnits::AtomsPerAngstromUnits,
                             InsertNode::BoxActionStyle::None);
    }
    // Append an import coordinates node
    Node *appendImportCoordinates(Node *lastNode, CoordinateImportFileFormat fileFormat, bool supercell = false)
    {
        auto importCoordinates = createNode("ImportConfigurationCoordinates");
        EXPECT_TRUE(importCoordinates->setOption<std::string>("FilePath", std::string(fileFormat.filename())));
        EXPECT_TRUE(importCoordinates->setOption<CoordinateImportFileFormat::CoordinateImportFormat>(
            "FileFormat",
            CoordinateImportFileFormat::coordinateImportFileFormat().enumerationByIndex(fileFormat.formatIndex())));
        EXPECT_TRUE(addEdge({std::string(lastNode->name()), supercell ? "SupercellConfiguration" : "Configuration",
                             "ImportConfigurationCoordinates", "Configuration"}));

        return importCoordinates;
    }

    // Append GR and SQ nodes
    std::pair<GRNode *, SQNode *> appendGRSQ(Node *lastNode, bool noAveraging = false, bool noIntraBroadening = false)
    {
        // Create and setup the GR node
        auto grNode = dynamic_cast<GRNode *>(createNode("GR"));
        EXPECT_TRUE(grNode);
        if (noAveraging)
            EXPECT_TRUE(grNode->setOption("Averaging", std::optional<Number>()));
        if (noIntraBroadening)
            EXPECT_TRUE(grNode->setOption("IntraBroadening", Function1DWrapper()));

        EXPECT_TRUE(addEdge({std::string(lastNode->name()), "Configuration", "GR", "Configuration"}));

        // Create the SQ node
        auto sqNode = dynamic_cast<SQNode *>(createNode("SQ"));
        EXPECT_TRUE(sqNode);
        EXPECT_TRUE(addEdge({"GR", "UnweightedGR", "SQ", "UnweightedGR"}));

        return {grNode, sqNode};
    }
    // Create a NeutronSQ node with optional reference data
    NeutronSQNode *appendNeutronSQ(SQNode *sqNode, std::string name,
                                   const std::vector<std::tuple<std::string, std::string, double>> isotopologues = {},
                                   const Exchangeables &exchangeables = {},
                                   Data1DImportFileFormat referenceData = Data1DImportFileFormat())
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

        auto neutronSQNode = dynamic_cast<NeutronSQNode *>(createNode("NeutronSQ", name));
        EXPECT_TRUE(neutronSQNode);
        EXPECT_TRUE(neutronSQNode->setOption("Isotopologues", isotopologueSet));
        EXPECT_TRUE(neutronSQNode->setOption("Exchangeables", exchangeables));
        EXPECT_TRUE(addEdge({std::string(sqNode->name()), "UnweightedGR", name, "UnweightedGR"}));
        EXPECT_TRUE(addEdge({std::string(sqNode->name()), "UnweightedSQ", name, "UnweightedSQ"}));

        // Set reference F(Q) data
        if (referenceData.hasFilename())
        {
            auto data1DImportNode = createNode("Data1DImport", std::format("Reference-{}", name));
            EXPECT_TRUE(data1DImportNode);
            EXPECT_TRUE(data1DImportNode->setOption<std::string>("FilePath", std::string(referenceData.filename())));
            EXPECT_TRUE(data1DImportNode->setOption<Data1DImportFileFormat::Data1DImportFormat>(
                "ImportFormat", Data1DImportFileFormat::data1DImportFormat().enumerationByIndex(referenceData.formatIndex())));
            EXPECT_TRUE(addEdge({std::format("Reference-{}", name), "Data", name, "ReferenceData"}));
        }

        return neutronSQNode;
    }
    // Create an XRaySQ node with optional reference data
    XRaySQNode *appendXRaySQ(SQNode *sqNode, std::string name, Data1DImportFileFormat referenceData = Data1DImportFileFormat())
    {
        auto xRaySQNode = dynamic_cast<XRaySQNode *>(createNode("XRaySQ", name));
        EXPECT_TRUE(xRaySQNode);
        EXPECT_TRUE(addEdge({std::string(sqNode->name()), "UnweightedGR", name, "UnweightedGR"}));
        EXPECT_TRUE(addEdge({std::string(sqNode->name()), "UnweightedSQ", name, "UnweightedSQ"}));

        // Set reference F(Q) data
        if (referenceData.hasFilename())
        {
            auto data1DImportNode = createNode("Data1DImport", std::format("Reference-{}", name));
            EXPECT_TRUE(data1DImportNode);
            EXPECT_TRUE(data1DImportNode->setOption<std::string>("FilePath", std::string(referenceData.filename())));
            EXPECT_TRUE(data1DImportNode->setOption<Data1DImportFileFormat::Data1DImportFormat>(
                "ImportFormat", Data1DImportFileFormat::data1DImportFormat().enumerationByIndex(referenceData.formatIndex())));
            EXPECT_TRUE(addEdge({std::format("Reference-{}", name), "Data", name, "ReferenceData"}));
        }
        return xRaySQNode;
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
