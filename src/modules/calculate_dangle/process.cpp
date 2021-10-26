// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "base/sysfunc.h"
#include "main/dissolve.h"
#include "modules/calculate_dangle/dangle.h"
#include "procedure/nodes/collect1d.h"
#include "procedure/nodes/collect2d.h"
#include "procedure/nodes/select.h"

// Run set-up stage
bool CalculateDAngleModule::setUp(Dissolve &dissolve, ProcessPool &procPool) { return true; }

// Run main processing
bool CalculateDAngleModule::process(Dissolve &dissolve, ProcessPool &procPool)
{
    // Check for zero Configuration targets
    if (targetConfigurationsKeyword_.data().empty())
        return Messenger::error("No configuration targets set for module '{}'.\n", uniqueName());

    // Ensure any parameters in our nodes are set correctly
    const auto distanceRange = keywords_.asVec3Double("DistanceRange");
    const auto angleRange = keywords_.asVec3Double("AngleRange");
    collectDistance_->setKeyword<Vec3<double>>("RangeX", distanceRange);
    collectAngle_->setKeyword<Vec3<double>>("RangeX", angleRange);
    collectDAngle_->setKeyword<Vec3<double>>("RangeX", distanceRange);
    collectDAngle_->setKeyword<Vec3<double>>("RangeY", angleRange);
    if (excludeSameMolecule_)
        selectC_->setKeyword<std::vector<const ProcedureNode *>>("ExcludeSameMolecule", {selectA_});
    else
        selectC_->setKeyword<std::vector<const ProcedureNode *>>("ExcludeSameMolecule", {});

    // Grab Configuration pointer
    auto *cfg = targetConfigurationsKeyword_.data().front();

    // Set up process pool - must do this to ensure we are using all available processes
    procPool.assignProcessesToGroups(cfg->processPool());

    // Execute the analysis
    if (!analyser_.execute(procPool, cfg, uniqueName(), dissolve.processingModuleData()))
        return Messenger::error("CalculateDAngle experienced problems with its analysis.\n");

    return true;
}
