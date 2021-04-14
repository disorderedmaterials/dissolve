// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "procedure/procedure.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"
#include "classes/configuration.h"

Procedure::Procedure(ProcedureNode::NodeContext context, std::string_view blockTerminationKeyword)
    : rootSequence_(context, this, nullptr, blockTerminationKeyword)
{
    context_ = context;
}

Procedure::~Procedure() {}

/*
 * Data
 */

// Clear all data
void Procedure::clear() { rootSequence_.clear(); }

// Add (own) specified node to root sequence
void Procedure::addRootSequenceNode(ProcedureNode *node)
{
    rootSequence_.addNode(node);
    node->setScope(&rootSequence_);
}

// Return root sequence
const SequenceProcedureNode &Procedure::rootSequence() const { return rootSequence_; }

// Return the block termination keyword for the Procedure
std::string_view Procedure::blockTerminationKeyword() const { return rootSequence_.blockTerminationKeyword(); }

// Return named node if present, and which matches the (optional) type given
ProcedureNode *Procedure::node(std::string_view name, std::optional<ProcedureNode::NodeType> optNodeType) const
{
    return rootSequence_.node(name, optNodeType);
}

/*
 * Execute
 */

// Run procedure for specified Configuration, storing / retrieving generated data from supplied list
bool Procedure::execute(ProcessPool &procPool, Configuration *cfg, std::string_view prefix, GenericList &targetList)
{
    // Depending on context, we may or may not operate on the supplied Configuration
    if (context_ == ProcedureNode::AnalysisContext)
    {
        // Check that the Configuration has changed before we do any more analysis on it
        RefDataItem<Configuration, int> *ri = configurationPoints_.contains(cfg);
        if (ri)
        {
            // A Configuration we've processed before - check the index
            if (cfg->contentsVersion() == ri->data())
            {
                Messenger::warn("Refusing to analyse Configuration '{}' since it has not changed.\n", cfg->name());
                return true;
            }
            else
                ri->data() = cfg->contentsVersion();
        }
        else
            configurationPoints_.append(cfg, cfg->contentsVersion());
    }

    // Prepare the nodes
    if (!rootSequence_.prepare(cfg, prefix, targetList))
        return Messenger::error("Failed to prepare procedure for execution.\n");

    // Execute the root sequence
    if (!rootSequence_.execute(procPool, cfg, prefix, targetList))
        return Messenger::error("Failed to execute procedure.\n");

    // Finalise any nodes that need it
    if (!rootSequence_.finalise(procPool, cfg, prefix, targetList))
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
bool Procedure::write(LineParser &parser, std::string_view prefix) { return rootSequence_.write(parser, prefix); }
