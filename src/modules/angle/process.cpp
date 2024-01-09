// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "expression/variable.h"
#include "main/dissolve.h"
#include "module/context.h"
#include "modules/angle/angle.h"
#include "procedure/nodes/calculateAngle.h"
#include "procedure/nodes/collect1D.h"
#include "procedure/nodes/collect2D.h"
#include "procedure/nodes/collect3D.h"
#include "procedure/nodes/operateExpression.h"
#include "procedure/nodes/select.h"

// Run main processing
Module::ExecutionResult AngleModule::process(ModuleContext &moduleContext)
{
    // Check for zero Configuration targets
    if (!targetConfiguration_)
    {
        Messenger::error("No configuration target set for module '{}'.\n", name());
        return ExecutionResult::Failed;
    }

    // Ensure any parameters in our nodes are set correctly
    selectB_->setDistanceReferenceSite(selectA_);
    selectB_->setInclusiveDistanceRange({rangeAB_.x, rangeAB_.y});
    selectC_->setDistanceReferenceSite(selectB_);
    selectC_->setInclusiveDistanceRange({rangeBC_.x, rangeBC_.y});
    calculateAngle_->keywords().set("Symmetric", symmetric_);
    dAngleABNormalisationExpression_->setExpression(
        fmt::format("{} * value/sin(toRad(y))/sin(toRad(yDelta))", symmetric_ ? 1.0 : 2.0));
    dAngleBCNormalisationExpression_->setExpression(
        fmt::format("{} * value/sin(toRad(y))/sin(toRad(yDelta))", symmetric_ ? 1.0 : 2.0));
    collectDDA_->keywords().set("RangeX", rangeAB_);
    collectDDA_->keywords().set("RangeY", rangeBC_);
    collectDDA_->keywords().set("RangeZ", angleRange_);
    collectAB_->keywords().set("RangeX", rangeAB_);
    collectBC_->keywords().set("RangeX", rangeBC_);
    collectABC_->keywords().set("RangeX", angleRange_);
    collectDAngleAB_->keywords().set("RangeX", rangeAB_);
    collectDAngleAB_->keywords().set("RangeY", angleRange_);
    collectDAngleBC_->keywords().set("RangeX", rangeBC_);
    collectDAngleBC_->keywords().set("RangeY", angleRange_);
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
    if (!analyser_.execute({moduleContext.dissolve(), targetConfiguration_, name()}))
    {
        Messenger::error("Angle experienced problems with its analysis.\n");
        return ExecutionResult::Failed;
    }

    return ExecutionResult::Success;
}
