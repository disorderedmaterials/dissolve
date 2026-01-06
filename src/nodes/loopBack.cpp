// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodes/loopGraph.h"
#include "nodes/outputs.h"

LoopBacksNode::LoopBacksNode(Graph *parentGraph) : Node(parentGraph) {}

/*
 * Definition (Virtuals)
 */

// Return type of the node
std::string_view LoopBacksNode::type() const { return "LoopBacks"; }

// Return short summary of the node's purpose
std::string_view LoopBacksNode::summary() const { return "Maps feedback from internal node outputs to graph inputs"; }

/*
 * Processing & Validity
 */

// Perform processing
NodeConstants::ProcessResult LoopBacksNode::process() { return NodeConstants::ProcessResult::Success; }

// Run the node, retrieving dependent inputs as necessary
NodeConstants::ProcessResult LoopBacksNode::run()
{
    auto status = NodeConstants::ProcessResult::Unchanged;

    auto loopGraph = static_cast<LoopGraph *>(parentGraph_);

    if (loopGraph->loopEdges().empty())
        return status;

    for (const auto &edge : loopGraph->loopEdges())
    {
        status = static_cast<LoopEdge *>(edge.get())->pull();
        if (status != NodeConstants::ProcessResult::Success)
            return status;
    }

    if (status != NodeConstants::ProcessResult::Failed)
        ++versionIndex_;

    return status;
}

// Flag that the node data needs to be updated
void LoopBacksNode::setUpdateRequired()
{
    // If already flagged then do nothing
    if (!isUpToDate())
        return;

    // Need to flag parent graph since we represent its outputs
    parentGraph_->setUpdateRequired();

    // Call base class function to set flag and propagate through outputs (not there there should be any)
    Node::setUpdateRequired();
}

/*
 * Serialisation
 */

// Is it appropriate to bother serialising this node?
bool LoopBacksNode::shouldSerialise() const { return false; }

// Express as a serialisable value
void LoopBacksNode::serialise(std::string tag, SerialisedValue &target) const {}

// Read values from a serialisable value
void LoopBacksNode::deserialise(const SerialisedValue &node) {};
