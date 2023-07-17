// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "base/sysFunc.h"
#include "io/export/data1D.h"
#include "main/dissolve.h"
#include "modules/siteRDF/siteRDF.h"
#include "procedure/nodes/collect1D.h"
#include "procedure/nodes/operateSitePopulationNormalise.h"
#include "procedure/nodes/select.h"
#include "procedure/nodes/sequence.h"
#include "procedure/nodes/sum1D.h"

// Run main processing
enum executionResult SiteRDFModule::process(Dissolve &dissolve, const ProcessPool &procPool)
{
    // Check for zero Configuration targets
    if (!targetConfiguration_)
    {
        Messenger::error("No configuration target set for module '{}'.\n", name());
        return failed;
    }

    // Ensure any parameters in our nodes are set correctly
    collectDistance_->keywords().set("RangeX", distanceRange_);
    if (excludeSameMolecule_)
        selectB_->setSameMoleculeExclusions({selectA_});
    else
        selectB_->setSameMoleculeExclusions({});
    cnNormaliser_->keywords().set("Site", ConstNodeVector<SelectProcedureNode>{selectA_});

    // Execute the analysis
    ProcedureContext context(procPool, targetConfiguration_);
    context.setDataListAndPrefix(dissolve.processingModuleData(), name());
    if (!analyser_.execute(context))
    {
        Messenger::error("CalculateRDF experienced problems with its analysis.\n");
        return failed;
    }

    // Accumulate instantaneous coordination number data
    if (instantaneous_)
    {
        if (isRangeEnabled(0))
        {
            auto &sumA = dissolve.processingModuleData().realise<Data1D>("SumA", name(), GenericItem::InRestartFileFlag);
            sumA.addPoint(dissolve.iteration(), sumCN_->sum(0).value());
            if (exportInstantaneous_)
            {
                Data1DExportFileFormat exportFormat(fmt::format("{}_SumA.txt", name()));
                if (!exportFormat.exportData(sumA))
                {
                    Messenger::error("Failed to write instantaneous coordination number data for range A.\n");
                    return failed;
                }
            }
        }
        if (isRangeEnabled(1))
        {
            auto &sumB = dissolve.processingModuleData().realise<Data1D>("SumB", name(), GenericItem::InRestartFileFlag);
            sumB.addPoint(dissolve.iteration(), sumCN_->sum(1).value());
            if (exportInstantaneous_)
            {
                Data1DExportFileFormat exportFormat(fmt::format("{}_SumB.txt", name()));
                if (!exportFormat.exportData(sumB))
                {
                    Messenger::error("Failed to write instantaneous coordination number data for range B.\n");
                    return failed;
                }
            }
        }
        if (isRangeEnabled(2))
        {
            auto &sumC = dissolve.processingModuleData().realise<Data1D>("SumC", name(), GenericItem::InRestartFileFlag);
            sumC.addPoint(dissolve.iteration(), sumCN_->sum(2).value());
            if (exportInstantaneous_)
            {
                Data1DExportFileFormat exportFormat(fmt::format("{}_SumC.txt", name()));
                if (!exportFormat.exportData(sumC))
                {
                    Messenger::error("Failed to write instantaneous coordination number data for range C.\n");
                    return failed;
                }
            }
        }
    }

    // Test?
    if (testRangeA_)
    {
        // Is range A enabled?
        if (!isRangeEnabled(0))
        {
            Messenger::error("Test coordination number for range B supplied, but calculation for that range "
                            "is not active.\n");
            return failed;
        }

        const auto delta = testRangeA_.value() - sumCN_->sum(0);

        Messenger::print("Reference coordination number delta with correct value for range A is {:15.9e} and is {} "
                         "(threshold is {:10.3e})\n",
                         delta, fabs(delta) < testThreshold_ ? "OK" : "NOT OK", testThreshold_);
        if (!procPool.allTrue(fabs(delta) < testThreshold_))
            return failed;
    }
    if (testRangeB_)
    {
        // Is range B enabled?
        if (!isRangeEnabled(1))
        {
            Messenger::error("Test coordination number for range B supplied, but calculation for that range "
                                    "is not active.\n");
            return failed;
        }

        const auto delta = testRangeB_.value() - sumCN_->sum(1);

        Messenger::print("Reference coordination number delta with correct value for range B is {:15.9e} and is {} "
                         "(threshold is {:10.3e})\n",
                         delta, fabs(delta) < testThreshold_ ? "OK" : "NOT OK", testThreshold_);
        if (!procPool.allTrue(fabs(delta) < testThreshold_))
            return failed;
    }
    if (testRangeC_)
    {
        // Is range B enabled?
        if (!isRangeEnabled(2))
        {
            Messenger::error("Test coordination number for range C supplied, but calculation for that range "
                            "is not active.\n");
            return failed;
        }

        const auto delta = testRangeC_.value() - sumCN_->sum(2);

        Messenger::print("Reference coordination number delta with correct value for range C is {:15.9e} and is {} "
                         "(threshold is {:10.3e})\n",
                         delta, fabs(delta) < testThreshold_ ? "OK" : "NOT OK", testThreshold_);
        if (!procPool.allTrue(fabs(delta) < testThreshold_))
            return failed;
    }

    return success;
}
