// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "expression/variable.h"
#include "main/dissolve.h"
#include "module/context.h"
#include "modules/intraAngle/intraAngle.h"
#include "procedure/nodes/calculateAngle.h"
#include "procedure/nodes/collect1D.h"
#include "procedure/nodes/operateExpression.h"
#include "procedure/nodes/select.h"

// Run main processing
Module::ExecutionResult IntraAngleModule::process(ModuleContext &moduleContext)
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
    collectABC_->keywords().set("RangeX", angleRange_);

    // Execute the analysis
    if (!analyser_.execute({moduleContext.dissolve(), targetConfiguration_, name()}))
    {
        Messenger::error("CalculateAngle experienced problems with its analysis.\n");
        return ExecutionResult::Failed;
    }

    return ExecutionResult::Success;
}
