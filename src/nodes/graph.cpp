// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include "nodes/graph.h"
#include "nodes/edge.h"
#include "nodes/inputs.h"
#include "nodes/outputs.h"
#include "nodes/registry.h"

Graph::Graph(Graph *parentGraph) : Node(parentGraph)
{
    mappedInputs_ = dynamic_cast<InputsNode *>(addNode(std::make_unique<InputsNode>(this), "Inputs"));
    mappedOutputs_ = dynamic_cast<OutputsNode *>(addNode(std::make_unique<OutputsNode>(this), "Outputs"));
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
    auto outputsResult = mappedOutputs_->run();
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

    // Propagate changes through mappedInputs_
    mappedInputs_->setUpdateRequired();

    // Call base class function to set flag and propagate through outputs
    Node::setUpdateRequired();
}

/*
 * Inputs, Outputs, and Options
 */

// Create and return mapped input
std::shared_ptr<ParameterBase> Graph::createMappedInput(std::string_view inputName, std::type_index typeIndex)
{
    std::shared_ptr<ParameterBase> inputParameter;

    // Check first that the inputName doesn't exist in our inputs_ (implicitly this carries for InputNode's outputs_)
    if (findInput(inputName))
    {
        Messenger::error("Can't create mapped input as one named '{}' already exists in the Graph.", inputName);
        return {};
    }

    // TODO Convert to Factory
    if (typeIndex == std::type_index(typeid(Number)))
    {
        // Create a parameter holder object with the correct type
        auto proxy = std::make_shared<ParameterHolder<Number>>();
        parameterHolders_.emplace_back(proxy);

        // Create an input on ourself, linked to the proxy data
        inputParameter = addInput(inputName, "", proxy->data);

        // Create a companion output on our Inputs node, again linked to the proxy data
        mappedInputs_->addOutput(inputName, "", proxy->data);
    }

    return inputParameter;
}

// Create mapped output, returning the relevant input (rather than the output)
std::shared_ptr<ParameterBase> Graph::createMappedOutput(std::string_view outputName, std::type_index typeIndex)
{
    // Check first that the inputName doesn't exist in our outputs_ (implicitly this carries for OutputNode's inputs_)
    if (findOutput(outputName))
    {
        Messenger::error("Can't create mapped output as one named '{}' already exists in the Graph.", outputName);
        return {};
    }

    std::shared_ptr<ParameterBase> outputParameter;
    // TODO Convert to Factory
    if (typeIndex == std::type_index(typeid(Number)))
    {
        // Create n parameter holder object with the correct type
        auto proxy = std::make_shared<ParameterHolder<Number>>();
        parameterHolders_.emplace_back(proxy);

        // Create an output on ourself, linked to the proxy data
        addOutput(outputName, "", proxy->data);

        // Create a companion input on our Outputs node, again linked to the proxy data
        outputParameter = mappedOutputs_->addInput(outputName, "", proxy->data);
    }

    return outputParameter;
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
Node *Graph::createNode(std::string_view nodeType, std::string_view nodeName, bool strictTypeName)
{
    std::string newNodeType{nodeType};
    if (strictTypeName)
    {
        if (!NodeRegistry::hasNodeType(nodeType))
        {
            Messenger::warn("Can't create a node of type '{}' as this type does not exist.\n", nodeType);
            return nullptr;
        }
    }
    else
    {
        newNodeType = NodeRegistry::getNodeTypeFuzzy(nodeType);
        if (newNodeType.empty())
        {
            Messenger::warn("Can't create a node of type '{}' as neither this type nor a close match exists.\n", nodeType);
            return nullptr;
        }
    }

    return addNode(NodeRegistry::produce(this, newNodeType), nodeName.empty() ? newNodeType : nodeName);
}

// Add node to graph
Node *Graph::addNode(std::unique_ptr<Node> node, std::string_view newName)
{
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
Node *Graph::node(std::string_view nodeName)
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

/*
 * I/O
 */

// Express as a serialisable value
SerialisedValue Graph::serialise() const
{
    SerialisedValue result = Node::serialise();
    fromMap(nodes_, "nodes", result);
    fromVector(edges_, "edges", result);
    return result;
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
