// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#pragma once

#include "nodes/constants.h"
#include "nodes/parameter.h"
#include <string>

// Forward Declarations
class Graph;

// Edge Definition
struct EdgeDefinition
{
    std::string sourceNode, sourceOutput, targetNode, targetInput;
    bool operator==(const EdgeDefinition &other) const = default;
};

// Edge
class Edge
{
    public:
    ~Edge();

    protected:
    // The constructor is private because it can only be constructed by the factory method
    Edge(Node &sourceNode, ParameterBase &sourceOutput, Node &targetNode, ParameterBase &targetInput);

    private:
    // Edge definition
    EdgeDefinition definition_;
    // Store references instead of pointers to the linked nodes and parameters for two reasons:
    // 1) Neither end of the link should EVER be null
    // 2) The link itself is immutable.  You can create links and
    // destroy links, but *never mutate links*.
    //
    // The immutability is important because the factory ensures that
    // all links are valid at creation time.  By keeping the links
    // immutable, that means that all links are valid at all times.
    // This enables us to skip a great deal of error checking when
    // processing a chain of links.
    Node &sourceNode_;
    Node &targetNode_;
    ParameterBase &sourceOutput_;
    ParameterBase &targetInput_;
    // Version of the source node when this edge was last pulled by the target node.
    int sourceNodeVersionIndex_{NodeConstants::InvalidVersion};

    public:
    // A factory method to create an Edge from the supplied definition, or nullptr if it cannot
    static std::unique_ptr<Edge> create(Graph *parent, const EdgeDefinition &definition);
    // Return source node
    Node &sourceNode() const;
    // Return source output parameter
    const ParameterBase &sourceOutput() const;
    // Return target node
    Node &targetNode() const;
    // Return target input parameter
    const ParameterBase &targetInput() const;
    // Return definition for the edge
    EdgeDefinition definition() const;
    // Pull the data from the source node to the target, returning a ProcessResult
    NodeConstants::ProcessResult pull();
};
