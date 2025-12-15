// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include "nodes/registry.h"
#include "base/enumOptions.h"
#include "base/enumOptionsBase.h"
#include "math/averaging.h"
#include "math/windowFunction.h"
#include "nodes/add.h"
#include "nodes/atomicMC/atomicMC.h"
#include "nodes/configuration.h"
#include "nodes/data1DImport.h"
#include "nodes/derivative.h"
#include "nodes/dotProduct.h"
#include "nodes/edge.h"
#include "nodes/energy/energy.h"
#include "nodes/gr/gr.h"
#include "nodes/importConfigurationCoordinates.h"
#include "nodes/insert.h"
#include "nodes/integrator.h"
#include "nodes/loopGraph.h"
#include "nodes/md/md.h"
#include "nodes/multiply.h"
#include "nodes/neutronSQ/neutronSQ.h"
#include "nodes/numberNode.h"
#include "nodes/species.h"
#include "nodes/sq/sq.h"
#include "nodes/subtract.h"
#include "nodes/vec3Assembly.h"
#include "nodes/vec3Decomposition.h"
#include <memory>
#include <ranges>

// Static Singletons
std::map<std::string_view, NodeProducer> NodeRegistry::producers_;

// Makes unique pointer to derived node instance
template <typename T> NodeProducer makeDerivedNode()
{
    return [=](Graph *parent) -> std::unique_ptr<Node> { return std::make_unique<T>(parent); };
}

// Instantiate Node Producers
void NodeRegistry::instantiateNodeProducers()
{
    // Only need to do this once
    if (!producers_.empty())
        return;

    producers_ = {
        {"Add", makeDerivedNode<AddNode>()},
        {"AtomicMC", makeDerivedNode<AtomicMCNode>()},
        {"Configuration", makeDerivedNode<ConfigurationNode>()},
        {"Data1DImport", makeDerivedNode<Data1DImportNode>()},
        {"Derivative", makeDerivedNode<DerivativeNode>()},
        {"DotProduct", makeDerivedNode<DotProductNode>()},
        {"Energy", makeDerivedNode<EnergyNode>()},
        {"GR", makeDerivedNode<GRNode>()},
        {"Graph", makeDerivedNode<Graph>()},
        {"GR", makeDerivedNode<GRNode>()},
        {"ImportConfigurationCoordinates", makeDerivedNode<ImportConfigurationCoordinatesNode>()},
        {"Insert", makeDerivedNode<InsertNode>()},
        {"Integrator", makeDerivedNode<Integrator1DNode>()},
        {"Loop", makeDerivedNode<LoopGraph>()},
        {"MD", makeDerivedNode<MDNode>()},
        {"Multiply", makeDerivedNode<MultiplyNode>()},
        {"NeutronSQ", makeDerivedNode<NeutronSQNode>()},
        {"Number", makeDerivedNode<NumberNode>()},
        {"SQ", makeDerivedNode<SQNode>()},
        {"Species", makeDerivedNode<SpeciesNode>()},
        {"Subtract", makeDerivedNode<SubtractNode>()},
        {"Vec3Assembly", makeDerivedNode<Vec3AssemblyNode>()},
        {"Vec3Decomposition", makeDerivedNode<Vec3DecompositionNode>()},
    };
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
