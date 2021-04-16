// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "procedure/nodes/node.h"
#include "procedure/nodes/sequence.h"
#include "templates/refdatalist.h"

// Forward Declarations
class Configuration;
class LineParser;

// Procedure
class Procedure
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
    SequenceProcedureNode rootSequence_;

    public:
    // Clear all data
    void clear();
    // Add (own) specified node to root sequence
    void addRootSequenceNode(ProcedureNode *node);
    // Return root sequence
    const SequenceProcedureNode &rootSequence() const;
    // Return the block termination keyword for the Procedure
    std::string_view blockTerminationKeyword() const;
    // Return named node if present, and which matches the (optional) type given
    ProcedureNode *node(std::string_view name, std::optional<ProcedureNode::NodeType> optNodeType = std::nullopt) const;

    /*
     * Execute
     */
    private:
    // List of Configurations and the coordinate indices at which they were last processed
    RefDataList<Configuration, int> configurationPoints_;

    public:
    // Run procedure on specified Configuration, storing / retrieving generated data from supplied list
    bool execute(ProcessPool &procPool, Configuration *cfg, std::string_view prefix, GenericList &targetList);

    /*
     * Read / Write
     */
    public:
    // Read procedure from specified LineParser
    bool deserialise(LineParser &parser, const CoreData &coreData);
    // Write procedure to specified LineParser
    bool write(LineParser &parser, std::string_view prefix);
};
