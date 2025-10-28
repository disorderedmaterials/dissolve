// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#pragma once

#include "base/serialiser.h"
#include "nodes/constants.h"
#include "nodes/parameter.h"
#include <string>

// Forward Declarations
class Graph;

// Edge Definition
class EdgeDefinition : public Serialisable<>
{
    public:
    EdgeDefinition() = default;
    EdgeDefinition(std::string srcNode, std::string srcOutput, std::string tgtNode, std::string tgtInput);
    EdgeDefinition(const EdgeDefinition &other);
    std::string sourceNode, sourceOutput, targetNode, targetInput;
    bool operator==(const EdgeDefinition &other) const;
    // Return as a string
    std::string asString() const;
    // Express as a serialisable value
    SerialisedValue serialise() const override;
    // Read values from a serialisable value
    void deserialise(const SerialisedValue &node) override;
};

// Edge
class Edge : public Serialisable<>
{
    public:
    ~Edge();

    protected:
    // The constructor is private because it can only be constructed by the factory method
    Edge(Node &sourceNode, ParameterBase &sourceOutput, Node &targetNode, ParameterBase &targetInput);

    private:
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
    ParameterBase &targetInput() const;
    // Return version of the source node when this edge was last pulled by the target node.
    int sourceNodeVersionIndex() const;
    // Return definition for the edge
    EdgeDefinition definition() const;
    // Return whether the edge links to updated data and requires a pull
    bool requiresPull() const;
    // Pull the data from the source node to the target, returning a ProcessResult
    NodeConstants::ProcessResult pull();
    // Ensure next call to pull() will retrieve the data from the source node
    void forceNextPull();

    /*
     * Serialisation
     */
    public:
    // Express as a serialisable value
    SerialisedValue serialise() const override;
    // Read values from a serialisable value
    void deserialise(const SerialisedValue &node) override;
};
