// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "main/dissolve.h"
#include "nodes/atomicSpecies.h"
#include "nodes/configuration.h"
#include "nodes/dissolve.h"
#include "nodes/insert.h"
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

// Create an Argon graph in the supplied root node
inline void createArgonGraph(Graph *root, int population = 1000)
{
    /*
     *    Configuration (Bulk)
     *    ------------------
     *    -   Configuration-o ---+
     *    -                |      \        Insert (Insert)
     *    -----------------/       \       ------------------
     *                              +---- o-Configuration   |
     *    AtomicSpecies (Ar)      +------ o-Species         |
     *    ------------------     /         -----------------/
     *    -         Species-o --+
     *    -                |
     *    -----------------/
     */

    // Create nodes
    auto arNode = root->addNode(std::make_unique<AtomicSpeciesNode>(root, Elements::Ar), "Ar");
    ASSERT_TRUE(arNode);
    auto configurationNode = root->createNode("Configuration", "Bulk");
    ASSERT_TRUE(configurationNode);
    auto insertNode = root->createNode("Insert", "Insert");
    ASSERT_TRUE(insertNode);

    // Create edges
    ASSERT_TRUE(root->addEdge({"Ar", "Species", "Insert", "Species"}));
    ASSERT_TRUE(root->addEdge({"Bulk", "Configuration", "Insert", "Configuration"}));

    // Set configuration contents
    ASSERT_TRUE(insertNode->setInput<Number>("Population", population));
    ASSERT_TRUE(insertNode->setInput<Number>("Density", 0.0213));
    ASSERT_TRUE(insertNode->setOption<Units::DensityUnits>("DensityUnits", Units::DensityUnits::AtomsPerAngstromUnits));
}

// Create a water graph in the supplied root node
inline void createWater1000Graph(Graph *root, CoordinateImportFileFormat initialCoordinates, bool addNeutronSQ = false,
                                 bool addXRaySQ = false)
{
    // Create species and configuration
    auto waterNode = createWater(root);
    ASSERT_TRUE(waterNode);
    auto configurationNode = root->createNode("Configuration", "Bulk");
    ASSERT_TRUE(configurationNode);
    auto insertNode = root->createNode("Insert");
    ASSERT_TRUE(insertNode);
    ASSERT_TRUE(root->addEdge({"Water", "Species", "Insert", "Species"}));
    ASSERT_TRUE(root->addEdge({"Bulk", "Configuration", "Insert", "Configuration"}));
    ASSERT_TRUE(insertNode->setInput<Number>("Population", 1000));
    ASSERT_TRUE(insertNode->setInput<Number>("Density", 0.1));
    ASSERT_TRUE(insertNode->setOption<Units::DensityUnits>("DensityUnits", Units::DensityUnits::AtomsPerAngstromUnits));

    // Import reference coordinates
    auto importCoordinates = root->createNode("ImportConfigurationCoordinates", "Import");
    ASSERT_TRUE(importCoordinates->setOption<std::string>("FilePath", std::string(initialCoordinates.filename())));
    ASSERT_TRUE(importCoordinates->setOption<CoordinateImportFileFormat::CoordinateImportFormat>(
        "FileFormat",
        CoordinateImportFileFormat::coordinateImportFileFormat().enumerationByIndex(initialCoordinates.formatIndex())));
    ASSERT_TRUE(root->addEdge({"Insert", "Configuration", "Import", "Configuration"}));

    // Add GR node and link to the import node
    auto grNode = root->createNode("GR");
    ASSERT_TRUE(grNode);
    ASSERT_TRUE(root->addEdge({"Import", "Configuration", "GR", "Configuration"}));

    Node *sqNode = nullptr;

    // Add in NeutronSQ?
    if (addNeutronSQ)
    {
        // Create the SQ node
        sqNode = root->createNode("SQ");
        ASSERT_TRUE(sqNode);
        ASSERT_TRUE(root->addEdge({"GR", "UnweightedGR", "SQ", "UnweightedGR"}));

        auto h2o = root->createNode("NeutronSQ", "H2O");
        ASSERT_TRUE(h2o);
        ASSERT_TRUE(root->addEdge({"SQ", "UnweightedGR", "H2O", "UnweightedGR"}));
        ASSERT_TRUE(root->addEdge({"SQ", "UnweightedSQ", "H2O", "UnweightedSQ"}));

        auto d2o = root->createNode("NeutronSQ", "D2O");
        ASSERT_TRUE(d2o);
        ASSERT_TRUE(root->addEdge({"SQ", "UnweightedGR", "D2O", "UnweightedGR"}));
        ASSERT_TRUE(root->addEdge({"SQ", "UnweightedSQ", "D2O", "UnweightedSQ"}));

        auto hdo = root->createNode("NeutronSQ", "HDO");
        ASSERT_TRUE(hdo);
        ASSERT_TRUE(root->addEdge({"SQ", "UnweightedGR", "HDO", "UnweightedGR"}));
        ASSERT_TRUE(root->addEdge({"SQ", "UnweightedSQ", "HDO", "UnweightedSQ"}));
    }

    // Add in XRaySQ?
    if (addXRaySQ)
    {
        // Create the SQ node if needed
        if (!sqNode)
        {
            sqNode = root->createNode("SQ");
            ASSERT_TRUE(sqNode);
            ASSERT_TRUE(root->addEdge({"GR", "UnweightedGR", "SQ", "UnweightedGR"}));
        }

        auto h2ox = root->createNode("XRaySQ", "H2OX");
        ASSERT_TRUE(h2ox);
        ASSERT_TRUE(root->addEdge({"SQ", "UnweightedGR", "H2OX", "UnweightedGR"}));
        ASSERT_TRUE(root->addEdge({"SQ", "UnweightedSQ", "H2OX", "UnweightedSQ"}));
    }
}

} // namespace UnitTest
