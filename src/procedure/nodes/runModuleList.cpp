// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "procedure/nodes/runModuleList.h"
#include "keywords/moduleVector.h"
#include "module/context.h"
#include "keywords/integer.h"
#include "templates/flags.h"
#include "main/dissolve.h"

RunModuleListNode::RunModuleListNode(std::vector<Module *> modules, int frequency) : ProcedureNode(ProcedureNode::NodeType::RunModuleList, {ProcedureNode::ControlContext}), modules_(std::move(modules)), frequency_(frequency)
{
    keywords_.setOrganisation("Options", "Targets");
    keywords_.add<ModuleVectorKeyword>("Modules", "Target modules to run", modules_);
    keywords_.add<IntegerKeyword>("Frequency", "Frequency to run modules", frequency_);
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
    return true;
}

// Execute node
bool RunModuleListNode::execute(const ProcedureContext &procedureContext)
{
    return true;
    if (procedureContext.dissolve().iteration() % frequency_ == 0)
    {
        for (auto &module : modules_)
        {
            if (!module->runThisIteration(procedureContext.dissolve().iteration() / frequency_))
                continue;
            ModuleContext moduleContext({procedureContext.processPool(), procedureContext.dissolve()});
            if (module->executeProcessing(moduleContext) == Module::ExecutionResult::Failed)
                return Messenger::error("Module '{}' experienced problems. Exiting now.\n", module->name());
        }
    }
}