// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#pragma once

#include "nodes/node.h"

// Outputs Node
class OutputsNode : public Node
{
    public:
    OutputsNode(Graph *parentGraph);
    ~OutputsNode() override = default;

    /*
     * Definition
     */
    public:
    // Return type of the node
    std::string_view type() const override;
    // Return short summary of the node's purpose
    std::string_view summary() const override;

    /*
     * Processing & Validity
     */
    protected:
    // Perform processing
    NodeConstants::ProcessResult process() override;

    public:
    // Flag that the node data needs to be updated
    void setUpdateRequired() override;

    /*
     * Serialisation
     */
    public:
    // Is it appropriate to bother serialising this node?
    bool shouldSerialise() const override;
    // Express as a serialisable value
    void serialize(std::string tag, SerialisedValue &target) const override;
    // Read values from a serialisable value
    void deserialise(const SerialisedValue &node) override;
};
