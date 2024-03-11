// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "analyser/dataExporter.h"
#include "analyser/dataOperator1D.h"
#include "analyser/siteSelector.h"
#include "base/sysFunc.h"
#include "io/export/data1D.h"
#include "main/dissolve.h"
#include "math/integerHistogram1D.h"
#include "module/context.h"
#include "modules/histogramCN/histogramCN.h"

// Run main processing
Module::ExecutionResult HistogramCNModule::process(ModuleContext &moduleContext)
{
    // Check for zero Configuration targets
    if (!targetConfiguration_)
    {
        Messenger::error("No configuration target set for module '{}'.\n", name());
        return ExecutionResult::Failed;
    }

    auto &processingData = moduleContext.dissolve().processingModuleData();

    // Select site A
    SiteSelector a(targetConfiguration_, a_);

    // Select site B
    SiteSelector b(targetConfiguration_, b_);

    // Coordination Histogram
    auto [hist, status] = processingData.realiseIf<IntegerHistogram1D>("Bins", name(), GenericItem::InRestartFileFlag);
    if (status == GenericItem::ItemStatus::Created)
        hist.initialise();
    hist.zeroBins();

    for (const auto &[siteA, indexA] : a.sites())
    {
        auto nSelected = 0;
        for (const auto &[siteB, indexB] : b.sites())
        {
            if (excludeSameMolecule_ && (siteB->molecule() == siteA->molecule()))
                continue;
            if (siteB == siteA)
                continue;
            if (!distanceRange_.contains(targetConfiguration_->box()->minimumDistance(siteB->origin(), siteA->origin())))
                continue;
            ++nSelected;
        }
        hist.bin(nSelected);
    }

    // Accumulate histogram
    hist.accumulate();

    // CN
    auto &dataCN = processingData.realise<Data1D>("Histogram", name(), GenericItem::InRestartFileFlag);
    dataCN = hist.accumulatedData();

    // Normalise CN
    DataOperator1D normaliserCN(dataCN);
    // Normalise by value
    normaliserCN.normaliseTo();

    // Save CN data?
    if (!DataExporter<Data1D, Data1DExportFileFormat>::exportData(dataCN, exportFileAndFormat_, moduleContext.processPool()))
        return ExecutionResult::Failed;

    return ExecutionResult::Success;
}
