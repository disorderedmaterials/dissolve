// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodes/iterableGraph.h"

IterableGraph::IterableGraph(Graph *parentGraph) : Graph(parentGraph)
{
    addOption("N", "Number of loops (iterations) to perform", nIterations_);
    loopBacks_ = dynamic_cast<LoopBacksNode *>(addNode(std::make_unique<LoopBacksNode>(this), "LoopBacks"));
}

/*
 * Definition
 */

// Return node name
std::string_view IterableGraph::name() const { return "Iterator"; }

// Return type of the node
std::string_view IterableGraph::type() const { return "Iterator"; }

// Return short summary of the node's purpose
std::string_view IterableGraph::summary() const { return "Iterate the contained graph"; }

/*
 * Data
 */

// Number of loops (iterations) to perform
int IterableGraph::nIterations() const { return nIterations_.asInteger(); }

// Return loopbacks node
LoopBacksNode *IterableGraph::loopBacks() const { return loopBacks_; }

// Return loop edges
const Graph::Edges &IterableGraph::loopEdges() const { return loopEdges_; }

// Current loop iteration
int IterableGraph::currentIteration() const { return i_; }

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

// Remove specified loop edge
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

// Add edge between nodes
bool IterableGraph::addLoopEdge(std::unique_ptr<Edge> edge, std::string_view source)
{
    loopEdges_.emplace_back(LoopEdge::makeLoopEdge(edge.release(), proxyInputs()));

    return addOutputLoopEdge(source, loopEdges_.back().get());
}

// Add edge to node map
Edge *IterableGraph::addOutputLoopEdge(std::string_view sourceOutput, Edge *edge)
{
    auto &outputEdgeMap = loopBacks()->outputEdges();
    auto outputEdges = outputEdgeMap.find(sourceOutput);

    // If source not in edge map, insert it, otherwise push new edge to current source node
    if (outputEdges != outputEdgeMap.end())
    {
        outputEdges->second.push_back(edge);
        return edge;
    }
    else
    {
        outputEdgeMap.insert({sourceOutput, {edge}});
        return edge;
    }

    return nullptr;
}

// Remove edge from node map
Edge *IterableGraph::removeOutputLoopEdge(std::string_view sourceOutput, Edge *edge)
{
    auto outputEdgeMap = loopBacks()->outputEdges();
    auto outputEdges = outputEdgeMap.find(sourceOutput);
    if (outputEdges != outputEdgeMap.end())
    {
        auto removedEdge = std::remove(outputEdges->second.begin(), outputEdges->second.end(), edge);
        outputEdges->second.erase(removedEdge, outputEdges->second.end());
        return edge;
    }

    return nullptr;
}

/*
 * Nodes and Edges
 */

// Add edge between nodes
bool IterableGraph::addEdge(const EdgeDefinition &definition)
{
    // Refresh the graph loopbacks
    setLoopBacks();

    // Check if the connection is invertible.
    // Invertibility is satisfied when the source node (internal to the graph) can output to an existing loopback,
    // which discounts any edge for which no loopbacks correspond to the target input, as well as the graphs own InputsNode.
    auto nonInvertible = dynamic_cast<InputsNode *>(parentGraph()->findNode(definition.sourceNode)) ||
                         !loopBacks_->findInput(definition.targetInput);

    // If not invertible, create and return a standard edge
    if (nonInvertible)
        return Graph::addEdge(definition);

    // Create loop edge
    auto edge =
        Edge::create(this, {definition.sourceNode, definition.sourceOutput, definition.targetNode, definition.targetInput});
    if (!edge)
        return false;

    return addLoopEdge(std::move(edge), definition.sourceOutput);
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

        removeOutputLoopEdge(definition.sourceOutput, static_cast<Edge *>(loopEdge));
    }
    return true;
}

/*
 * Processing
 */

// Perform processing
NodeConstants::ProcessResult IterableGraph::process()
{
    const auto N = nIterations_.asInteger();
    if (N < 1)
        return NodeConstants::ProcessResult::Unchanged;

    for (i_ = 1; i_ <= N; ++i_)
    {
        if ((i_ > 1 && (loopBacks_->run() == NodeConstants::ProcessResult::Failed)) ||
            (Graph::process() == NodeConstants::ProcessResult::Failed))
            return NodeConstants::ProcessResult::Failed;
    }

    return NodeConstants::ProcessResult::Success;
}

/*
 * Serialisation
 */

// Express as a serialisable value
void IterableGraph::serialise(std::string tag, SerialisedValue &target) const
{
    Graph::serialise(tag, target);
    auto &result = target[tag];
    fromVector(loopEdges_, "loopEdges", result);
}

// Read values from a serialisable value
void IterableGraph::deserialise(const SerialisedValue &node)
{
    Graph::deserialise(node);
    toVector(node, "loopEdges",
             [this](const auto &value)
             {
                 auto definition = toml::get<EdgeDefinition>(value);
                 auto edge =
                     Edge::create(this, {definition.sourceNode, definition.sourceOutput, "LoopBacks", definition.targetInput});
                 if (!edge)
                     return false;

                 return addLoopEdge(std::move(edge), definition.sourceOutput);
             });
}
