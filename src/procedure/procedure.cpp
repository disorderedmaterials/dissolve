// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#include "procedure/procedure.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"
#include "classes/configuration.h"

Procedure::Procedure(ProcedureNode::NodeContext context, std::string_view blockKeyword)
    : rootSequence_(std::make_shared<SequenceProcedureNode>(context, this, nullptr, blockKeyword))
{
    context_ = context;
}

Procedure::~Procedure() = default;

/*
 * Data
 */

// Clear all data
void Procedure::clear() { rootSequence_->clear(); }

// Return root sequence
const SequenceProcedureNode &Procedure::rootSequence() const { return *rootSequence_; }

// Return named node if present (and matches the type / class given)
ConstNodeRef Procedure::node(std::string_view name, std::optional<ProcedureNode::NodeType> optNodeType,
                             std::optional<ProcedureNode::NodeClass> optNodeClass) const
{
    return rootSequence_->node(name, optNodeType, optNodeClass);
}

// Return all nodes (matching the type / class given)
std::vector<ConstNodeRef> Procedure::nodes(std::optional<ProcedureNode::NodeType> optNodeType,
                                           std::optional<ProcedureNode::NodeClass> optNodeClass) const
{
    return rootSequence_->nodes(optNodeType, optNodeClass);
}

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
    if (!rootSequence_->prepare(context))
        return Messenger::error("Failed to prepare procedure for execution.\n");

    // Execute the root sequence
    if (!rootSequence_->execute(context))
        return Messenger::error("Failed to execute procedure.\n");

    // Finalise any nodes that need it
    if (!rootSequence_->finalise(context))
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

    return rootSequence_->deserialise(parser, coreData);
}

// Write structure to specified LineParser
bool Procedure::write(LineParser &parser, std::string_view prefix) { return rootSequence_->write(parser, prefix); }
