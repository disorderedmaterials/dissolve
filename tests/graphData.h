// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "classes/isotopologueSet.h"
#include "data/structureFactors.h"
#include "io/import/coordinates.h"
#include "io/import/data1D.h"
#include "main/dissolve.h"
#include "math/windowFunction.h"
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
class GraphTestData
{
    public:
    GraphTestData() : dissolve(coreData), graphRoot(dissolve) { Node::echo_ = true; }
    CoreData coreData;
    Dissolve dissolve;
    DissolveGraph graphRoot;
};

// Create basic configuration graph, returning the last node
inline Node *createConfiguration(Graph *root,
                                 const std::vector<std::pair<std::function<std::unique_ptr<SpeciesNode>()>, int>> &species,
                                 double rho, Units::DensityUnits rhoUnits = Units::DensityUnits::AtomsPerAngstromUnits)
{
    // Create configuration node
    auto lastNode = root->createNode("Configuration", "Mixture");
    EXPECT_TRUE(lastNode);

    // Add Species and Insert nodes
    for (auto &[speciesCreator, population] : species)
    {
        // Create the species node and get the species pointer
        auto speciesUnique = speciesCreator();
        auto &speciesNode = speciesUnique->species();

        // Move the species node into the graph
        root->addNode(std::move(speciesUnique), speciesNode.name());

        auto insertNodeName = std::format("Insert-{}", speciesNode.name());
        auto insertNode = root->createNode("Insert", insertNodeName);

        EXPECT_TRUE(insertNode);
        EXPECT_TRUE(insertNode->setInput<Number>("Population", population));
        EXPECT_TRUE(insertNode->setInput<Number>("Density", rho));
        EXPECT_TRUE(insertNode->setOption<Units::DensityUnits>("DensityUnits", rhoUnits));
        EXPECT_TRUE(root->addEdge({std::string(speciesNode.name()), "Species", insertNodeName, "Species"}));
        EXPECT_TRUE(root->addEdge({std::string(lastNode->name()), "Configuration", insertNodeName, "Configuration"}));

        lastNode = insertNode;
    }

    return lastNode;
}

// Append an import coordinates node
inline Node *appendImportCoordinates(Graph *root, Node *lastNode, CoordinateImportFileFormat fileFormat)
{
    auto importCoordinates = root->createNode("ImportConfigurationCoordinates");
    EXPECT_TRUE(importCoordinates->setOption<std::string>("FilePath", std::string(fileFormat.filename())));
    EXPECT_TRUE(importCoordinates->setOption<CoordinateImportFileFormat::CoordinateImportFormat>(
        "FileFormat", CoordinateImportFileFormat::coordinateImportFileFormat().enumerationByIndex(fileFormat.formatIndex())));
    EXPECT_TRUE(
        root->addEdge({std::string(lastNode->name()), "Configuration", "ImportConfigurationCoordinates", "Configuration"}));

    return importCoordinates;
}

// Append GR and SQ nodes
inline std::pair<GRNode *, SQNode *> appendGRSQ(Graph *root, Node *lastNode, bool noAveraging = false,
                                                bool noIntraBroadening = false)
{
    // Create and setup the GR node
    auto grNode = dynamic_cast<GRNode *>(root->createNode("GR"));
    EXPECT_TRUE(grNode);
    if (noAveraging)
        EXPECT_TRUE(grNode->setOption("Averaging", std::optional<Number>()));
    if (noIntraBroadening)
        EXPECT_TRUE(grNode->setOption("IntraBroadening", Function1DWrapper()));

    EXPECT_TRUE(root->addEdge({std::string(lastNode->name()), "Configuration", "GR", "Configuration"}));

    // Create the SQ node
    auto sqNode = dynamic_cast<SQNode *>(root->createNode("SQ"));
    EXPECT_TRUE(sqNode);
    EXPECT_TRUE(root->addEdge({"GR", "UnweightedGR", "SQ", "UnweightedGR"}));

    return {grNode, sqNode};
}

