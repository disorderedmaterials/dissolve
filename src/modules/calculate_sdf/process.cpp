// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "base/sysfunc.h"
#include "main/dissolve.h"
#include "modules/calculate_sdf/sdf.h"
#include "procedure/nodes/collect3d.h"
#include "procedure/nodes/process3d.h"
#include "procedure/nodes/select.h"
#include "procedure/nodes/sequence.h"

// Run main processing
bool CalculateSDFModule::process(Dissolve &dissolve, ProcessPool &procPool)
{
    // Check for Configuration target
    if (!targetConfiguration_)
        return Messenger::error("No configuration targets set for module '{}'.\n", uniqueName());

    // Ensure any parameters in our nodes are set correctly
    collectVector_->keywords().set("RangeX", rangeX_);
    collectVector_->keywords().set("RangeY", rangeY_);
    collectVector_->keywords().set("RangeZ", rangeZ_);
    if (excludeSameMolecule_)
        selectB_->keywords().set("ExcludeSameMolecule", std::vector<const SelectProcedureNode *>{selectA_});
    else
        selectB_->keywords().set("ExcludeSameMolecule", std::vector<const SelectProcedureNode *>{});

    // Set up process pool - must do this to ensure we are using all available processes
    procPool.assignProcessesToGroups(targetConfiguration_->processPool());

    // Execute the analysis
    if (!analyser_.execute(procPool, targetConfiguration_, uniqueName(), dissolve.processingModuleData()))
        return Messenger::error("CalculateSDF experienced problems with its analysis.\n");

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
                return false;
            }
        }
        else if (!procPool.decision())
            return false;
    }

    return true;
}
