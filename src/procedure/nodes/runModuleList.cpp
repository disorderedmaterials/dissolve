// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "procedure/nodes/runModuleList.h"
#include "keywords/moduleVector.h"

RunModuleListNode::RunModuleListNode(std::vector<const Module *>modules) : ProcedureNode(ProcedureNode::NodeType::RunModuleList, {ProcedureNode::ControlContext}), modules_(modules)
{
    keywords_.setOrganisation("Options", "Target");
    keywords_.add<ModuleVectorKeyword>("Module", "Target modules to run", modules_);
}

/*
 * Identity
 */

// Return whether a name for the node must be provided
bool RunModuleListNode::mustBeNamed() const { return false; }

/*
 * Execute
 */

// Prepare any necessary data, ready for execution
bool RunModuleListNode::prepare(const ProcedureContext &procedureContext) { return true; }

// Execute node
bool RunModuleListNode::execute(const ProcedureContext &procedureContext) { return true; }
