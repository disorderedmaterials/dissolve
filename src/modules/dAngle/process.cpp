// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "base/sysFunc.h"
#include "main/dissolve.h"
#include "modules/dAngle/dAngle.h"
#include "procedure/nodes/calculateAngle.h"
#include "procedure/nodes/collect1D.h"
#include "procedure/nodes/collect2D.h"
#include "procedure/nodes/operateExpression.h"
#include "procedure/nodes/select.h"

// Run main processing
bool DAngleModule::process(Dissolve &dissolve, const ProcessPool &procPool)
{
    // Check for Configuration target
    if (!targetConfiguration_)
        return Messenger::error("No configuration target set for module '{}'.\n", name());

    // Ensure any parameters in our nodes are set correctly
    calculateAngle_->keywords().set("Symmetric", symmetric_);
    dAngleNormalisationExpression_->setExpression(
        fmt::format("{} * value/sin(toRad(y))/sin(toRad(yDelta))", symmetric_ ? 1.0 : 2.0));
    collectDistance_->keywords().set("RangeX", distanceRange_);
    collectAngle_->keywords().set("RangeX", angleRange_);
    collectDAngle_->keywords().set("RangeX", distanceRange_);
    collectDAngle_->keywords().set("RangeY", angleRange_);
    if (excludeSameMolecule_)
        selectC_->keywords().set("ExcludeSameMolecule", ConstNodeVector<SelectProcedureNode>{selectA_});
    else
        selectC_->keywords().set("ExcludeSameMolecule", ConstNodeVector<SelectProcedureNode>{});

    // Execute the analysis
    ProcedureContext context(procPool, targetConfiguration_);
    context.setDataListAndPrefix(dissolve.processingModuleData(), name());
    if (!analyser_.execute(context))
        return Messenger::error("CalculateDAngle experienced problems with its analysis.\n");

    return true;
}
