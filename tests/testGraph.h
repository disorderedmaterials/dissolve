// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "base/units.h"
#include "kernels/common.h"
#include "nodes/dissolve.h"
#include "tests/testing.h"
#include <gtest/gtest.h>

// Forward Declarations
class GRNode;
class IterableGraph;
class SpeciesNode;
class NeutronSQNode;
class SQNode;
class XRaySQNode;

namespace UnitTest
{
// Basic object setup for any Graph-based test
class TestGraph : public DissolveGraph
{
    public:
    TestGraph() : currentGraph_(this)
    {
        setEcho(true);
        PairPotential::setChargeSource(PairPotential::ChargeSource::Automatic);
    }
    ~TestGraph() { exportMermaidGraph(*this); }

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
    Node *fetchHead() const;
    // Returns the name of the current head node in the graph
    std::string fetchHeadName() const;
    // Returns reference to current top node in graph, cast to the known node type
    template <class NodeType> NodeType *head() const { return static_cast<NodeType *>(head_); }
    // Append new node to the graph
    Node *appendNode(const std::string &nodeType, const std::optional<std::string> &name = {});
    // Create species insertion node chain
    Node *createAndInsertSpecies(Node *cfgSourceNode, std::string cfgSourceOutput,
                                 const std::vector<std::pair<std::string, int>> &species, double rho,
                                 Units::DensityUnits rhoUnits = Units::DensityUnits::AtomsPerAngstromUnits,
                                 InsertNode::BoxActionStyle boxActionStyle = InsertNode::BoxActionStyle::AddVolume);

    public:
    // Create and return atomic SpeciesNode
    static std::unique_ptr<SpeciesNode> createAtomicSpecies(Elements::Element element,
                                                            InteractionPotential<ShortRangeFunctions> potential = {
                                                                ShortRangeFunctions::Form::Undefined, ""});
    // Create species from TOML file
    static std::unique_ptr<SpeciesNode> loadTOMLSpecies(std::string_view path);
    // Create a species node with structure and forcefield data sources
    SpeciesNode *createSpeciesFromStructureAndForcefield(std::string name, std::string structureNodeType,
                                                         std::string structureFilePath, std::shared_ptr<Forcefield> ff,
                                                         bool calculateBonding = true);
    // Create basic configuration graph, returning the last node
    Node *createConfiguration(std::string name, const std::vector<std::pair<std::string, int>> &species, double rho,
                              Units::DensityUnits rhoUnits = Units::DensityUnits::AtomsPerAngstromUnits);
    // Create basic configuration graph, returning the last node
    Node *createConfiguration(std::string name, const std::vector<std::pair<std::string, int>> &species,
                              const Vector3 &cellLengths, const Vector3 &cellAngles = {90.0, 90.0, 90.0});
    // Append a set coordinates node with a structure import input
    Node *appendSetCoordinates(std::string_view importNodeType, std::string filePath,
                               std::string sourceOutpuName = "Configuration");
    // Create a trajectory iterator subgraph
    IterableGraph *appendTrajectoryIterator(std::string trajectoryImportNodeType, std::string filePath);
    // Append GR and SQ nodes
    std::pair<GRNode *, SQNode *> appendGRSQ(bool noAveraging = false, bool noIntraBroadening = false);
    // Create a NeutronSQ node with optional reference data
    NeutronSQNode *appendNeutronSQ(SQNode *sqNode, std::string name,
                                   const std::vector<std::tuple<std::string, std::string, double>> isotopologues = {},
                                   std::string referenceData = {}, bool isHistogram = false);
    // Create an XRaySQ node with optional reference data
    XRaySQNode *appendXRaySQ(SQNode *sqNode, std::string name,
                             std::string referenceData = {}, bool isHistogram = false);
};
}; // namespace UnitTest
