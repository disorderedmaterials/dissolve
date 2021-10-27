// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "base/sysfunc.h"
#include "main/dissolve.h"
#include "modules/calculate_angle/angle.h"
#include "procedure/nodes/collect1d.h"
#include "procedure/nodes/collect2d.h"
#include "procedure/nodes/select.h"

// Run set-up stage
bool CalculateAngleModule::setUp(Dissolve &dissolve, ProcessPool &procPool) { return true; }

// Run main processing
bool CalculateAngleModule::process(Dissolve &dissolve, ProcessPool &procPool)
{
    // Check for zero Configuration targets
    if (targetConfigurationsKeyword_.data().empty())
        return Messenger::error("No configuration targets set for module '{}'.\n", uniqueName());

    // Ensure any parameters in our nodes are set correctly
    selectA_->setKeyword<const ProcedureNode *>("ReferenceSite", selectB_);
    selectA_->setKeyword<Range>("InclusiveRange", Range(rangeAB_.x, rangeAB_.y));
    selectC_->setKeyword<const ProcedureNode *>("ReferenceSite", selectB_);
    selectC_->setKeyword<Range>("InclusiveRange", Range(rangeBC_.x, rangeBC_.y));
    collectAB_->setKeyword<Vec3<double>>("RangeX", rangeAB_);
    collectBC_->setKeyword<Vec3<double>>("RangeX", rangeBC_);
    collectABC_->setKeyword<Vec3<double>>("RangeX", angleRange_);
    collectDAngleAB_->setKeyword<Vec3<double>>("RangeX", rangeAB_);
    collectDAngleAB_->setKeyword<Vec3<double>>("RangeY", angleRange_);
    collectDAngleBC_->setKeyword<Vec3<double>>("RangeX", rangeBC_);
    collectDAngleBC_->setKeyword<Vec3<double>>("RangeY", angleRange_);
    if (excludeSameMoleculeAB_)
        selectA_->setKeyword<std::vector<const ProcedureNode *>>("ExcludeSameMolecule", {selectB_});
    else
        selectA_->setKeyword<std::vector<const ProcedureNode *>>("ExcludeSameMolecule", {});
    if (excludeSameMoleculeBC_)
        selectC_->setKeyword<std::vector<const ProcedureNode *>>("ExcludeSameMolecule", {selectB_});
    else
        selectC_->setKeyword<std::vector<const ProcedureNode *>>("ExcludeSameMolecule", {});
    if (excludeSameSiteAC_)
        selectC_->setKeyword<std::vector<const ProcedureNode *>>("ExcludeSameSite", {selectA_});
    else
        selectC_->setKeyword<std::vector<const ProcedureNode *>>("ExcludeSameSite", {});

    // Grab Configuration pointer
    auto *cfg = targetConfigurationsKeyword_.data().front();

    // Set up process pool - must do this to ensure we are using all available processes
    procPool.assignProcessesToGroups(cfg->processPool());

    // Execute the analysis
    if (!analyser_.execute(procPool, cfg, uniqueName(), dissolve.processingModuleData()))
        return Messenger::error("CalculateAngle experienced problems with its analysis.\n");

    return true;
}
