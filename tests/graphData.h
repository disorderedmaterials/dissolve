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

// Create a NeutronSQ node with optional reference data
inline void addNeutronSQ(Graph *root, std::string name, const IsotopologueSet &isotopologues = {},
                         const Exchangeables &exchangeables = {},
                         Data1DImportFileFormat referenceData = Data1DImportFileFormat(), std::string sqNodeName = "SQ")
{
    auto neutronSQNode = root->createNode("NeutronSQ", name);
    ASSERT_TRUE(neutronSQNode);
    ASSERT_TRUE(neutronSQNode->setOption("Isotopologues", isotopologues));
    ASSERT_TRUE(neutronSQNode->setOption("Exchangeables", exchangeables));
    ASSERT_TRUE(root->addEdge({sqNodeName, "UnweightedGR", name, "UnweightedGR"}));
    ASSERT_TRUE(root->addEdge({sqNodeName, "UnweightedSQ", name, "UnweightedSQ"}));

    // Set reference F(Q) data
    if (referenceData.hasFilename())
    {
        auto data1DImportNode = root->createNode("Data1DImport", std::format("{}-Reference", name));
        ASSERT_TRUE(data1DImportNode);
        ASSERT_TRUE(data1DImportNode->setOption<std::string>("FilePath", std::string(referenceData.filename())));
        ASSERT_TRUE(data1DImportNode->setOption<Data1DImportFileFormat::Data1DImportFormat>(
            "ImportFormat", Data1DImportFileFormat::data1DImportFormat().enumerationByIndex(referenceData.formatIndex())));
        ASSERT_TRUE(root->addEdge({std::format("{}-Reference", name), "Data", name, "ReferenceData"}));
    }
}

// Create an XRaySQ node with optional reference data
inline void addXRaySQ(Graph *root, std::string name, Data1DImportFileFormat referenceData = Data1DImportFileFormat(),
                      std::string sqNodeName = "SQ")
{
    auto xRaySQNode = root->createNode("XRaySQ", name);
    ASSERT_TRUE(xRaySQNode);
    ASSERT_TRUE(root->addEdge({sqNodeName, "UnweightedGR", name, "UnweightedGR"}));
    ASSERT_TRUE(root->addEdge({sqNodeName, "UnweightedSQ", name, "UnweightedSQ"}));

    // Set reference F(Q) data
    if (referenceData.hasFilename())
    {
        auto data1DImportNode = root->createNode("Data1DImport", std::format("{}-Reference", name));
        ASSERT_TRUE(data1DImportNode);
        ASSERT_TRUE(data1DImportNode->setOption<std::string>("FilePath", std::string(referenceData.filename())));
        ASSERT_TRUE(data1DImportNode->setOption<Data1DImportFileFormat::Data1DImportFormat>(
            "ImportFormat", Data1DImportFileFormat::data1DImportFormat().enumerationByIndex(referenceData.formatIndex())));
        ASSERT_TRUE(root->addEdge({std::format("{}-Reference", name), "Data", name, "ReferenceData"}));
    }
}

