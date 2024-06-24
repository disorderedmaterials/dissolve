// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "base/serialiser.h"
#include "generator/node.h"
#include "generator/sequence.h"

// Forward Declarations
class Configuration;
class LineParser;

// Generator
class Generator : public Serialisable<const CoreData &>
{
    public:
    Generator(std::string_view blockTerminationKeyword = "EndGenerator");
    ~Generator();

    /*
     * Data
     */
    private:
    // Sequence node from which the Generator starts
    GeneratorNodeSequence rootSequence_;

    public:
    // Clear all data
    void clear();
    // Create new node
    template <class N, typename... Args> std::shared_ptr<N> createRootNode(std::string_view name, Args &&...args)
    {
        return rootSequence_.create<N>(name, args...);
    }
    // Return root sequence
    GeneratorNodeSequence &rootSequence();
    // Return named node if present (and matches the type / class given)
    ConstNodeRef node(std::string_view name, const ConstNodeRef &excludeNode = {},
                      const GeneratorNode::NodeTypeVector &allowedNodeTypes = {}) const;
    // Return all nodes matching the optional type(s) given
    std::vector<ConstNodeRef> nodes(const GeneratorNode::NodeTypeVector &allowedNodeTypes = {}) const;
    // Remove a node
    bool removeNode(NodeRef node);

    /*
     * Execute
     */
    private:
    // List of Configurations and the coordinate indices at which they were last processed
    std::vector<std::pair<Configuration *, int>> configurationPoints_;

    public:
    // Run generator in the specified data context
    bool execute(const ProcedureContext &context);

    /*
     * Read / Write
     */
    public:
    // Read generator from specified LineParser
    bool deserialise(LineParser &parser, const CoreData &coreData);
    // Write generator to specified LineParser
    bool serialise(LineParser &parser, std::string_view prefix);
    // Express as a serialisable value.
    SerialisedValue serialise() const override;
    // Read values from a serialisable value
    void deserialise(const SerialisedValue &node, const CoreData &data) override;
};
