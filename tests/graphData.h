// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "classes/isotopologueSet.h"
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
    // Create nodes
    auto arNode = createArgon(root);
    ASSERT_TRUE(arNode);

    auto configurationNode = root->createNode("Configuration", "Bulk");
    ASSERT_TRUE(configurationNode);
    auto insertNode = root->createNode("Insert", "Insert");
    ASSERT_TRUE(insertNode);

    // Create edges
    ASSERT_TRUE(root->addEdge({"Argon", "Species", "Insert", "Species"}));
    ASSERT_TRUE(root->addEdge({"Bulk", "Configuration", "Insert", "Configuration"}));

    // Set configuration contents
    ASSERT_TRUE(insertNode->setInput<Number>("Population", population));
    ASSERT_TRUE(insertNode->setInput<Number>("Density", 0.0213));
    ASSERT_TRUE(insertNode->setOption<Units::DensityUnits>("DensityUnits", Units::DensityUnits::AtomsPerAngstromUnits));
}

// Create a water graph in the supplied root node
inline void createWater1000Graph(Graph *root, CoordinateImportFileFormat initialCoordinates)
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

    // Create the SQ node
    auto sqNode = root->createNode("SQ");
    ASSERT_TRUE(sqNode);
    ASSERT_TRUE(root->addEdge({"GR", "UnweightedGR", "SQ", "UnweightedGR"}));

    // Add in NeutronSQ
    auto h2o = root->createNode("NeutronSQ", "H2O");
    ASSERT_TRUE(h2o);
    ASSERT_TRUE(root->addEdge({"SQ", "UnweightedGR", "H2O", "UnweightedGR"}));
    ASSERT_TRUE(root->addEdge({"SQ", "UnweightedSQ", "H2O", "UnweightedSQ"}));

    auto d2o = root->createNode("NeutronSQ", "D2O");
    ASSERT_TRUE(d2o);
    auto isoD = waterNode->species().findIsotopologue("D2O");
    ASSERT_TRUE(isoD);
    ASSERT_TRUE(d2o->setOption("Isotopologues", IsotopologueSet({{isoD, 1.0}})));
    ASSERT_TRUE(root->addEdge({"SQ", "UnweightedGR", "D2O", "UnweightedGR"}));
    ASSERT_TRUE(root->addEdge({"SQ", "UnweightedSQ", "D2O", "UnweightedSQ"}));

    auto hdo = root->createNode("NeutronSQ", "HDO");
    ASSERT_TRUE(hdo);
    ASSERT_TRUE(
        hdo->setOption("Isotopologues", IsotopologueSet({{isoD, 1.0}, {waterNode->species().naturalIsotopologue(), 1.0}})));
    ASSERT_TRUE(root->addEdge({"SQ", "UnweightedGR", "HDO", "UnweightedGR"}));
    ASSERT_TRUE(root->addEdge({"SQ", "UnweightedSQ", "HDO", "UnweightedSQ"}));

    // Add in XRaySQ
    // auto h2ox = root->createNode("XRaySQ", "H2OX");
    // ASSERT_TRUE(h2ox);
    // ASSERT_TRUE(root->addEdge({"SQ", "UnweightedGR", "H2OX", "UnweightedGR"}));
    // ASSERT_TRUE(root->addEdge({"SQ", "UnweightedSQ", "H2OX", "UnweightedSQ"}));
}

