// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#include "base/sysfunc.h"
#include "main/dissolve.h"
#include "modules/calculate_angle/angle.h"
#include "procedure/nodes/collect1d.h"
#include "procedure/nodes/collect2d.h"
#include "procedure/nodes/select.h"

// Run main processing
bool CalculateAngleModule::process(Dissolve &dissolve, ProcessPool &procPool)
{
    // Check for zero Configuration targets
    if (!targetConfiguration_)
        return Messenger::error("No configuration target set for module '{}'.\n", uniqueName());

    // Ensure any parameters in our nodes are set correctly
    selectA_->setDistanceReferenceSite(selectB_);
    selectA_->setInclusiveDistanceRange({rangeAB_.x, rangeAB_.y});
    selectC_->setDistanceReferenceSite(selectB_);
    selectC_->setInclusiveDistanceRange({rangeBC_.x, rangeBC_.y});
    collectAB_->keywords().set("RangeX", rangeAB_);
    collectBC_->keywords().set("RangeX", rangeBC_);
    collectABC_->keywords().set("RangeX", angleRange_);
    collectDAngleAB_->keywords().set("RangeX", rangeAB_);
    collectDAngleAB_->keywords().set("RangeY", angleRange_);
    collectDAngleBC_->keywords().set("RangeX", rangeBC_);
    collectDAngleBC_->keywords().set("RangeY", angleRange_);
    if (excludeSameMoleculeAB_)
        selectA_->keywords().set("ExcludeSameMolecule", std::vector<std::shared_ptr<const SelectProcedureNode>>{selectB_});
    else
        selectA_->keywords().set("ExcludeSameMolecule", std::vector<std::shared_ptr<const SelectProcedureNode>>{});
    if (excludeSameMoleculeBC_)
        selectC_->keywords().set("ExcludeSameMolecule", std::vector<std::shared_ptr<const SelectProcedureNode>>{selectB_});
    else
        selectC_->keywords().set("ExcludeSameMolecule", std::vector<std::shared_ptr<const SelectProcedureNode>>{});
    if (excludeSameSiteAC_)
        selectC_->keywords().set("ExcludeSameSite", std::vector<std::shared_ptr<const SelectProcedureNode>>{selectA_});
    else
        selectC_->keywords().set("ExcludeSameSite", std::vector<std::shared_ptr<const SelectProcedureNode>>{});

    // Set up process pool - must do this to ensure we are using all available processes
    procPool.assignProcessesToGroups(targetConfiguration_->processPool());

    // Execute the analysis
    ProcedureContext context(procPool, targetConfiguration_);
    context.setDataListAndPrefix(dissolve.processingModuleData(), uniqueName());
    if (!analyser_.execute(context))
        return Messenger::error("CalculateAngle experienced problems with its analysis.\n");

    return true;
}