// Create a NeutronSQ node with optional reference data
inline NeutronSQNode *appendNeutronSQ(Graph *root, SQNode *sqNode, std::string name,
                                      const std::vector<std::tuple<std::string, std::string, double>> isotopologues = {},
                                      const Exchangeables &exchangeables = {},
                                      Data1DImportFileFormat referenceData = Data1DImportFileFormat())
{
    // Construct the isotopologue set
    IsotopologueSet isotopologueSet;
    for (auto &&[speciesName, isotopologueName, relativeWeight] : isotopologues)
    {
        // Find the named species node
        auto speciesNode = dynamic_cast<SpeciesNode *>(root->findNode(speciesName));
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
            std::cout << std::format("No isotopologue named '{}' exists in species '{}' - can't construct IsotopologueSet\n",
                                     isotopologueName, speciesName);
            return nullptr;
        }
        isotopologueSet.add(isotopologue, relativeWeight);
    }

    auto neutronSQNode = dynamic_cast<NeutronSQNode *>(root->createNode("NeutronSQ", name));
    EXPECT_TRUE(neutronSQNode);
    EXPECT_TRUE(neutronSQNode->setOption("Isotopologues", isotopologueSet));
    EXPECT_TRUE(neutronSQNode->setOption("Exchangeables", exchangeables));
    EXPECT_TRUE(root->addEdge({std::string(sqNode->name()), "UnweightedGR", name, "UnweightedGR"}));
    EXPECT_TRUE(root->addEdge({std::string(sqNode->name()), "UnweightedSQ", name, "UnweightedSQ"}));

    // Set reference F(Q) data
    if (referenceData.hasFilename())
    {
        auto data1DImportNode = root->createNode("Data1DImport", std::format("Reference-{}", name));
        EXPECT_TRUE(data1DImportNode);
        EXPECT_TRUE(data1DImportNode->setOption<std::string>("FilePath", std::string(referenceData.filename())));
        EXPECT_TRUE(data1DImportNode->setOption<Data1DImportFileFormat::Data1DImportFormat>(
            "ImportFormat", Data1DImportFileFormat::data1DImportFormat().enumerationByIndex(referenceData.formatIndex())));
        EXPECT_TRUE(root->addEdge({std::format("Reference-{}", name), "Data", name, "ReferenceData"}));
    }

    return neutronSQNode;
}

// Create an XRaySQ node with optional reference data
inline XRaySQNode *appendXRaySQ(Graph *root, SQNode *sqNode, std::string name,
                                Data1DImportFileFormat referenceData = Data1DImportFileFormat())
{
    auto xRaySQNode = dynamic_cast<XRaySQNode *>(root->createNode("XRaySQ", name));
    EXPECT_TRUE(xRaySQNode);
    EXPECT_TRUE(root->addEdge({std::string(sqNode->name()), "UnweightedGR", name, "UnweightedGR"}));
    EXPECT_TRUE(root->addEdge({std::string(sqNode->name()), "UnweightedSQ", name, "UnweightedSQ"}));

    // Set reference F(Q) data
    if (referenceData.hasFilename())
    {
        auto data1DImportNode = root->createNode("Data1DImport", std::format("Reference-{}", name));
        EXPECT_TRUE(data1DImportNode);
        EXPECT_TRUE(data1DImportNode->setOption<std::string>("FilePath", std::string(referenceData.filename())));
        EXPECT_TRUE(data1DImportNode->setOption<Data1DImportFileFormat::Data1DImportFormat>(
            "ImportFormat", Data1DImportFileFormat::data1DImportFormat().enumerationByIndex(referenceData.formatIndex())));
        EXPECT_TRUE(root->addEdge({std::format("Reference-{}", name), "Data", name, "ReferenceData"}));
    }

    return xRaySQNode;
}

