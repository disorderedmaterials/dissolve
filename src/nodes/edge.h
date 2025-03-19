// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#pragma once

#include "nodes/parameter.h"
#include <string>

// Forward Declaraitons
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
    private:
    // Edge definition
    EdgeDefinition definition_;

    protected:
    // The constructor is private because it can only be constructed by the factory method
    Edge(Node *sourceNode, ParameterBase &sourceOutput, Node *targetNode, ParameterBase &targetInput);
    // Pointers to source and target nodes
    Node *sourceNode_{nullptr};
    Node *targetNode_{nullptr};
    // Store references instead of pointers to the linked parameters for two reasons:
    // 1) Neither end of the link should EVER be null
    // 2) The link itself is immutable.  You can create links and
    // destroy links, but *never mutate links*.
    //
    // The immutability is important because the factory ensures that
    // all links are valid at creation time.  By keeping the links
    // immutable, that means that all links are valid at all times.
    // This enables us to skip a great deal of error checking when
    // processing a chain of links.
    ParameterBase &sourceOutput_;
    ParameterBase &targetInput_;

    public:
    // A factory method to create an Edge from the supplied definition, or nullopt if it cannot
    static std::unique_ptr<Edge> create(Graph *parent, const EdgeDefinition &definition);
    // Return source node
    Node *sourceNode() const;
    // Return source output parameter
    const ParameterBase &sourceOutput() const;
    // Return target node
    Node *targetNode() const;
    // Return target input parameter
    const ParameterBase &targetInput() const;
    bool updateSource();
    // Return definition for the edge
    EdgeDefinition definition() const;
};
