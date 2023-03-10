// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "expression/variable.h"
#include "main/dissolve.h"
#include "modules/intraangle/intraangle.h"
#include "procedure/nodes/calculateangle.h"
#include "procedure/nodes/collect1d.h"
#include "procedure/nodes/operateexpression.h"
#include "procedure/nodes/select.h"

// Run main processing
bool IntraAngleModule::process(Dissolve &dissolve, const ProcessPool &procPool)
{
    // Check for zero Configuration targets
    if (!targetConfiguration_)
        return Messenger::error("No configuration target set for module '{}'.\n", name());

    // Ensure any parameters in our nodes are set correctly
    selectB_->setDistanceReferenceSite(selectA_);
    selectB_->setInclusiveDistanceRange({rangeAB_.x, rangeAB_.y});
    selectC_->setDistanceReferenceSite(selectB_);
    selectC_->setInclusiveDistanceRange({rangeBC_.x, rangeBC_.y});
    calculateAngle_->keywords().set("Symmetric", symmetric_);
    collectAB_->keywords().set("RangeX", rangeAB_);
    collectBC_->keywords().set("RangeX", rangeBC_);
    collectABC_->keywords().set("RangeX", angleRange_);
    if (excludeSameMoleculeAB_)
        selectB_->keywords().set("ExcludeSameMolecule", ConstNodeVector<SelectProcedureNode>{selectA_});
    else
        selectB_->keywords().set("ExcludeSameMolecule", ConstNodeVector<SelectProcedureNode>{});
    if (excludeSameMoleculeBC_)
        selectC_->keywords().set("ExcludeSameMolecule", ConstNodeVector<SelectProcedureNode>{selectB_});
    else
        selectC_->keywords().set("ExcludeSameMolecule", ConstNodeVector<SelectProcedureNode>{});
    if (excludeSameSiteAC_)
        selectC_->keywords().set("ExcludeSameSite", ConstNodeVector<SelectProcedureNode>{selectA_});
    else
        selectC_->keywords().set("ExcludeSameSite", ConstNodeVector<SelectProcedureNode>{});

    // Execute the analysis
    ProcedureContext context(procPool, targetConfiguration_);
    context.setDataListAndPrefix(dissolve.processingModuleData(), name());
    if (!analyser_.execute(context))
        return Messenger::error("CalculateAngle experienced problems with its analysis.\n");

    return true;
}
