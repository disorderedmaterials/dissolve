// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "procedure/nodes/runModule.h"
#include "keywords/module.h"
#include "module/context.h"
#include "procedure/nodes/node.h"

RunModuleNode::RunModuleNode(Module *module)
    : ProcedureNode(ProcedureNode::NodeType::RunModule, {ProcedureNode::ControlContext}), module_(module)
{
    keywords_.setOrganisation("Options", "Target");
    keywords_.add<ModuleKeyword<Module>>("Module", "Target module to run", module_, ModuleTypes::ModuleType::Any);
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
bool RunModuleNode::prepare(const ProcedureContext &procedureContext)
{
    if (!module_)
        return false;
    ModuleContext moduleContext(procedureContext.processPool(), procedureContext.dissolve());
    return module_->setUp(moduleContext);
}

// Execute node
bool RunModuleNode::execute(const ProcedureContext &procedureContext)
{
    ModuleContext moduleContext(procedureContext.processPool(), procedureContext.dissolve());
    if (module_->executeProcessing(moduleContext) == Module::ExecutionResult::Failed)
        return Messenger::error("Module '{}' experienced problems. Exiting now.\n", module_->name());
}