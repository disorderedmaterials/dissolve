// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "base/sysfunc.h"
#include "io/export/data1d.h"
#include "main/dissolve.h"
#include "modules/intradistance/intradistance.h"
#include "procedure/nodes/collect1d.h"
#include "procedure/nodes/operatesitepopulationnormalise.h"
#include "procedure/nodes/select.h"
#include "procedure/nodes/sequence.h"
#include "procedure/nodes/sum1d.h"

// Run main processing
bool IntraDistanceModule::process(Dissolve &dissolve, const ProcessPool &procPool)
{
    // Check for zero Configuration targets
    if (!targetConfiguration_)
        return Messenger::error("No configuration target set for module '{}'.\n", name());

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
        return Messenger::error("CalculateRDF experienced problems with its analysis.\n");

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
                    return Messenger::error("Failed to write instantaneous coordination number data for range A.\n");
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
                    return Messenger::error("Failed to write instantaneous coordination number data for range B.\n");
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
                    return Messenger::error("Failed to write instantaneous coordination number data for range C.\n");
            }
        }
    }

    // Test?
    if (testRangeA_)
    {
        // Is range A enabled?
        if (!isRangeEnabled(0))
            return Messenger::error("Test coordination number for range B supplied, but calculation for that range "
                                    "is not active.\n");

        const auto delta = testRangeA_.value() - sumCN_->sum(0);

        Messenger::print("Reference coordination number delta with correct value for range A is {:15.9e} and is {} "
                         "(threshold is {:10.3e})\n",
                         delta, fabs(delta) < testThreshold_ ? "OK" : "NOT OK", testThreshold_);
        if (!procPool.allTrue(fabs(delta) < testThreshold_))
            return false;
    }
    if (testRangeB_)
    {
        // Is range B enabled?
        if (!isRangeEnabled(1))
            return Messenger::error("Test coordination number for range B supplied, but calculation for that range "
                                    "is not active.\n");

        const auto delta = testRangeB_.value() - sumCN_->sum(1);

        Messenger::print("Reference coordination number delta with correct value for range B is {:15.9e} and is {} "
                         "(threshold is {:10.3e})\n",
                         delta, fabs(delta) < testThreshold_ ? "OK" : "NOT OK", testThreshold_);
        if (!procPool.allTrue(fabs(delta) < testThreshold_))
            return false;
    }
    if (testRangeC_)
    {
        // Is range B enabled?
        if (!isRangeEnabled(2))
            return Messenger::error("Test coordination number for range C supplied, but calculation for that range "
                                    "is not active.\n");

        const auto delta = testRangeC_.value() - sumCN_->sum(2);

        Messenger::print("Reference coordination number delta with correct value for range C is {:15.9e} and is {} "
                         "(threshold is {:10.3e})\n",
                         delta, fabs(delta) < testThreshold_ ? "OK" : "NOT OK", testThreshold_);
        if (!procPool.allTrue(fabs(delta) < testThreshold_))
            return false;
    }

    return true;
}
