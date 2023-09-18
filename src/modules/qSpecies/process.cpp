// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "main/dissolve.h"
#include "module/context.h"
#include "modules/qSpecies/qSpecies.h"
#include "procedure/nodes/select.h"
#include "procedure/nodes/sequence.h"

// Run main processing
Module::ExecutionResult QSpeciesModule::process(ModuleContext &moduleContext)
{
    // Check for zero Configuration targets
    if (!targetConfiguration_)
    {
        Messenger::error("No configuration target set for module '{}'.\n", name());
        return ExecutionResult::Failed;
    }

    // Ensure any parameters in our nodes are set correctly
    if (excludeSameMolecule_)
        selectNF_->setSameMoleculeExclusions({selectBO_});
    else
        selectNF_->setSameMoleculeExclusions({});
    selectNF_->keywords().set("InclusiveRange", distanceRange_);

    // Execute the analysis
    ProcedureContext context(moduleContext.processPool(), targetConfiguration_);
    context.setDissolve(moduleContext.dissolve());
    context.setProcessingDataPrefix(name());

    if (!analyser_.execute(context))
    {
        Messenger::error("Q Species experienced problems with its analysis.\n");
        return ExecutionResult::Failed;
    }

    return ExecutionResult::Success;
}
