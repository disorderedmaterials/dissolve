// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "main/dissolve.h"
#include "module/context.h"
#include "modules/orientedSDF/orientedSDF.h"
#include "procedure/nodes/calculateAxisAngle.h"
#include "procedure/nodes/collect3D.h"
#include "procedure/nodes/ifValueInRange.h"
#include "procedure/nodes/process3D.h"
#include "procedure/nodes/select.h"
#include "procedure/nodes/sequence.h"

// Run main processing
Module::ExecutionResult OrientedSDFModule::process(ModuleContext &moduleContext)
{
    // Check for Configuration target
    if (!targetConfiguration_)
    {
        Messenger::error("No configuration target set for module '{}'.\n", name());
        return ExecutionResult::Failed;
    }

    // Ensure any parameters in our nodes are set correctly
    calculateAxisAngle_->keywords().set("Symmetric", symmetric_);
    checkAxisValue_->keywords().set("ValidRange", axisAngleRange_);
    collectVector_->keywords().set("RangeX", rangeX_);
    collectVector_->keywords().set("RangeY", rangeY_);
    collectVector_->keywords().set("RangeZ", rangeZ_);
    if (excludeSameMolecule_)
        selectB_->keywords().set("ExcludeSameMolecule", ConstNodeVector<SelectProcedureNode>{selectA_});
    else
        selectB_->keywords().set("ExcludeSameMolecule", ConstNodeVector<SelectProcedureNode>{});

    // Execute the analysis
    if (!analyser_.execute({moduleContext.dissolve(), targetConfiguration_, name()}))
    {
        Messenger::error("CalculateSDF experienced problems with its analysis.\n");
        return ExecutionResult::Failed;
    }

    // Save data?
    if (sdfFileAndFormat_.hasFilename())
    {
        if (moduleContext.processPool().isMaster())
        {
            if (sdfFileAndFormat_.exportData(processPosition_->processedData()))
                moduleContext.processPool().decideTrue();
            else
            {
                moduleContext.processPool().decideFalse();
                return ExecutionResult::Failed;
            }
        }
        else if (!moduleContext.processPool().decision())
            return ExecutionResult::Failed;
    }

    return ExecutionResult::Success;
}