// Create a water graph in the supplied root node
inline void createWaterMethanolGraph(Graph *root)
{
    // Create species and configuration
    auto waterNode = createWater(root);
    ASSERT_TRUE(waterNode);
    auto methanolNode = createMethanol(root);
    ASSERT_TRUE(methanolNode);

    auto configurationNode = root->createNode("Configuration", "Bulk");
    ASSERT_TRUE(configurationNode);
    auto insertWaterNode = root->createNode("Insert", "InsertWater");
    ASSERT_TRUE(insertWaterNode);
    ASSERT_TRUE(root->addEdge({"Water", "Species", "InsertWater", "Species"}));
    ASSERT_TRUE(root->addEdge({"Bulk", "Configuration", "InsertWater", "Configuration"}));
    ASSERT_TRUE(insertWaterNode->setInput<Number>("Population", 300));
    ASSERT_TRUE(insertWaterNode->setInput<Number>("Density", 0.1));
    ASSERT_TRUE(insertWaterNode->setOption<Units::DensityUnits>("DensityUnits", Units::DensityUnits::AtomsPerAngstromUnits));
    auto insertMethanolNode = root->createNode("Insert", "InsertMethanol");
    ASSERT_TRUE(insertMethanolNode);
    ASSERT_TRUE(root->addEdge({"Methanol", "Species", "InsertMethanol", "Species"}));
    ASSERT_TRUE(root->addEdge({"InsertWater", "Configuration", "InsertMethanol", "Configuration"}));
    ASSERT_TRUE(insertMethanolNode->setInput<Number>("Population", 600));
    ASSERT_TRUE(insertMethanolNode->setInput<Number>("Density", 0.1));
    ASSERT_TRUE(insertMethanolNode->setOption<Units::DensityUnits>("DensityUnits", Units::DensityUnits::AtomsPerAngstromUnits));

    // Import reference coordinates
    auto importCoordinates = root->createNode("ImportConfigurationCoordinates", "Import");
    ASSERT_TRUE(importCoordinates->setOption<std::string>("FilePath", "epsr25/water300methanol600/watermeth.ato"));
    ASSERT_TRUE(importCoordinates->setOption<CoordinateImportFileFormat::CoordinateImportFormat>(
        "FileFormat", CoordinateImportFileFormat::CoordinateImportFormat::EPSR));
    ASSERT_TRUE(root->addEdge({"InsertMethanol", "Configuration", "Import", "Configuration"}));

    // Add GR node and link to the import node
    auto grNode = root->createNode("GR");
    ASSERT_TRUE(grNode);
    ASSERT_TRUE(root->addEdge({"Import", "Configuration", "GR", "Configuration"}));

    // Create the SQ node
    auto sqNode = root->createNode("SQ");
    ASSERT_TRUE(sqNode);
    ASSERT_TRUE(root->addEdge({"GR", "UnweightedGR", "SQ", "UnweightedGR"}));

    // Create NeutronSQ nodes
    auto h2o = waterNode->species().findIsotopologue("Natural");
    ASSERT_TRUE(h2o);
    auto d2o = waterNode->species().findIsotopologue("D2O");
    ASSERT_TRUE(d2o);
    auto methylD_OH = methanolNode->species().findIsotopologue("MethylD-OH");
    ASSERT_TRUE(methylD_OH);
    auto OD_methylH = methanolNode->species().findIsotopologue("OD-MethylH");
    ASSERT_TRUE(OD_methylH);
    auto methanolH = methanolNode->species().findIsotopologue("Natural");
    ASSERT_TRUE(methanolH);
    auto methanolD = methanolNode->species().findIsotopologue("Deuteriated");
    ASSERT_TRUE(methanolD);
    std::vector<std::tuple<std::string, std::vector<std::pair<const Isotopologue *, double>>>> samples = {
        {"HHH", {{h2o, 1.0}, {methanolH, 1.0}}},  {"H5H", {{h2o, 1.0}, {methanolH, 0.5}, {OD_methylH, 0.5}}},
        {"DHH", {{d2o, 1.0}, {methanolH, 1.0}}},  {"HDH", {{h2o, 1.0}, {OD_methylH, 1.0}}},
        {"HHD", {{h2o, 1.0}, {methylD_OH, 1.0}}}, {"DDH", {{d2o, 1.0}, {OD_methylH, 1.0}}},
        {"HDD", {{h2o, 1.0}, {methanolD, 1.0}}},  {"DDD", {{d2o, 1.0}, {methanolD, 1.0}}}};
    for (const auto &[name, isotopologues] : samples)
    {
        auto neutronSQNode = root->createNode("NeutronSQ", name);
        ASSERT_TRUE(neutronSQNode);
        ASSERT_TRUE(neutronSQNode->setOption("Isotopologues", IsotopologueSet(isotopologues)));
        ASSERT_TRUE(root->addEdge({"SQ", "UnweightedGR", name, "UnweightedGR"}));
        ASSERT_TRUE(root->addEdge({"SQ", "UnweightedSQ", name, "UnweightedSQ"}));
    }
}

