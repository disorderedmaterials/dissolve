// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "base/sysFunc.h"
#include "io/export/data1D.h"
#include "main/dissolve.h"
#include "module/context.h"
#include "modules/siteRDF/siteRDF.h"
#include "procedure/nodes/collect1D.h"
#include "procedure/nodes/operateSitePopulationNormalise.h"
#include "procedure/nodes/select.h"
#include "procedure/nodes/sequence.h"
#include "procedure/nodes/sum1D.h"

// Run main processing
Module::ExecutionResult SiteRDFModule::process(ModuleContext &moduleContext)
{
    // Check for zero Configuration targets
    if (!targetConfiguration_)
    {
        Messenger::error("No configuration target set for module '{}'.\n", name());
        return ExecutionResult::Failed;
    }

    // Ensure any parameters in our nodes are set correctly
    collectDistance_->keywords().set("RangeX", distanceRange_);
    if (excludeSameMolecule_)
        selectB_->setSameMoleculeExclusions({selectA_});
    else
        selectB_->setSameMoleculeExclusions({});
    cnNormaliser_->keywords().set("Site", ConstNodeVector<SelectProcedureNode>{selectA_});

    // Execute the analysis
    if (!analyser_.execute({moduleContext.dissolve(), targetConfiguration_, name()}))
    {
        Messenger::error("CalculateRDF experienced problems with its analysis.\n");
        return ExecutionResult::Failed;
    }

    // Accumulate instantaneous coordination number data
    if (instantaneous_)
    {
        if (isRangeEnabled(0))
        {
            auto &sumA =
                moduleContext.dissolve().processingModuleData().realise<Data1D>("SumA", name(), GenericItem::InRestartFileFlag);
            sumA.addPoint(moduleContext.dissolve().iteration(), sumCN_->sum(0).value());
            if (exportInstantaneous_)
            {
                Data1DExportFileFormat exportFormat(fmt::format("{}_SumA.txt", name()));
                if (!exportFormat.exportData(sumA))
                {
                    Messenger::error("Failed to write instantaneous coordination number data for range A.\n");
                    return ExecutionResult::Failed;
                }
            }
        }
        if (isRangeEnabled(1))
        {
            auto &sumB =
                moduleContext.dissolve().processingModuleData().realise<Data1D>("SumB", name(), GenericItem::InRestartFileFlag);
            sumB.addPoint(moduleContext.dissolve().iteration(), sumCN_->sum(1).value());
            if (exportInstantaneous_)
            {
                Data1DExportFileFormat exportFormat(fmt::format("{}_SumB.txt", name()));
                if (!exportFormat.exportData(sumB))
                {
                    Messenger::error("Failed to write instantaneous coordination number data for range B.\n");
                    return ExecutionResult::Failed;
                }
            }
        }
        if (isRangeEnabled(2))
        {
            auto &sumC =
                moduleContext.dissolve().processingModuleData().realise<Data1D>("SumC", name(), GenericItem::InRestartFileFlag);
            sumC.addPoint(moduleContext.dissolve().iteration(), sumCN_->sum(2).value());
            if (exportInstantaneous_)
            {
                Data1DExportFileFormat exportFormat(fmt::format("{}_SumC.txt", name()));
                if (!exportFormat.exportData(sumC))
                {
                    Messenger::error("Failed to write instantaneous coordination number data for range C.\n");
                    return ExecutionResult::Failed;
                }
            }
        }
    }

    return ExecutionResult::Success;
}
