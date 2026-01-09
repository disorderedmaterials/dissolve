// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodes/loopGraph.h"

LoopGraph::LoopGraph(Graph *parentGraph) : Graph(parentGraph)
{
    addOption<Number>("NLoops", "Number of loops (iterations) to perform", nIterations_);
    loopBacks_ = dynamic_cast<LoopBacksNode *>(addNode(std::make_unique<LoopBacksNode>(this), "LoopBacks"));
}

/*
 * Definitions (Virtuals)
 */

// Return node name
std::string_view LoopGraph::name() const { return "Loop"; }

// Return type of the node
std::string_view LoopGraph::type() const { return "Loop"; }

// Return short summary of the node's purpose
std::string_view LoopGraph::summary() const { return "Loop the contained graph"; }

// Number of loops (iterations) to perform
const int LoopGraph::nIterations() const { return nIterations_.asInteger(); }

// Loop backs
LoopBacksNode *LoopGraph::loopBacks() { return loopBacks_; }

// Loop edges
Graph::Edges &LoopGraph::loopEdges() { return loopEdges_; }

// Current loop iteration
int LoopGraph::currentIteration() { return i_; }

// Set the loopbacks corresponding to the graph inputs
void LoopGraph::setLoopBacks()
{
    auto &sources = proxyInputs().outputs();

    for (const auto &[name, param] : sources)
        loopBacks_->inputs().insert_or_assign(name, param);
}

// Release loopback by name
void LoopGraph::releaseLoopBack(const std::string &name)
{
    auto inputs = loopBacks_->inputs();
    auto it = inputs.find(name);
    if (it != inputs.end())
        inputs.erase(it);
}

// Add edge between nodes
bool LoopGraph::addEdge(const EdgeDefinition &definition)
{
    if (dynamic_cast<InputsNode *>(parentGraph()->node(definition.sourceNode)))
        setLoopBacks();
    else if (loopBacks_->findInput(definition.targetInput))
    {
        auto edge =
            Edge::create(this, {definition.sourceNode, definition.sourceOutput, definition.targetNode, definition.targetInput});
        if (!edge)
            return false;

        loopEdges_.emplace_back(LoopEdge::makeLoopEdge(edge.release(), proxyInputs()));

        return true;
    }

    return Graph::addEdge(definition);
}

// Remove edge between nodes
bool LoopGraph::removeEdge(const EdgeDefinition &definition)
{
    if (!Graph::removeEdge(definition))
    {
        auto loopEdge = findLoopEdge(definition);
        if (removeEdge(static_cast<LoopEdge *>(loopEdge)))
            releaseLoopBack(definition.targetInput);
        else
            return false;
    }
    return true;
}

bool LoopGraph::removeEdge(LoopEdge *edgeToRemove)
{
    if (!edgeToRemove)
        return Messenger::error("LoopEdge doesn't exist, so can't remove it.\n");

    auto edgePtr = static_cast<Edge *>(edgeToRemove);
    std::erase_if(loopEdges_, [edgePtr](const auto &edge) { return edge.get() == edgePtr; });

    return true;
}

// Find edge between nodes
LoopEdge *LoopGraph::findLoopEdge(const EdgeDefinition &definition) const
{
    auto it = std::find_if(loopEdges_.begin(), loopEdges_.end(),
                           [definition](const auto &edge)
                           {
                               auto def = edge->definition();
                               return def.sourceNode == definition.sourceNode && def.sourceOutput == definition.sourceOutput &&
                                      def.targetInput == definition.targetInput;
                           });

    if (it != loopEdges_.end())
        return static_cast<LoopEdge *>(it->get());

    return {};
}

/*
 * Processing & Validity
 */

// Perform processing
NodeConstants::ProcessResult LoopGraph::process()
{
    const auto N = nIterations_.asInteger();
    if (N < 1)
        return NodeConstants::ProcessResult::Unchanged;

    for (i_ = 1; i_ <= N; ++i_)
        if ((i_ > 1 && (loopBacks_->run() == NodeConstants::ProcessResult::Failed)) ||
            (Graph::process() == NodeConstants::ProcessResult::Failed))
            return NodeConstants::ProcessResult::Failed;

    return NodeConstants::ProcessResult::Success;
}
