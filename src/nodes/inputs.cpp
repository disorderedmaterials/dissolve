// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include "nodes/inputs.h"
#include "nodes/loop.h"

InputsNode::InputsNode(Graph *parentGraph) : Node(parentGraph) {}

/*
 * Definition (Virtuals)
 */

// Return type of the node
std::string_view InputsNode::type() const { return "Inputs"; }

// Return short summary of the node's purpose
std::string_view InputsNode::summary() const { return "Maps graph inputs to local outputs"; }

/*
 * Processing & Validity
 */

// Perform processing
NodeConstants::ProcessResult InputsNode::process() { return NodeConstants::ProcessResult::Success; }

// Run the node, retrieving dependent inputs as necessary
NodeConstants::ProcessResult InputsNode::run()
{
    auto loopGraph = dynamic_cast<LoopGraph *>(parentGraph_);

    /*
     * Only pull loop edges when we are at loop 1 or above.
     * During these iterations, this block overrides the base Node run method.
     *
     */
    if (loopGraph && (loopGraph->loopCount() > 0 && loopGraph->loopCount() <= loopGraph->nLoops()))
    {
        auto status = NodeConstants::ProcessResult::Unchanged;

        if (loopGraph->loopEdges().empty())
            return status;

        for (const auto &edge : loopGraph->loopEdges())
            status = static_cast<LoopEdge *>(edge.get())->pull();

        return status;
    }

    return Node::run();
}

/*
 * Serialisation
 */

// Is it appropriate to bother serialising this node?
bool InputsNode::shouldSerialise() const { return false; }

// Express as a serialisable value
void InputsNode::serialise(std::string tag, SerialisedValue &target) const {}

// Read values from a serialisable value
void InputsNode::deserialise(const SerialisedValue &node) {};
