// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "generator/generator.h"
#include "base/lineParser.h"
#include "base/sysFunc.h"
#include "classes/configuration.h"

Generator::Generator(std::string_view blockKeyword) : rootSequence_({}, blockKeyword) {}

Generator::~Generator() = default;

/*
 * Data
 */

// Clear all data
void Generator::clear() { rootSequence_.clear(); }

// Return root sequence
GeneratorNodeSequence &Generator::rootSequence() { return rootSequence_; }

// Return named node if present (and matches the type / class given)
ConstNodeRef Generator::node(std::string_view name, const ConstNodeRef &excludeNode,
                             const GeneratorNode::NodeTypeVector &allowedNodeTypes) const
{
    return rootSequence_.node(name, excludeNode, allowedNodeTypes);
}

// Return all nodes (matching the type / class given)
std::vector<ConstNodeRef> Generator::nodes(const GeneratorNode::NodeTypeVector &allowedNodeTypes) const
{
    return rootSequence_.nodes(allowedNodeTypes);
}

// Remove a node
bool Generator::removeNode(NodeRef node) { return rootSequence_.removeNode(node); }

/*
 * Execute
 */

// Run generator in the specified data context
bool Generator::execute(const ProcedureContext &context)
{
    // Prepare the nodes
    if (!rootSequence_.prepare(context))
        return Messenger::error("Failed to prepare generator for execution.\n");

    // Execute the root sequence
    if (!rootSequence_.execute(context))
        return Messenger::error("Failed to execute generator.\n");

    // Finalise any nodes that need it
    if (!rootSequence_.finalise(context))
        return Messenger::error("Failed to finalise generator after execution.\n");

    return true;
}

/*
 * Read / Write
 */

// Read structure from specified LineParser
bool Generator::deserialise(LineParser &parser, const CoreData &coreData)
{
    clear();

    return rootSequence_.deserialise(parser, coreData);
}

// Write structure to specified LineParser
bool Generator::serialise(LineParser &parser, std::string_view prefix) { return rootSequence_.serialise(parser, prefix); }

// Express as a serialisable value
SerialisedValue Generator::serialise() const { return rootSequence_.serialise(); }

// Read values from a serialisable value
void Generator::deserialise(const SerialisedValue &node, const CoreData &data) { rootSequence_.deserialise(node, data); }
