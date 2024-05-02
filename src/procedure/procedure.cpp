// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "procedure/procedure.h"
#include "base/lineParser.h"
#include "base/sysFunc.h"
#include "classes/configuration.h"

Procedure::Procedure(ProcedureNode::NodeContext context, std::string_view blockKeyword)
    : rootSequence_(context, {}, blockKeyword)
{
    context_ = context;
}

Procedure::~Procedure() = default;

/*
 * Data
 */

// Clear all data
void Procedure::clear() { rootSequence_.clear(); }

// Return context for the main Procedure
ProcedureNode::NodeContext Procedure::context() { return context_; }

// Return root sequence
ProcedureNodeSequence &Procedure::rootSequence() { return rootSequence_; }

// Return named node if present (and matches the type / class given)
ConstNodeRef Procedure::node(std::string_view name, ConstNodeRef excludeNode,
                             std::optional<ProcedureNode::NodeType> optNodeType,
                             std::optional<ProcedureNode::NodeClass> optNodeClass) const
{
    return rootSequence_.node(name, std::move(excludeNode), optNodeType, optNodeClass);
}

// Return all nodes (matching the type / class given)
std::vector<ConstNodeRef> Procedure::nodes(std::optional<ProcedureNode::NodeType> optNodeType,
                                           std::optional<ProcedureNode::NodeClass> optNodeClass) const
{
    return rootSequence_.nodes(optNodeType, optNodeClass);
}

// Remove a node
bool Procedure::removeNode(NodeRef node) { return rootSequence_.removeNode(node); }

/*
 * Execute
 */

// Run procedure in the specified data context
bool Procedure::execute(const ProcedureContext &context)
{
    // Depending on context, we may or may not operate on the supplied Configuration
    if (context_ == ProcedureNode::AnalysisContext)
    {
        auto *cfg = context.configuration();

        // Check that the Configuration has changed before we do any more analysis on it
        auto ri = std::find_if(configurationPoints_.begin(), configurationPoints_.end(),
                               [cfg](const auto pair) { return pair.first == cfg; });
        if (ri != configurationPoints_.end())
        {
            // A Configuration we've processed before - check the index
            if (cfg->contentsVersion() == ri->second)
            {
                Messenger::warn("Refusing to analyse Configuration '{}' since it has not changed.\n", cfg->name());
                return true;
            }
            else
                ri->second = cfg->contentsVersion();
        }
        else
            configurationPoints_.emplace_back(cfg, cfg->contentsVersion());
    }

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
