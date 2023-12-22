// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "procedure/nodes/generate.h"
#include "keywords/configuration.h"

GenerateProcedureNode::GenerateProcedureNode(Configuration *cfg) : ProcedureNode(ProcedureNode::NodeType::Generate, {ProcedureNode::ControlContext}), target_(cfg)
{
    keywords_.setOrganisation("Options", "Configuration");
    keywords_.add<ConfigurationKeyword>("Target", "Target configuration to generate", target_);
}

/*
 * Identity
 */

// Return whether a name for the node must be provided
bool GenerateProcedureNode::mustBeNamed() const { return false; }

/*
 * Execute
 */

// Prepare any necessary data, ready for execution
bool GenerateProcedureNode::prepare(const ProcedureContext &procedureContext)
{
    // Check for valid target configuration
    if (!target_)
        return Messenger::error("No target configuration set in node '{}.\n", name());

    return true;
}

// Execute node
bool GenerateProcedureNode::execute(const ProcedureContext &procedureContext)
{
    return true;
}