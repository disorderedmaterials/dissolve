// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#include "base/sysfunc.h"
#include "main/dissolve.h"
#include "modules/calculate_axisangle/axisangle.h"
#include "procedure/nodes/collect1d.h"
#include "procedure/nodes/collect2d.h"
#include "procedure/nodes/select.h"

// Run set-up stage
bool CalculateAxisAngleModule::setUp(Dissolve &dissolve, ProcessPool &procPool, KeywordSignals actionSignals) { return true; }

// Run main processing
bool CalculateAxisAngleModule::process(Dissolve &dissolve, ProcessPool &procPool)
{
    // Check for zero Configuration targets
    if (!targetConfiguration_)
        return Messenger::error("No configuration targets set for module '{}'.\n", uniqueName());

    // Ensure any parameters in our nodes are set correctly
    collectDistance_->keywords().set("RangeX", distanceRange_);
    collectAngle_->keywords().set("RangeX", angleRange_);
    collectDAngle_->keywords().set("RangeX", distanceRange_);
    collectDAngle_->keywords().set("RangeY", angleRange_);
    if (excludeSameMolecule_)
        selectB_->keywords().set("ExcludeSameMolecule", std::vector<std::shared_ptr<const SelectProcedureNode>>{selectA_});
    else
        selectB_->keywords().set("ExcludeSameMolecule", std::vector<std::shared_ptr<const SelectProcedureNode>>{});

    // Set up process pool - must do this to ensure we are using all available processes
    procPool.assignProcessesToGroups(targetConfiguration_->processPool());

    // Execute the analysis
    if (!analyser_.execute(procPool, targetConfiguration_, uniqueName(), dissolve.processingModuleData()))
        return Messenger::error("CalculateDAngle experienced problems with its analysis.\n");

    return true;
}