// Create an Argon graph in the supplied root node
inline void createArgonGraph(Graph *root, int population = 1000,
                             CoordinateImportFileFormat initialCoordinates = CoordinateImportFileFormat())
{
    // Create species
    auto arNode = createArgon(root);
    ASSERT_TRUE(arNode);

    // Create configuration
    auto configurationNode = root->createNode("Configuration", "Bulk");
    ASSERT_TRUE(configurationNode);
    auto insertNode = root->createNode("Insert", "Insert");
    ASSERT_TRUE(insertNode);
    ASSERT_TRUE(insertNode->setInput<Number>("Population", population));
    ASSERT_TRUE(insertNode->setInput<Number>("Density", 0.0213));
    ASSERT_TRUE(insertNode->setOption<Units::DensityUnits>("DensityUnits", Units::DensityUnits::AtomsPerAngstromUnits));
    ASSERT_TRUE(root->addEdge({"Argon", "Species", "Insert", "Species"}));
    ASSERT_TRUE(root->addEdge({"Bulk", "Configuration", "Insert", "Configuration"}));

    // Import reference coordinates?
    if (initialCoordinates.hasFilename())
    {
        auto importCoordinates = root->createNode("ImportConfigurationCoordinates", "Import");
        ASSERT_TRUE(importCoordinates->setOption<std::string>("FilePath", std::string(initialCoordinates.filename())));
        ASSERT_TRUE(importCoordinates->setOption<CoordinateImportFileFormat::CoordinateImportFormat>(
            "FileFormat",
            CoordinateImportFileFormat::coordinateImportFileFormat().enumerationByIndex(initialCoordinates.formatIndex())));
        ASSERT_TRUE(root->addEdge({"Insert", "Configuration", "Import", "Configuration"}));
    }

    // Add GR node and link in configuration / import node
    auto grNode = root->createNode("GR", "GR");
    ASSERT_TRUE(grNode);
    ASSERT_TRUE(grNode->setOption("Averaging", std::optional<Number>()));
    ASSERT_TRUE(grNode->setOption("IntraBroadening", Function1DWrapper()));
    ASSERT_TRUE(
        root->addEdge({initialCoordinates.hasFilename() ? "Import" : "Insert", "Configuration", "GR", "Configuration"}));

    // Create the SQ node
    auto sqNode = root->createNode("SQ");
    ASSERT_TRUE(sqNode);
    ASSERT_TRUE(root->addEdge({"GR", "UnweightedGR", "SQ", "UnweightedGR"}));

    // Add NeutronSQ node and reference data
    auto neutronSQNode = root->createNode("NeutronSQ", "NeutronSQ");
    ASSERT_TRUE(neutronSQNode);
    ASSERT_TRUE(neutronSQNode->setOption<StructureFactors::NormalisationType>("ReferenceNormalisedTo",
                                                                              StructureFactors::SquareOfAverageNormalisation));
    ASSERT_TRUE(neutronSQNode->setOption<IsotopologueSet>(
        "Isotopologues", IsotopologueSet({{arNode->species().findIsotopologue("Ar36"), 1.0}})));
    ASSERT_TRUE(root->addEdge({"SQ", "UnweightedGR", "NeutronSQ", "UnweightedGR"}));
    ASSERT_TRUE(root->addEdge({"SQ", "UnweightedSQ", "NeutronSQ", "UnweightedSQ"}));

    // Set reference F(Q) data
    auto data1DImportNode = root->createNode("Data1DImport", "Yarnell");
    ASSERT_TRUE(data1DImportNode);
    ASSERT_TRUE(data1DImportNode->setOption<std::string>("FilePath", "dissolve2/argon/yarnell.sq"));
    ASSERT_TRUE(data1DImportNode->setOption<Data1DImportFileFormat::Data1DImportFormat>(
        "ImportFormat", Data1DImportFileFormat::Data1DImportFormat::XY));
    ASSERT_TRUE(data1DImportNode->setOption<std::optional<Number>>("RemoveAverageFromX", 9.0));
    ASSERT_TRUE(root->addEdge({"Yarnell", "Data", "NeutronSQ", "ReferenceData"}));
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

    // Insert Mg species
    auto insertMgNode = root->createNode("Insert");
    ASSERT_TRUE(insertMgNode);
    ASSERT_TRUE(root->addEdge({"Mg", "Species", "Insert", "Species"}));
    ASSERT_TRUE(root->addEdge({"Crystal", "Configuration", "Insert", "Configuration"}));
    ASSERT_TRUE(insertMgNode->setInput<Number>("Population", populationMG));
    ASSERT_TRUE(insertMgNode->setInput<Number>("Density", 0.1));
    ASSERT_TRUE(insertMgNode->setOption<Units::DensityUnits>("DensityUnits", Units::DensityUnits::AtomsPerAngstromUnits));

    // Insert O species
    auto insertONode = root->createNode("Insert");
    ASSERT_TRUE(insertONode);
    ASSERT_TRUE(root->addEdge({"O", "Species", "Insert", "Species"}));
    ASSERT_TRUE(root->addEdge({"Crystal", "Configuration", "Insert", "Configuration"}));
    ASSERT_TRUE(insertONode->setInput<Number>("Population", populationO));
    ASSERT_TRUE(insertONode->setInput<Number>("Density", 0.1));
    ASSERT_TRUE(insertONode->setOption<Units::DensityUnits>("DensityUnits", Units::DensityUnits::AtomsPerAngstromUnits));

    // Import reference coordinates
    if (initialCoordinates.hasFilename())
    {
        auto importCoordinates = root->createNode("ImportConfigurationCoordinates", "Import");
        ASSERT_TRUE(importCoordinates->setOption<std::string>("FilePath", std::string(initialCoordinates.filename())));
        ASSERT_TRUE(importCoordinates->setOption<CoordinateImportFileFormat::CoordinateImportFormat>(
            "FileFormat",
            CoordinateImportFileFormat::coordinateImportFileFormat().enumerationByIndex(initialCoordinates.formatIndex())));
        ASSERT_TRUE(root->addEdge({"Insert", "Configuration", "Import", "Configuration"}));
    }

    // Add Bragg node
    auto braggNode = root->createNode("Bragg", "Bragg01");
    ASSERT_TRUE(braggNode);
    ASSERT_TRUE(root->addEdge({"Insert", "Configuration", "Bragg01", "Configuration"}));
    ASSERT_TRUE(braggNode->setOption<Vector3i>("Multiplicity", {5, 5, 5}));
    ASSERT_TRUE(braggNode->setOption("QMax", 20.0));

    // Add GR node and link to the import node
    auto grNode = root->createNode("GR", "GRs");
    ASSERT_TRUE(grNode);
    ASSERT_TRUE(
        root->addEdge({initialCoordinates.hasFilename() ? "Import" : "Insert", "Configuration", "GRs", "Configuration"}));

    // Create the SQ node
    auto sqNode = root->createNode("SQ", "SQs");
    ASSERT_TRUE(sqNode);
    ASSERT_TRUE(root->addEdge({"GRs", "UnweightedGR", "SQs", "UnweightedGR"}));
    ASSERT_TRUE(sqNode->setOption("QMin", 0.05));
    ASSERT_TRUE(sqNode->setOption("QMax", 19.0));
    ASSERT_TRUE(sqNode->setOption("QDelta", 0.05));
    ASSERT_TRUE(sqNode->setOption<Function1DWrapper>("QBroadening", {Functions1D::Form::OmegaDependentGaussian, {0.0, 0.02}}));
    ASSERT_TRUE(sqNode->setOption("WindowFunction", WindowFunction::Form::Lorch0));
    ASSERT_TRUE(
        sqNode->setOption<Function1DWrapper>("BraggQBroadening", {Functions1D::Form::GaussianC2, {0.0235482, 0.0470964}}));

    // Add in NeutronSQ
    addNeutronSQ(root, "NeutronSQ01", {}, {},
                 {"epsr25/mgo500-555/mgo.EPSR.u01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 2});
}
// Create a water graph in the supplied root node
inline void createWaterGraph(Graph *root, int population,
                             CoordinateImportFileFormat initialCoordinates = CoordinateImportFileFormat())
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
    ASSERT_TRUE(insertNode->setInput<Number>("Population", population));
    ASSERT_TRUE(insertNode->setInput<Number>("Density", 0.1));
    ASSERT_TRUE(insertNode->setOption<Units::DensityUnits>("DensityUnits", Units::DensityUnits::AtomsPerAngstromUnits));

    // Import reference coordinates
    if (initialCoordinates.hasFilename())
    {
        auto importCoordinates = root->createNode("ImportConfigurationCoordinates", "Import");
        ASSERT_TRUE(importCoordinates->setOption<std::string>("FilePath", std::string(initialCoordinates.filename())));
        ASSERT_TRUE(importCoordinates->setOption<CoordinateImportFileFormat::CoordinateImportFormat>(
            "FileFormat",
            CoordinateImportFileFormat::coordinateImportFileFormat().enumerationByIndex(initialCoordinates.formatIndex())));
        ASSERT_TRUE(root->addEdge({"Insert", "Configuration", "Import", "Configuration"}));
    }

    // Add GR node and link to the import node
    auto grNode = root->createNode("GR");
    ASSERT_TRUE(grNode);
    ASSERT_TRUE(
        root->addEdge({initialCoordinates.hasFilename() ? "Import" : "Insert", "Configuration", "GR", "Configuration"}));

    // Create the SQ node
    auto sqNode = root->createNode("SQ");
    ASSERT_TRUE(sqNode);
    ASSERT_TRUE(root->addEdge({"GR", "UnweightedGR", "SQ", "UnweightedGR"}));

    // Add in NeutronSQ
    auto isoD = waterNode->species().findIsotopologue("D2O");
    ASSERT_TRUE(isoD);
    addNeutronSQ(root, "H2O", {}, {},
                 Data1DImportFileFormat("epsr25/water1000-neutron-xray/H2O.mint01",
                                        Data1DImportFileFormat::Data1DImportFormat::GudrunMint));
    addNeutronSQ(root, "D2O", IsotopologueSet({{isoD, 1.0}}), {},
                 Data1DImportFileFormat("epsr25/water1000-neutron-xray/D2O.mint01",
                                        Data1DImportFileFormat::Data1DImportFormat::GudrunMint));
    addNeutronSQ(root, "HDO", IsotopologueSet({{isoD, 1.0}, {waterNode->species().naturalIsotopologue(), 1.0}}),
                 Exchangeables({"HW"}),
                 Data1DImportFileFormat("epsr25/water1000-neutron-xray/HDO.mint01",
                                        Data1DImportFileFormat::Data1DImportFormat::GudrunMint));

    // Add in XRaySQ
    addXRaySQ(
        root, "H2Ox",
        Data1DImportFileFormat("epsr25/water1000-neutron-xray/PCCPfofq.txt", Data1DImportFileFormat::Data1DImportFormat::XY));
}
// Create a water graph in the supplied root node
inline void createWaterGraphDlPoly(Graph *root, int population,
                                   CoordinateImportFileFormat initialCoordinates = CoordinateImportFileFormat())
{
    // Create species and configuration
    auto waterNode = createWaterDlPoly(root);
    ASSERT_TRUE(waterNode);
    auto configurationNode = root->createNode("Configuration", "Bulk");
    ASSERT_TRUE(configurationNode);
    auto insertNode = root->createNode("Insert");
    ASSERT_TRUE(insertNode);
    ASSERT_TRUE(root->addEdge({"Water", "Species", "Insert", "Species"}));
    ASSERT_TRUE(root->addEdge({"Bulk", "Configuration", "Insert", "Configuration"}));
    ASSERT_TRUE(insertNode->setInput<Number>("Population", population));
    ASSERT_TRUE(insertNode->setInput<Number>("Density", 0.1));
    ASSERT_TRUE(insertNode->setOption<Units::DensityUnits>("DensityUnits", Units::DensityUnits::AtomsPerAngstromUnits));

    // Import reference coordinates
    if (initialCoordinates.hasFilename())
    {
        auto importCoordinates = root->createNode("ImportConfigurationCoordinates", "Import");
        ASSERT_TRUE(importCoordinates->setOption<std::string>("FilePath", std::string(initialCoordinates.filename())));
        ASSERT_TRUE(importCoordinates->setOption<CoordinateImportFileFormat::CoordinateImportFormat>(
            "FileFormat",
            CoordinateImportFileFormat::coordinateImportFileFormat().enumerationByIndex(initialCoordinates.formatIndex())));
        ASSERT_TRUE(root->addEdge({"Insert", "Configuration", "Import", "Configuration"}));
    }
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
        addNeutronSQ(root, name, isotopologues, Exchangeables({"HW", "HO"}));
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
    auto isoD = benzeneNode->species().findIsotopologue("C6D6");
    ASSERT_TRUE(isoD);
    addNeutronSQ(root, "C6H6");
    addNeutronSQ(root, "C6D6", IsotopologueSet({{isoD, 1.0}}));
    addNeutronSQ(root, "5050", IsotopologueSet({{benzeneNode->species().naturalIsotopologue(), 1.0}, {isoD, 1.0}}));

    // Add in XRaySQ?
    // auto X = root->createNode("XRaySQ", "X");
    // ASSERT_TRUE(X);
    // ASSERT_TRUE(root->addEdge({"SQ", "UnweightedGR", "X", "UnweightedGR"}));
    // ASSERT_TRUE(root->addEdge({"SQ", "UnweightedSQ", "X", "UnweightedSQ"}));
}

} // namespace UnitTest
