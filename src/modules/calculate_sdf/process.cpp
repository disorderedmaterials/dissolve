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
    // Check for zero Configuration targets
    if (targetConfigurationsKeyword_.data().empty())
        return Messenger::error("No configuration targets set for module '{}'.\n", uniqueName());

    // Ensure any parameters in our nodes are set correctly
    const auto rangeX = keywords_.asVec3Double("RangeX");
    const auto rangeY = keywords_.asVec3Double("RangeY");
    const auto rangeZ = keywords_.asVec3Double("RangeZ");
    collectVector_->setKeyword<Vec3<double>>("RangeX", rangeX);
    collectVector_->setKeyword<Vec3<double>>("RangeY", rangeY);
    collectVector_->setKeyword<Vec3<double>>("RangeZ", rangeZ);
    if (excludeSameMolecule_)
        selectB_->setKeyword<std::vector<const ProcedureNode *>>("ExcludeSameMolecule", {selectA_});
    else
        selectB_->setKeyword<std::vector<const ProcedureNode *>>("ExcludeSameMolecule", {});

    // Grab Configuration pointer
    auto *cfg = targetConfigurationsKeyword_.data().front();

    // Set up process pool - must do this to ensure we are using all available processes
    procPool.assignProcessesToGroups(cfg->processPool());

    // Execute the analysis
    if (!analyser_.execute(procPool, cfg, uniqueName(), dissolve.processingModuleData()))
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
