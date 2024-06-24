// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "generator/generator.h"
#include "base/lineParser.h"
#include "base/sysFunc.h"
#include "classes/configuration.h"

Procedure::Procedure(std::string_view blockKeyword) : rootSequence_({}, blockKeyword) {}

Procedure::~Procedure() = default;

/*
 * Data
 */

// Clear all data
void Procedure::clear() { rootSequence_.clear(); }

// Return root sequence
ProcedureNodeSequence &Procedure::rootSequence() { return rootSequence_; }

// Return named node if present (and matches the type / class given)
ConstNodeRef Procedure::node(std::string_view name, const ConstNodeRef &excludeNode,
                             const ProcedureNode::NodeTypeVector &allowedNodeTypes) const
{
    return rootSequence_.node(name, excludeNode, allowedNodeTypes);
}

// Return all nodes (matching the type / class given)
std::vector<ConstNodeRef> Procedure::nodes(const ProcedureNode::NodeTypeVector &allowedNodeTypes) const
{
    return rootSequence_.nodes(allowedNodeTypes);
}

// Remove a node
bool Procedure::removeNode(NodeRef node) { return rootSequence_.removeNode(node); }

/*
 * Execute
 */

// Run procedure in the specified data context
bool Procedure::execute(const ProcedureContext &context)
{
    // Prepare the nodes
    if (!rootSequence_.prepare(context))
        return Messenger::error("Failed to prepare procedure for execution.\n");

    // Execute the root sequence
    if (!rootSequence_.execute(context))
        return Messenger::error("Failed to execute procedure.\n");

    // Finalise any nodes that need it
    if (!rootSequence_.finalise(context))
        return Messenger::error("Failed to finalise procedure after execution.\n");

    return true;
}

/*
 * Read / Write
 */

// Read structure from specified LineParser
bool Procedure::deserialise(LineParser &parser, const CoreData &coreData)
{
    clear();

    return rootSequence_.deserialise(parser, coreData);
}

// Write structure to specified LineParser
bool Procedure::serialise(LineParser &parser, std::string_view prefix) { return rootSequence_.serialise(parser, prefix); }

// Express as a serialisable value
SerialisedValue Procedure::serialise() const { return rootSequence_.serialise(); }

// Read values from a serialisable value
void Procedure::deserialise(const SerialisedValue &node, const CoreData &data) { rootSequence_.deserialise(node, data); }
