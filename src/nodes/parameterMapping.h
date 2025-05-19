// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#pragma once

#include "nodes/node.h"

// Parameter Mapping Node
class ParameterMappingNode : public Node
{
    public:
    ParameterMappingNode(Graph *parentGraph, bool outputMapper = false);
    ~ParameterMappingNode() override = default;

    /*
     * Definition
     */
    public:
    // Return type of the node
    std::string_view type() const override;
    // Return short summary of the node's purpose
    std::string_view summary() const override;

    /*
     * Data
     */
    private:
    // Whether the node maps its inputs to Graph outputs
    bool mapsOutputs_{false};

    public:
    // Return whether the node maps its inputs to Graph outputs
    bool mapsOutputs() const;

    /*
     * Processing & Validity
     */
    protected:
    // Perform processing
    NodeConstants::ProcessResult process() override;

    /*
     * I/O
     */
    public:
    // Express as a serialisable value
    SerialisedValue serialise() const override;
    // Read values from a serialisable value
    void deserialise(const SerialisedValue &node) override;
};
