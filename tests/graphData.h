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
#include "nodes/importConfigurationCoordinates.h"
#include "nodes/insert.h"
#include "nodes/neutronSQ/neutronSQ.h"
#include "nodes/species.h"
#include "nodes/setCoordinates.h"
#include "nodes/sq/sq.h"
#include "nodes/xRaySQ/xRaySQ.h"
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

    private:
    // Current most recently appended node in graph
    Node *head_{nullptr};

    /*
     * Graph Creation Helpers
     */
    public:
    // Returns pointer to current top node in graph
    Node *fetchHead() const { return head_; }
    // Returns reference to current top node in graph, cast to the known node type
    template <class NodeType> NodeType *head() const { return static_cast<NodeType *>(head_); }
    // Append new node to the graph
    Node *appendNode(const std::string &nodeType, const std::optional<std::string> &name = {})
    {
        auto node = name.has_value() ? createNode(nodeType, *name) : createNode(nodeType);

        if (!node)
            return nullptr;

        head_ = node;

        return node;
    }
    // Create species insertion node chain
    Node *createAndInsertSpecies(Node *cfgSourceNode, const std::vector<std::pair<std::string, int>> &species, double rho,
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
            addNode(std::move(speciesUnique), speciesNode.name());

            auto insertNodeName = std::format("Insert-{}", speciesNode.name());
            EXPECT_TRUE(appendNode("Insert", insertNodeName));
            EXPECT_TRUE(fetchHead()->setInput<Number>("Population", population));
            EXPECT_TRUE(fetchHead()->setInput<Number>("Density", rho));
            EXPECT_TRUE(fetchHead()->setOption("BoxAction", boxActionStyle));
            EXPECT_TRUE(fetchHead()->setOption<Units::DensityUnits>("DensityUnits", rhoUnits));
            EXPECT_TRUE(addEdge({std::string(speciesNode.name()), "Species", insertNodeName, "Species"}));
            EXPECT_TRUE(addEdge({std::string(cfgSourceNode->name()), "Configuration", insertNodeName, "Configuration"}));

            cfgSourceNode = fetchHead();
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

        // Set up atom types
        auto atomType = species->addAtomType(element, Elements::symbol(element));
        atomType->interactionPotential().setFormAndParameters(potential.form(), potential.parameters());
        species->addAtom(element, {}, 0.0, atomType);

        // Create isotopologues
        for (auto isotope : Sears91::isotopes(element))
        {
            auto iso = species->addIsotopologue(std::format("{}{}", Elements::symbol(element), Sears91::A(isotope)));
            iso->setAtomTypeIsotope(atomType, isotope);
        }

        return speciesNodeUniquePtr;
    }
    // Create species from TOML file
    static std::unique_ptr<SpeciesNode> loadTOMLSpecies(std::string_view path)
    {
        // Add species node
        auto speciesNodeUniquePtr = std::make_unique<SpeciesNode>(nullptr);
        auto speciesNodePtr = speciesNodeUniquePtr.get();
        auto &species = speciesNodePtr->species();

        SerialisedValue contents = toml::parse(std::string(path));
        if (contents.contains("species"))
        {
            species.deserialise(contents["species"]);
            auto name = contents["species"]["name"].as_string();
            species.setName(name.str);
        }

        return speciesNodeUniquePtr;
    }
    // Create basic configuration graph, returning the last node
    Node *createConfiguration(std::string name, const std::vector<std::pair<std::string, int>> &species, double rho,
                              Units::DensityUnits rhoUnits = Units::DensityUnits::AtomsPerAngstromUnits)
    {
        // Create configuration and SetCell nodes
        EXPECT_TRUE(appendNode("Configuration", name));
        EXPECT_TRUE(appendNode("SetCell"));
        EXPECT_TRUE(addEdge({name, "Configuration", "SetCell", "Configuration"}));

        // Add Species and Insert nodes
        return createAndInsertSpecies(fetchHead(), species, rho, rhoUnits, InsertNode::BoxActionStyle::AddVolume);
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
        EXPECT_TRUE(addEdge({name, "Configuration", "SetCell", "Configuration"}));

        // Add Species and Insert nodes
        return createAndInsertSpecies(fetchHead(), species, 0.1, Units::DensityUnits::AtomsPerAngstromUnits,
                                      InsertNode::BoxActionStyle::None);
    }
    // Append an import coordinates node
    Node *appendImportCoordinates(CoordinateImportFileFormat fileFormat, bool supercell = false)
    {
        const auto cfgSourceNode = fetchHead();

        EXPECT_TRUE(appendNode("ImportConfigurationCoordinates"));
        EXPECT_TRUE(fetchHead()->setOption<std::string>("FilePath", std::string(fileFormat.filename())));
        EXPECT_TRUE(fetchHead()->setOption<CoordinateImportFileFormat::CoordinateImportFormat>(
            "FileFormat",
            CoordinateImportFileFormat::coordinateImportFileFormat().enumerationByIndex(fileFormat.formatIndex())));
        EXPECT_TRUE(addEdge({std::string(cfgSourceNode->name()), supercell ? "SupercellConfiguration" : "Configuration",
                             "ImportConfigurationCoordinates", "Configuration"}));

        return head<ImportConfigurationCoordinatesNode>();
    }
    // Append a set coordinates node with a structure import input
    Node *appendSetCoordinates(std::string_view importNodeType, std::string filePath)
    {
        const auto cfgSourceNode = fetchHead();

        EXPECT_TRUE(appendNode("SetCoordinates"));
        auto structureNode = createNode(importNodeType);
        EXPECT_TRUE(structureNode);
        EXPECT_TRUE(structureNode->setOption<std::string>("FilePath", filePath));

        EXPECT_TRUE(addEdge({std::string(structureNode->name()), "Structure", "SetCoordinates", "Structure"}));

        EXPECT_TRUE(addEdge({std::string(cfgSourceNode->name()), "Configuration", "SetCoordinates", "Configuration"}));

        return head<SetCoordinatesNode>();
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

        EXPECT_TRUE(addEdge({std::string(fetchHead()->name()), "Configuration", "GR", "Configuration"}));

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

        EXPECT_TRUE(appendNode("NeutronSQ", name));
        EXPECT_TRUE(fetchHead()->setOption("Isotopologues", isotopologueSet));
        EXPECT_TRUE(fetchHead()->setOption("Exchangeables", exchangeables));
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

        return head<NeutronSQNode>();
    }
    // Create an XRaySQ node with optional reference data
    XRaySQNode *appendXRaySQ(SQNode *sqNode, std::string name, Data1DImportFileFormat referenceData = Data1DImportFileFormat())
    {
        EXPECT_TRUE(appendNode("XRaySQ", name));
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
