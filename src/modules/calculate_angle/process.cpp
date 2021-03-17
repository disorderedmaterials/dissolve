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
    RefList<SelectProcedureNode> exclusions;
    const auto rangeAB = keywords_.asVec3Double("RangeAB");
    const auto rangeBC = keywords_.asVec3Double("RangeBC");
    const auto angleRange = keywords_.asVec3Double("AngleRange");
    selectA_->setKeyword<SelectProcedureNode *>("ReferenceSite", selectB_);
    selectA_->setKeyword<Range>("InclusiveRange", Range(rangeAB.x, rangeAB.y));
    selectC_->setKeyword<SelectProcedureNode *>("ReferenceSite", selectB_);
    selectC_->setKeyword<Range>("InclusiveRange", Range(rangeBC.x, rangeBC.y));
    collectAB_->setKeyword<Vec3<double>>("RangeX", rangeAB);
    collectBC_->setKeyword<Vec3<double>>("RangeX", rangeBC);
    collectABC_->setKeyword<Vec3<double>>("RangeX", angleRange);
    collectDAngleAB_->setKeyword<Vec3<double>>("RangeX", rangeAB);
    collectDAngleAB_->setKeyword<Vec3<double>>("RangeY", angleRange);
    collectDAngleBC_->setKeyword<Vec3<double>>("RangeX", rangeBC);
    collectDAngleBC_->setKeyword<Vec3<double>>("RangeY", angleRange);
    const bool excludeSameMoleculeAB = keywords_.asBool("ExcludeSameMoleculeAB");
    exclusions.clear();
    if (excludeSameMoleculeAB)
        exclusions.append(selectB_);
    selectA_->setKeyword<RefList<SelectProcedureNode> &>("ExcludeSameMolecule", exclusions);
    const bool excludeSameMoleculeBC = keywords_.asBool("ExcludeSameMoleculeBC");
    exclusions.clear();
    if (excludeSameMoleculeBC)
        exclusions.append(selectB_);
    selectC_->setKeyword<RefList<SelectProcedureNode> &>("ExcludeSameMolecule", exclusions);
    const bool excludeSameSiteAC = keywords_.asBool("ExcludeSameSiteAC");
    exclusions.clear();
    if (excludeSameSiteAC)
        exclusions.append(selectA_);
    selectC_->setKeyword<RefList<SelectProcedureNode> &>("ExcludeSameSite", exclusions);

    // Grab Configuration pointer
    auto *cfg = targetConfigurations_.firstItem();

    // Set up process pool - must do this to ensure we are using all available processes
    procPool.assignProcessesToGroups(cfg->processPool());

    // Execute the analysis
    if (!analyser_.execute(procPool, cfg, uniqueName(), dissolve.processingModuleData()))
        return Messenger::error("CalculateAngle experienced problems with its analysis.\n");

    return true;
}
