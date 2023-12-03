// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "procedure/nodes/runModule.h"
#include "keywords/module.h"

RunModuleNode::RunModuleNode(const Module *module) : ProcedureNode(ProcedureNode::NodeType::RunModule, {ProcedureNode::ControlContext}), module_(module)
{
    keywords_.setOrganisation("Options", "Target");
    keywords_.add<ModuleKeyword>("Module", "Target module to run", module_);
}

/*
 * Identity
 */

// Return whether a name for the node must be provided
bool RunModuleNode::mustBeNamed() const { return false; }

/*
 * Execute
 */

// Prepare any necessary data, ready for execution
bool RunModuleNode::prepare(const ProcedureContext &procedureContext) { return true; }

// Execute node
bool RunModuleNode::execute(const ProcedureContext &procedureContext) { return true; }
