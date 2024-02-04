// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "analyser/dataNormaliser1D.h"
#include "base/sysFunc.h"
#include "io/export/data1D.h"
#include "main/dissolve.h"
#include "module/context.h"
#include "modules/histogramCN/histogramCN.h"
#include "procedure/nodes/integerCollect1D.h"
#include "procedure/nodes/operateSitePopulationNormalise.h"
#include "procedure/nodes/select.h"
#include "procedure/nodes/sequence.h"
#include "procedure/nodes/sum1D.h"

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
    hist.accumulate();

    auto &cn = processingData.realise<Data1D>("Histogram", name(), GenericItem::InRestartFileFlag);
    cn = hist.accumulatedData();
    DataNormaliser1D cnNormaliser(cn);
    cnNormaliser.normaliseByValue();

    return ExecutionResult::Success;
}
