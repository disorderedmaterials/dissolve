// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include "nodes/loop.h"

LoopGraph::LoopGraph(Graph *parentGraph) : Graph(parentGraph)
{
    addOption<Number>("NLoops", "Number of loops (iterations) to perform", nLoops_);
    loopBacks_ = dynamic_cast<OutputsNode *>(addNode(std::make_unique<OutputsNode>(this), "LoopBacks"));
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

// Increment the loop counter
void LoopGraph::increment() { loopCounter_++; }

// Number of loops (iterations) to perform
const int LoopGraph::nLoops() const { return nLoops_.asInteger(); }

// Loop backs
OutputsNode *LoopGraph::loopBacks() { return loopBacks_; }

// Loop edges
Graph::Edges &LoopGraph::loopEdges() { return loopEdges_; }

// Current loop iteration
int LoopGraph::loopCount() { return loopCounter_; }

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

// Unlink edge, releasing the loop back if one accompanies it
void LoopGraph::unlinkEdge(Edge *edge)
{
    Node::unlinkEdge(edge);
    std::string release(edge->targetInput().name());
    releaseLoopBack(release);
}

// Reset the loop counter to zero
void LoopGraph::resetLoopCounter() { loopCounter_ = 0; }

/*
 * Processing & Validity
 */

// Perform processing
NodeConstants::ProcessResult LoopGraph::process()
{
    while (loopCounter_ <= nLoops_.asInteger())
    {
        auto graphStatus = NodeConstants::ProcessResult::Unchanged;

        // Don't run loopbacks on 0th iteration
        if (loopCounter_ >= 1)
        {
            auto looped = loopBacks_->run();
            if (looped == NodeConstants::ProcessResult::Failed)
                return looped;
        }
        proxyOutputs().setUpdateRequired();
        graphStatus = Graph::process();

        if (graphStatus == NodeConstants::ProcessResult::Failed)
            return graphStatus;

        increment();
    }

    resetLoopCounter();

    return Graph::process();
}
