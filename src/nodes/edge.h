// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "base/serialiser.h"
#include "nodes/constants.h"
#include "nodes/parameter.h"
#include <string>

// Forward Declarations
class Graph;

// Edge Definition
class EdgeDefinition : public Serialisable
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
    void serialise(std::string tag, SerialisedValue &target) const override;
    // Read values from a serialisable value
    void deserialise(const SerialisedValue &node) override;
};

// Edge
class Edge : public Serialisable
{
    friend class LoopEdge;

    public:
    ~Edge();

    protected:
    // The constructor is private because it can only be constructed by the factory method
    Edge(Node &sourceNode, ParameterBase &sourceOutput, Node &targetNode, ParameterBase &targetInput);

    protected:
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
    // Return definition for the edge
    EdgeDefinition definition() const;
    // Pull the data from the source node to the target, returning a ProcessResult
    NodeConstants::ProcessResult pull();
    // Ensure next call to pull() will retrieve the data from the source node
    void forceNextPull();

    /*
     * Serialisation
     */
    public:
    // Express as a serialisable value
    void serialise(std::string tag, SerialisedValue &target) const override;
    // Read values from a serialisable value
    void deserialise(const SerialisedValue &node) override;
};

// Loop edge
class LoopEdge : public Edge
{
    protected:
    // The constructor is private because it can only be constructed by the factory method
    LoopEdge(Node &sourceNode, ParameterBase &sourceOutput, Node &targetNode, ParameterBase &targetInput);

    public:
    // Pull the data from the source node to the target, returning a ProcessResult
    NodeConstants::ProcessResult pull();
    // Make a loop edge from a given node output (LoopBack), which feeds back into the graph's inputs node
    static std::unique_ptr<LoopEdge> makeLoopEdge(const Edge *edge, Node &inputs);
    // Return the analogue parameter, which the loop edge source corresponds to
    ParameterBase &analogue();

    private:
    /*
     * The analogue parameter, which the loop edge source corresponds to.
     *
     * Since the LoopEdge and normal Edge are different types, we must identify
     * the analogous Edge parameter corresponding the source of the feedback
     * from the perspective of the "looping" node, for instance a mathematical entity that
     * requires a recursively compounded value.
     *
     * This source output will be assigned to the analogue parameter, so that edges that pull from it
     * recieve the feedback.
     *
     */
    ParameterBase *analogue_;

    /*
     * Serialisation
     */
    public:
    // Express as a serialisable value
    void serialise(std::string tag, SerialisedValue &target) const override;
    // Read values from a serialisable value
    void deserialise(const SerialisedValue &node) override;
};