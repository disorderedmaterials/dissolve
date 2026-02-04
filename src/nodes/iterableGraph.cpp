// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodes/iterableGraph.h"

IterableGraph::IterableGraph(Graph *parentGraph) : Graph(parentGraph)
{
    addOption<Number>("N", "Number of loops (iterations) to perform", nIterations_);
    loopBacks_ = dynamic_cast<LoopBacksNode *>(addNode(std::make_unique<LoopBacksNode>(this), "LoopBacks"));
}

/*
 * Definitions (Virtuals)
 */

// Return node name
std::string_view IterableGraph::name() const { return "Iterator"; }

// Return type of the node
std::string_view IterableGraph::type() const { return "Iterator"; }

// Return short summary of the node's purpose
std::string_view IterableGraph::summary() const { return "Loop the contained graph"; }

// Number of loops (iterations) to perform
const int IterableGraph::nIterations() const { return nIterations_.asInteger(); }

// Loop backs
LoopBacksNode *IterableGraph::loopBacks() { return loopBacks_; }

// Loop edges
Graph::Edges &IterableGraph::loopEdges() { return loopEdges_; }

// Current loop iteration
int IterableGraph::currentIteration() { return i_; }

// Set the loopbacks corresponding to the graph inputs
void IterableGraph::setLoopBacks()
{
    auto &sources = proxyInputs().outputs();

    for (const auto &[name, param] : sources)
        loopBacks_->inputs().insert_or_assign(name, param);
}

// Release loopback by name
void IterableGraph::releaseLoopBack(const std::string &name)
{
    auto inputs = loopBacks_->inputs();
    auto it = inputs.find(name);
    if (it != inputs.end())
        inputs.erase(it);
}

// Add edge between nodes
bool IterableGraph::addEdge(const EdgeDefinition &definition)
{
    if (dynamic_cast<InputsNode *>(parentGraph()->findNode(definition.sourceNode)))
        setLoopBacks();
    else if (loopBacks_->findInput(definition.targetInput))
    {
        auto edge =
            Edge::create(this, {definition.sourceNode, definition.sourceOutput, definition.targetNode, definition.targetInput});
        if (!edge)
            return false;

        loopEdges_.emplace_back(LoopEdge::makeLoopEdge(edge.release(), proxyInputs()));

        addOutputEdge(definition.sourceOutput, loopEdges_.back().get());

        return true;
    }

    return Graph::addEdge(definition);
}

// Remove edge between nodes
bool IterableGraph::removeEdge(const EdgeDefinition &definition)
{
    if (!Graph::removeEdge(definition))
    {
        auto loopEdge = findLoopEdge(definition);
        if (removeEdge(static_cast<LoopEdge *>(loopEdge)))
            releaseLoopBack(definition.targetInput);
        else
            return false;

        removeOutputEdge(definition.sourceOutput, static_cast<Edge *>(loopEdge));
    }
    return true;
}

bool IterableGraph::removeEdge(LoopEdge *edgeToRemove)
{
    if (!edgeToRemove)
        return Messenger::error("LoopEdge doesn't exist, so can't remove it.\n");

    auto edgePtr = static_cast<Edge *>(edgeToRemove);
    std::erase_if(loopEdges_, [edgePtr](const auto &edge) { return edge.get() == edgePtr; });

    return true;
}

// Find edge between nodes
LoopEdge *IterableGraph::findLoopEdge(const EdgeDefinition &definition) const
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

// Add edge to node map
Edge *IterableGraph::addOutputEdge(std::string_view nodeName, Edge *edge)
{
    auto &outputEdgeMap = loopBacks()->outputEdges();
    auto outputEdges = outputEdgeMap.find(nodeName);
    if (outputEdges != outputEdgeMap.end())
    {
        outputEdges->second.push_back(edge);
        return edge;
    }
    else
    {
        outputEdgeMap.insert({nodeName, {edge}});
        return edge;
    }

    return nullptr;
}

// Remove edge from node map
Edge *IterableGraph::removeOutputEdge(std::string_view nodeName, Edge *edge)
{
    auto outputEdgeMap = loopBacks()->outputEdges();
    auto outputEdges = outputEdgeMap.find(nodeName);
    if (outputEdges != outputEdgeMap.end())
    {
        auto removedEdge = std::remove(outputEdges->second.begin(), outputEdges->second.end(), edge);
        outputEdges->second.erase(removedEdge, outputEdges->second.end());
        return edge;
    }

    return nullptr;
}

/*
 * Processing & Validity
 */

// Perform processing
NodeConstants::ProcessResult IterableGraph::process()
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