// Create a benzene graph in the supplied root node
inline void createBenzeneGraph(Graph *root)
{
    // Create species and configuration
    auto benzeneNode = createBenzene(root);
    ASSERT_TRUE(benzeneNode);
    auto configurationNode = root->createNode("Configuration", "Bulk");
    ASSERT_TRUE(configurationNode);
    auto insertNode = root->createNode("Insert");
    ASSERT_TRUE(insertNode);
    ASSERT_TRUE(root->addEdge({"Benzene", "Species", "Insert", "Species"}));
    ASSERT_TRUE(root->addEdge({"Bulk", "Configuration", "Insert", "Configuration"}));
    ASSERT_TRUE(insertNode->setInput<Number>("Population", 200));
    ASSERT_TRUE(insertNode->setInput<Number>("Density", 0.876));

    // Import reference coordinates
    auto importCoordinates = root->createNode("ImportConfigurationCoordinates", "Import");
    ASSERT_TRUE(importCoordinates->setOption<std::string>("FilePath", "epsr25/benzene200-neutron/boxbenz.ato"));
    ASSERT_TRUE(importCoordinates->setOption<CoordinateImportFileFormat::CoordinateImportFormat>(
        "FileFormat", CoordinateImportFileFormat::CoordinateImportFormat::EPSR));
    ASSERT_TRUE(root->addEdge({"Insert", "Configuration", "Import", "Configuration"}));

    // Add GR node and link to the import node
    auto grNode = root->createNode("GR");
    ASSERT_TRUE(grNode);
    ASSERT_TRUE(root->addEdge({"Import", "Configuration", "GR", "Configuration"}));

    // Create the SQ node
    auto sqNode = root->createNode("SQ");
    ASSERT_TRUE(sqNode);
    ASSERT_TRUE(root->addEdge({"GR", "UnweightedGR", "SQ", "UnweightedGR"}));

    // Add in NeutronSQ
    auto C6H6 = root->createNode("NeutronSQ", "C6H6");
    ASSERT_TRUE(C6H6);
    ASSERT_TRUE(root->addEdge({"SQ", "UnweightedGR", "C6H6", "UnweightedGR"}));
    ASSERT_TRUE(root->addEdge({"SQ", "UnweightedSQ", "C6H6", "UnweightedSQ"}));

    auto C6D6 = root->createNode("NeutronSQ", "C6D6");
    ASSERT_TRUE(C6D6);
    auto isoD = benzeneNode->species().findIsotopologue("C6D6");
    ASSERT_TRUE(isoD);
    ASSERT_TRUE(C6D6->setOption("Isotopologues", IsotopologueSet({{isoD, 1.0}})));
    ASSERT_TRUE(root->addEdge({"SQ", "UnweightedGR", "C6D6", "UnweightedGR"}));
    ASSERT_TRUE(root->addEdge({"SQ", "UnweightedSQ", "C6D6", "UnweightedSQ"}));

    auto HD = root->createNode("NeutronSQ", "5050");
    ASSERT_TRUE(HD);
    ASSERT_TRUE(
        HD->setOption("Isotopologues", IsotopologueSet({{benzeneNode->species().naturalIsotopologue(), 1.0}, {isoD, 1.0}})));
    ASSERT_TRUE(root->addEdge({"SQ", "UnweightedGR", "5050", "UnweightedGR"}));
    ASSERT_TRUE(root->addEdge({"SQ", "UnweightedSQ", "5050", "UnweightedSQ"}));

    // Add in XRaySQ?
    // auto X = root->createNode("XRaySQ", "X");
    // ASSERT_TRUE(X);
    // ASSERT_TRUE(root->addEdge({"SQ", "UnweightedGR", "X", "UnweightedGR"}));
    // ASSERT_TRUE(root->addEdge({"SQ", "UnweightedSQ", "X", "UnweightedSQ"}));
}

} // namespace UnitTest
