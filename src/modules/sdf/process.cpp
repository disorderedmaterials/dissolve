// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "base/sysFunc.h"
#include "main/dissolve.h"
#include "modules/sdf/sdf.h"
#include "procedure/nodes/collect3D.h"
#include "procedure/nodes/process3D.h"
#include "procedure/nodes/select.h"
#include "procedure/nodes/sequence.h"

// Run main processing
enum Module::executionResult SDFModule::process(Dissolve &dissolve, const ProcessPool &procPool)
{
    // Check for Configuration target
    if (!targetConfiguration_)
    {
        Messenger::error("No configuration target set for module '{}'.\n", name());
        return failed;
    }

    // Ensure any parameters in our nodes are set correctly
    collectVector_->keywords().set("RangeX", rangeX_);
    collectVector_->keywords().set("RangeY", rangeY_);
    collectVector_->keywords().set("RangeZ", rangeZ_);
    if (excludeSameMolecule_)
        selectB_->keywords().set("ExcludeSameMolecule", ConstNodeVector<SelectProcedureNode>{selectA_});
    else
        selectB_->keywords().set("ExcludeSameMolecule", ConstNodeVector<SelectProcedureNode>{});

    // Execute the analysis
    ProcedureContext context(procPool, targetConfiguration_);
    context.setDataListAndPrefix(dissolve.processingModuleData(), name());
    if (!analyser_.execute(context))
    {
        Messenger::error("CalculateSDF experienced problems with its analysis.\n");
        return failed;
    }

    // Save data?
    if (sdfFileAndFormat_.hasFilename())
    {
        if (procPool.isMaster())
        {
            if (sdfFileAndFormat_.exportData(processPosition_->processedData()))
                procPool.decideTrue();
            else
            {
                procPool.decideFalse();
                return failed;
            }
        }
        else if (!procPool.decision())
            return failed;
    }

    return success;
}
