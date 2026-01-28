// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodes/graph.h"
#include "nodes/edge.h"
#include "nodes/inputs.h"
#include "nodes/outputs.h"
#include "nodes/registry.h"
#include "species.h"

Graph::Graph(Graph *parentGraph) : Node(parentGraph)
{
    proxyInputs_ = dynamic_cast<InputsNode *>(addNode(std::make_unique<InputsNode>(this), "Inputs"));
    proxyOutputs_ = dynamic_cast<OutputsNode *>(addNode(std::make_unique<OutputsNode>(this), "Outputs"));
}

/*
 * Definition
 */

// Return type of the node
std::string_view Graph::type() const { return "Graph"; }

// Return short summary of the node's purpose
std::string_view Graph::summary() const { return "A node which contains its own inner graph"; }

/*
 * Processing & Validity
 */

// Perform processing
NodeConstants::ProcessResult Graph::process()
{
    /*
     * Processing a Graph involves running any child nodes we have, but we can only detect the nodes that need to be run in
     * one of two ways. Either 1) We cycle over Edge connections to inputs on our Outputs node and pull() those in, or 2) we
     * look for any nodes that don't have any edge connections to their Outputs and try to run() them one at a time. The
     * latter case is important if a Graph has no defined Outputs, and so no external dependence on running the child nodes.
     */

    // Pull outputs first
    auto outputsResult = proxyOutputs_->run();
    if (outputsResult == NodeConstants::ProcessResult::Failed)
        return outputsResult;

    // Check each node for output edges - any that have zero output edges need to be run()
    auto terminalNodeResult = NodeConstants::ProcessResult::Unchanged;
    for (auto &&[nodeName, node] : nodes_)
        if (!node->outputEdges().empty())
        {
            switch (node->run())
            {
                case (NodeConstants::ProcessResult::Failed):
                    return NodeConstants::ProcessResult::Failed;
                case (NodeConstants::ProcessResult::Success):
                    terminalNodeResult = NodeConstants::ProcessResult::Success;
                    break;
                case (NodeConstants::ProcessResult::Unchanged):
                    break;
                case (NodeConstants::ProcessResult::InputsNotSatisfied):
                    /* This should never happen? */
                    break;
            }
        }

    return outputsResult == terminalNodeResult ? outputsResult : NodeConstants::ProcessResult::Success;
}

// Flag that the node data needs to be updated
void Graph::setUpdateRequired()
{
    // If already flagged then do nothing
    if (!isUpToDate())
        return;

    // Propagate changes through proxyInputs_
    proxyInputs_->setUpdateRequired();

    // Call base class function to set flag and propagate through outputs
    Node::setUpdateRequired();
}

/*
 * Inputs, Outputs, and Options
 */

InputsNode &Graph::proxyInputs() { return *proxyInputs_; }

OutputsNode &Graph::proxyOutputs() { return *proxyOutputs_; }

// Add supplied proxy input, setting ownership of the parameters appropriately
bool Graph::addProxyInput(std::shared_ptr<ParameterBase> &input, std::shared_ptr<ParameterBase> &output)
{
    // We (the Graph) own the input and the proxyInputs_ owns the output
    return ownParameter(input) && proxyInputs_->ownParameter(output, true);
}

// Add supplied proxy output, setting ownership of the parameters appropriately
bool Graph::addProxyOutput(std::shared_ptr<ParameterBase> &input, std::shared_ptr<ParameterBase> &output)
{
    // We (the Graph) own the output and the proxyOutputs_ owns the input
    return proxyOutputs_->ownParameter(input) && ownParameter(output, true);
}

/*
 * Nodes and Edges
 */

// Get unique node name
std::string Graph::uniqueNodeName(const Node *node, std::string_view baseName) const
{
    auto newName = std::string(baseName);

    // Check for existing node with this name and suffix until we get a unique key
    auto count = 1;
    while (nodes_.contains(newName) && nodes_.at(newName).get() != node)
        newName = std::format("{}{:02d}", baseName, count++);

    return newName;
}

// Create node of specified type with the name provided
Node *Graph::createNode(std::string_view nodeType, std::string_view nodeName)
{
    std::string newNodeType{nodeType};

    if (!NodeRegistry::hasNodeType(nodeType))
    {
        Messenger::warn("Can't create a node of type '{}' as this type does not exist.\n", nodeType);
        return nullptr;
    }

    return addNode(NodeRegistry::produce(this, newNodeType), nodeName.empty() ? newNodeType : nodeName);
}

