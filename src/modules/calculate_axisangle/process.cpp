// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "base/sysfunc.h"
#include "main/dissolve.h"
#include "modules/calculate_axisangle/axisangle.h"
#include "procedure/nodes/collect1d.h"
#include "procedure/nodes/collect2d.h"
#include "procedure/nodes/select.h"

// Run set-up stage
bool CalculateAxisAngleModule::setUp(Dissolve &dissolve, ProcessPool &procPool) { return true; }

// Run main processing
bool CalculateAxisAngleModule::process(Dissolve &dissolve, ProcessPool &procPool)
{
    // Check for zero Configuration targets
    if (targetConfigurations_.empty())
        return Messenger::error("No configuration targets set for module '{}'.\n", uniqueName());

    // Ensure any parameters in our nodes are set correctly
    collectDistance_->setKeyword<Vec3<double>>("RangeX", distanceRange_);
    collectAngle_->setKeyword<Vec3<double>>("RangeX", angleRange_);
    collectDAngle_->setKeyword<Vec3<double>>("RangeX", distanceRange_);
    collectDAngle_->setKeyword<Vec3<double>>("RangeY", angleRange_);
    if (excludeSameMolecule_)
        selectB_->setKeyword<std::vector<const ProcedureNode *>>("ExcludeSameMolecule", {selectA_});
    else
        selectB_->setKeyword<std::vector<const ProcedureNode *>>("ExcludeSameMolecule", {});

    // Grab Configuration pointer
    auto *cfg = targetConfigurations_.front();

    // Set up process pool - must do this to ensure we are using all available processes
    procPool.assignProcessesToGroups(cfg->processPool());

    // Execute the analysis
    if (!analyser_.execute(procPool, cfg, uniqueName(), dissolve.processingModuleData()))
        return Messenger::error("CalculateDAngle experienced problems with its analysis.\n");

    return true;
}
