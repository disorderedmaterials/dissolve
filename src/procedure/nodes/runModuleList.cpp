// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "procedure/nodes/runModuleList.h"
#include "keywords/bool.h"
#include "keywords/integer.h"
#include "keywords/moduleVector.h"
#include "main/dissolve.h"
#include "module/context.h"
#include "templates/flags.h"

RunModuleListNode::RunModuleListNode(std::vector<Module *> modules, int frequency, const Flags<ModuleLayer::RunControlFlag> &flags)
    : ProcedureNode(ProcedureNode::NodeType::RunModuleList, {ProcedureNode::ControlContext}), modules_(std::move(modules)),
      frequency_(frequency), energyStability_(flags.isSet(ModuleLayer::RunControlFlag::EnergyStability)), sizeFactors_(flags.isSet(ModuleLayer::RunControlFlag::SizeFactors))
{

    keywords_.setOrganisation("Options", "Target");
    keywords_.add<ModuleVectorKeyword>("Modules", "Target modules to run", modules_);
    keywords_.setOrganisation("Options", "Run Control");
    keywords_.add<IntegerKeyword>("Frequency", "Frequency to run modules", frequency_);
    keywords_.add<BoolKeyword>("Energy Stability", "Run if the energy of all relevant configurations is stable", energyStability_);
    keywords_.add<BoolKeyword>("Size Factors", "Run if the size factors of all relevant configurations are 1.0", sizeFactors_);
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
bool RunModuleListNode::execute(const ProcedureContext &procedureContext)
{
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
    return true;
}