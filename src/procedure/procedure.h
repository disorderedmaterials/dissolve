// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "base/serialiser.h"
#include "procedure/nodes/node.h"
#include "procedure/nodes/sequence.h"

// Forward Declarations
class Configuration;
class LineParser;

// Procedure
class Procedure : public Serialisable<const CoreData &>
{
    public:
    Procedure(ProcedureNode::NodeContext context, std::string_view blockTerminationKeyword = "EndProcedure");
    ~Procedure();

    /*
     * Data
     */
    private:
    // Context for the main Procedure
    ProcedureNode::NodeContext context_;
    // Sequence node from which the Procedure starts
    ProcedureNodeSequence rootSequence_;

    public:
    // Clear all data
    void clear();
    // Create new node
    template <class N, typename... Args> std::shared_ptr<N> createRootNode(std::string_view name, Args &&...args)
    {
        return rootSequence_.create<N>(name, args...);
    }
    // Return context for the main Procedure
    ProcedureNode::NodeContext context();
    // Return root sequence
    ProcedureNodeSequence &rootSequence();
    // Return named node if present (and matches the type / class given)
    ConstNodeRef node(std::string_view name, ConstNodeRef excludeNode = nullptr,
                      std::optional<ProcedureNode::NodeType> optNodeType = std::nullopt,
                      std::optional<ProcedureNode::NodeClass> optNodeClass = std::nullopt) const;
    // Return all nodes (matching the type / class given)
    std::vector<ConstNodeRef> nodes(std::optional<ProcedureNode::NodeType> optNodeType = std::nullopt,
                                    std::optional<ProcedureNode::NodeClass> optNodeClass = std::nullopt) const;

    // Remove a node
    bool removeNode(NodeRef node);

    /*
     * Execute
     */
    private:
    // List of Configurations and the coordinate indices at which they were last processed
    std::vector<std::pair<Configuration *, int>> configurationPoints_;

    public:
    // Run procedure in the specified data context
    bool execute(const ProcedureContext &context);

    /*
     * Read / Write
     */
    public:
    // Read procedure from specified LineParser
    bool deserialise(LineParser &parser, const CoreData &coreData);
    // Write procedure to specified LineParser
    bool serialise(LineParser &parser, std::string_view prefix);
    // Express as a serialisable value.
    SerialisedValue serialise() const override;
    // Read values from a serialisable value
    void deserialise(const SerialisedValue &node, const CoreData &data) override;
};
