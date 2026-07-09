// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "nodes/node.h"

// LoopBacks Node
class LoopBacksNode : public Node
{
    public:
    LoopBacksNode(Graph *parentGraph);
    ~LoopBacksNode() override = default;

    /*
     * Definition
     */
    public:
    // Return type of the node
    std::string_view type() const override;
    // Return short summary of the node's purpose
    std::string_view summary() const override;

    /*
     * Processing
     */
    protected:
    // Perform processing
    NodeConstants::ProcessResult process() override;

    public:
    // Run the node, retrieving dependent inputs as necessary
    NodeConstants::ProcessResult run() override;

    // Get the outgoing edges from this node
    Node::EdgeMap &outputEdges() override;

    private:
    Node::EdgeMap loopEdges_;

    public:
    // Flag that the node data needs to be updated
    void setUpdateRequired() override;

    /*
     * Serialisation
     */
    public:
    // Is it appropriate to bother serialising this node?
    bool shouldSerialise() const;
    // Express as a serialisable value
    void serialise(std::string tag, SerialisedValue &target) const;
    // Read values from a serialisable value
    void deserialise(const SerialisedValue &node);
};
