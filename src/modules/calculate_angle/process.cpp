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
    if (targetConfigurations_.nItems() == 0)
        return Messenger::error("No configuration targets set for module '{}'.\n", uniqueName());

    // Ensure any parameters in our nodes are set correctly
    const auto rangeAB = keywords_.asVec3Double("RangeAB");
    const auto rangeBC = keywords_.asVec3Double("RangeBC");
    const auto angleRange = keywords_.asVec3Double("AngleRange");
    selectA_->setKeyword<const ProcedureNode *>("ReferenceSite", selectB_);
    selectA_->setKeyword<Range>("InclusiveRange", Range(rangeAB.x, rangeAB.y));
    selectC_->setKeyword<const ProcedureNode *>("ReferenceSite", selectB_);
    selectC_->setKeyword<Range>("InclusiveRange", Range(rangeBC.x, rangeBC.y));
    collectAB_->setKeyword<Vec3<double>>("RangeX", rangeAB);
    collectBC_->setKeyword<Vec3<double>>("RangeX", rangeBC);
    collectABC_->setKeyword<Vec3<double>>("RangeX", angleRange);
    collectDAngleAB_->setKeyword<Vec3<double>>("RangeX", rangeAB);
    collectDAngleAB_->setKeyword<Vec3<double>>("RangeY", angleRange);
    collectDAngleBC_->setKeyword<Vec3<double>>("RangeX", rangeBC);
    collectDAngleBC_->setKeyword<Vec3<double>>("RangeY", angleRange);
    const bool excludeSameMoleculeAB = keywords_.asBool("ExcludeSameMoleculeAB");
    if (excludeSameMoleculeAB)
        selectA_->setKeyword<std::vector<const ProcedureNode *>>("ExcludeSameMolecule", {selectB_});
    const bool excludeSameMoleculeBC = keywords_.asBool("ExcludeSameMoleculeBC");
    if (excludeSameMoleculeBC)
        selectC_->setKeyword<std::vector<const ProcedureNode *>>("ExcludeSameMolecule", {selectB_});
    const bool excludeSameSiteAC = keywords_.asBool("ExcludeSameSiteAC");
    if (excludeSameSiteAC)
        selectC_->setKeyword<std::vector<const ProcedureNode *>>("ExcludeSameSite", {selectA_});

    // Grab Configuration pointer
    auto *cfg = targetConfigurations_.firstItem();

    // Set up process pool - must do this to ensure we are using all available processes
    procPool.assignProcessesToGroups(cfg->processPool());

    // Execute the analysis
    if (!analyser_.execute(procPool, cfg, uniqueName(), dissolve.processingModuleData()))
        return Messenger::error("CalculateAngle experienced problems with its analysis.\n");

    return true;
}
