// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "procedure/nodes/runModuleList.h"
#include "keywords/moduleVector.h"
#include "module/context.h"

RunModuleListNode::RunModuleListNode(std::vector<Module *>modules) : ProcedureNode(ProcedureNode::NodeType::RunModuleList, {ProcedureNode::ControlContext}), modules_(modules)
{
    keywords_.setOrganisation("Options", "Target");
    keywords_.add<ModuleVectorKeyword>("Modules", "Target modules to run", modules_);
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
bool RunModuleListNode::prepare(const ProcedureContext &procedureContext)
{
    ModuleContext moduleContext(procedureContext.processPool(), procedureContext.dissolve());

    for (auto& module : modules_)
    {
        if (!module->setUp(moduleContext))
            return false;
    }
    return true;
}

// Execute node
bool RunModuleListNode::execute(const ProcedureContext &procedureContext)
{
    ModuleContext moduleContext(procedureContext.processPool(), procedureContext.dissolve());
    for (auto& module : modules_)
    {
        if (module->executeProcessing({procedureContext.processPool(), procedureContext.dissolve()}) == Module::ExecutionResult::Failed)
            return Messenger::error("Module '{}' experienced problems. Exiting now.\n", module->name());
    }
    return true;
}