// Create a MgO graph in the supplied root node
inline void createMgOGraph(Graph *root, int populationMG, int populationO,
                           CoordinateImportFileFormat initialCoordinates = CoordinateImportFileFormat())
{
    // Create species and configuration
    auto [mGNode, oNode] = createMgOSpecies(root);
    ASSERT_TRUE(mGNode);
    auto configurationNode = root->createNode("Configuration", "Crystal");
    ASSERT_TRUE(configurationNode);

    // Set cell
    auto setCellNode = root->createNode("SetCell", "Box");
    ASSERT_TRUE(setCellNode);
    ASSERT_TRUE(root->addEdge({"Crystal", "Configuration", "Box", "Configuration"}));

    // Insert Mg species
    auto insertMgNode = root->createNode("Insert", "InsertMg");
    ASSERT_TRUE(insertMgNode);
    ASSERT_TRUE(root->addEdge({"Mg", "Species", "InsertMg", "Species"}));
    ASSERT_TRUE(root->addEdge({"Box", "Configuration", "InsertMg", "Configuration"}));
    ASSERT_TRUE(insertMgNode->setInput<Number>("Population", populationMG));
    ASSERT_TRUE(insertMgNode->setInput<Number>("Density", 0.1));
    ASSERT_TRUE(insertMgNode->setOption<Units::DensityUnits>("DensityUnits", Units::DensityUnits::AtomsPerAngstromUnits));
    ASSERT_TRUE(insertMgNode->setOption<InsertNode::BoxActionStyle>("BoxAction", InsertNode::BoxActionStyle::None));

    // Insert O species
    auto insertONode = root->createNode("Insert", "InsertO");
    ASSERT_TRUE(insertONode);
    ASSERT_TRUE(root->addEdge({"O", "Species", "InsertO", "Species"}));
    ASSERT_TRUE(root->addEdge({"InsertMg", "Configuration", "InsertO", "Configuration"}));
    ASSERT_TRUE(insertONode->setInput<Number>("Population", populationO));
    ASSERT_TRUE(insertONode->setInput<Number>("Density", 0.1));
    ASSERT_TRUE(insertONode->setOption<Units::DensityUnits>("DensityUnits", Units::DensityUnits::AtomsPerAngstromUnits));
    ASSERT_TRUE(insertONode->setOption<InsertNode::BoxActionStyle>("BoxAction", InsertNode::BoxActionStyle::None));

    // Import reference coordinates
    if (initialCoordinates.hasFilename())
    {
        auto importCoordinates = root->createNode("ImportConfigurationCoordinates", "Import");
        ASSERT_TRUE(importCoordinates->setOption<std::string>("FilePath", std::string(initialCoordinates.filename())));
        ASSERT_TRUE(importCoordinates->setOption<CoordinateImportFileFormat::CoordinateImportFormat>(
            "FileFormat",
            CoordinateImportFileFormat::coordinateImportFileFormat().enumerationByIndex(initialCoordinates.formatIndex())));
        ASSERT_TRUE(root->addEdge({"InsertO", "Configuration", "Import", "Configuration"}));
    }

    // Add GR node and link to the import node
    auto grNode = root->createNode("GR", "GRs");
    ASSERT_TRUE(grNode);
    ASSERT_TRUE(
        root->addEdge({initialCoordinates.hasFilename() ? "Import" : "Insert", "Configuration", "GRs", "Configuration"}));

    // Create the SQ node
    auto sqNode = root->createNode("SQ", "SQs");
    ASSERT_TRUE(sqNode);
    ASSERT_TRUE(root->addEdge({"GRs", "UnweightedGR", "SQs", "UnweightedGR"}));

    // Add Bragg node
    auto braggNode = root->createNode("Bragg", "Bragg01");
    ASSERT_TRUE(braggNode);
    ASSERT_TRUE(
        root->addEdge({initialCoordinates.hasFilename() ? "Import" : "Insert", "Configuration", "Bragg01", "Configuration"}));
    ASSERT_TRUE(root->addEdge({"SQs", "UnweightedSQ", "Bragg01", "UnweightedSQ"}));

    // Add in NeutronSQ
    Data1DImportFileFormat referenceData = {"epsr25/mgo500-555/mgo.EPSR.u01", Data1DImportFileFormat::Data1DImportFormat::XY, 1,
                                            2};
    auto neutronSQNode = root->createNode("NeutronSQ", "NeutronSQ01");
    ASSERT_TRUE(neutronSQNode);
    ASSERT_TRUE(root->addEdge({"SQs", "UnweightedGR", "NeutronSQ01", "UnweightedGR"}));
    ASSERT_TRUE(root->addEdge({"Bragg01", "UnweightedSQ", "NeutronSQ01", "UnweightedSQ"}));

    // Set reference F(Q) data
    if (referenceData.hasFilename())
    {
        auto data1DImportNode = root->createNode("Data1DImport", std::format("{}-Reference", "NeutronSQ01"));
        ASSERT_TRUE(data1DImportNode);
        ASSERT_TRUE(data1DImportNode->setOption<std::string>("FilePath", std::string(referenceData.filename())));
        ASSERT_TRUE(data1DImportNode->setOption<Data1DImportFileFormat::Data1DImportFormat>(
            "ImportFormat", Data1DImportFileFormat::data1DImportFormat().enumerationByIndex(referenceData.formatIndex())));
        ASSERT_TRUE(root->addEdge({std::format("{}-Reference", "NeutronSQ01"), "Data", "NeutronSQ01", "ReferenceData"}));
    }
}

} // namespace UnitTest
