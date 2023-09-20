// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "base/sysFunc.h"
#include "io/export/data1D.h"
#include "main/dissolve.h"
#include "module/context.h"
#include "modules/histogramCN/histogramCN.h"
#include "procedure/nodes/integerCollect1D.h"
#include "procedure/nodes/operateSitePopulationNormalise.h"
#include "procedure/nodes/select.h"
#include "procedure/nodes/sequence.h"
#include "procedure/nodes/sum1D.h"

// Run main processing
Module::ExecutionResult HistogramCNModule::process(ModuleContext &moduleContext)
{
    // Check for zero Configuration targets
    if (!targetConfiguration_)
    {
        Messenger::error("No configuration target set for module '{}'.\n", name());
        return ExecutionResult::Failed;
    }

    // Ensure any parameters in our nodes are set correctly
    if (excludeSameMolecule_)
        selectB_->setSameMoleculeExclusions({selectA_});
    else
        selectB_->setSameMoleculeExclusions({});
    selectB_->keywords().set("InclusiveRange", distanceRange_);

    // Execute the analysis
    if (!analyser_.execute({moduleContext.dissolve(), targetConfiguration_, name()}))
    {
        Messenger::error("HistogramCN experienced problems with its analysis.\n");
        return ExecutionResult::Failed;
    }

    return ExecutionResult::Success;
}