// Add node to graph
Node *Graph::addNode(std::unique_ptr<Node> node, std::string_view newName)
{
    node->setParent(this);
    auto nodePtr = node.get();

    auto uniqueName = uniqueNodeName(node.get(), newName.empty() ? node->type() : newName);
    reverseNodes_.insert(std::make_pair<Node *, std::string>(node.get(), std::string(uniqueName)));
    nodes_.insert(std::make_pair<std::string, std::unique_ptr<Node>>(std::string(uniqueName), std::move(node)));

    return nodePtr;
}

// Get name of specified child node
std::string_view Graph::nodeName(const Node *node) const
{
    if (reverseNodes_.contains(node))
        return reverseNodes_.at(node);

    return "UNKNOWN_NODE";
}

// Set name of specified child node
void Graph::setNodeName(const Node *node, std::string_view nodeName)
{
    auto uniqueName = uniqueNodeName(node, nodeName);

    // Extract the forward node mapping (name -> node) using its current name in reverseNodes_
    auto nodeHandle = nodes_.extract(reverseNodes_.at(node));

    // Set the new name and reinsert
    nodeHandle.key() = uniqueName;
    nodes_.insert(std::move(nodeHandle));

    // Update reverseNodes_
    reverseNodes_[node] = uniqueName;
}

// Add parameter link between nodes
bool Graph::addEdge(const EdgeDefinition &definition)
{
    auto edge = Edge::create(this, definition);
    if (!edge)
        return false;

    edges_.emplace_back(std::move(edge));

    return true;
}

// Remove edge between nodes
bool Graph::removeEdge(const EdgeDefinition &definition)
{
    auto edge = findEdge(definition);
    if (!edge)
        return Messenger::error("Edge doesn't exist, so can't remove it.\n");
    else
        return removeEdge(edge);
}
bool Graph::removeEdge(Edge *edgeToRemove)
{
    auto it =
        std::find_if(edges_.begin(), edges_.end(), [edgeToRemove](const auto &edge) { return edge.get() == edgeToRemove; });
    if (it == edges_.end())
        return Messenger::error("Edge pointer doesn't exist, so can't remove it.\n");

    // Need to flag the node containing the connected input that it is now out-of-date
    auto &input = it->get()->targetInput();
    input.setParentUpdateRequired();
    if (input.isVector())
        input.invalidateVector();

    // Can now erase it
    edges_.erase(it);

    return true;
}

// Find edge between nodes
Edge *Graph::findEdge(const EdgeDefinition &definition) const
{
    auto it =
        std::find_if(edges_.begin(), edges_.end(), [definition](const auto &edge) { return edge->definition() == definition; });

    if (it != edges_.end())
        return it->get();

    return {};
}

// Return named node, if it exists
Node *Graph::findNode(std::string_view nodeName)
{
    // Return ourself if this is our name
    if (name() == nodeName)
        return this;

    // Search through child nodes
    if (nodes_.contains(std::string(nodeName)))
        return nodes_[std::string(nodeName)].get();

    return nullptr;
}

// Return container of nodes
Graph::Nodes &Graph::nodes() { return nodes_; }

// Return edges on the graph
Graph::Edges &Graph::edges() { return edges_; }

// Return a path to this graph from the root
std::string Graph::location() const
{
    if (!parentGraph_)
        return std::string("/") + std::string(name());
    return parentGraph_->location() + "/" + std::string(name());
}

/*
 * Serialisation
 */

// Express as a serialisable value
void Graph::serialise(std::string tag, SerialisedValue &target) const
{
    Node::serialise(tag, target);
    auto &result = target[tag];
    fromMap(nodes_, "nodes", result, [](const auto key, const auto &value) { return value->shouldSerialise(); });
    fromVector(edges_, "edges", result);
}

// Read values from a serialisable value
void Graph::deserialise(const SerialisedValue &node)
{
    Node::deserialise(node);
    toMap(node, "nodes",
          [this](const auto name, const auto &value)
          {
              std::string nodeType = toml::find<std::string>(value, "type");
              auto child = createNode(nodeType, name);

              child->deserialise(value);
          });
    toVector(node, "edges", [this](const auto &value) { addEdge(toml::get<EdgeDefinition>(value)); });
}
