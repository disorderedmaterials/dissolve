// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include "registry.h"
#include "add.h"
#include "atomShake/atomShake.h"
#include "atomicSpecies.h"
#include "configuration.h"
#include "derivative.h"
#include "dissolve.h"
#include "dotProduct.h"
#include "integrator.h"
#include "md/md.h"
#include "multiply.h"
#include "subtract.h"
#include "vec3Assembly.h"
#include "vec3Decomposition.h"
#include <memory>
#include <string>

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

    producers_ = {{"Add", makeDerivedNode<AddNode>()},
                  {"AtomShake", makeDerivedNode<AtomShakeNode>()},
                  {"AtomicSpecies", makeDerivedNode<AtomicSpeciesNode>()},
                  {"Configuration", makeDerivedNode<ConfigurationNode>()},
                  {"Derivative", makeDerivedNode<DerivativeNode>()},
                  {"DotProduct", makeDerivedNode<DotProductNode>()},
                  {"Integrator", makeDerivedNode<Integrator1DNode>()},
                  {"MD", makeDerivedNode<MDNode>()},
                  {"Multiply", makeDerivedNode<MultiplyNode>()},
                  {"Subtract", makeDerivedNode<SubtractNode>()},
                  {"Vec3Assembly", makeDerivedNode<Vec3AssemblyNode>()},
                  {"Vec3Decomposition", makeDerivedNode<Vec3DecompositionNode>()}};
}

// Check whether the supplied node type is known
bool NodeRegistry::hasNodeType(std::string_view nodeType)
{
    instantiateNodeProducers();
    return producers_.contains(nodeType);
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
