// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "analyser/dataNormaliser1D.h"
#include "analyser/siteSelector.h"
#include "io/export/data1D.h"
#include "main/dissolve.h"
#include "math/histogram1D.h"
#include "module/context.h"
#include "modules/intraDistance/intraDistance.h"

// Run main processing
Module::ExecutionResult IntraDistanceModule::process(ModuleContext &moduleContext)
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

    // Calculate rAB
    auto [histAB, status] = processingData.realiseIf<Histogram1D>("Histo-AB", name(), GenericItem::InRestartFileFlag);
    if (status == GenericItem::ItemStatus::Created)
        histAB.initialise(distanceRange_.x, distanceRange_.y, distanceRange_.z);
    histAB.zeroBins();

    for (const auto &[siteA, indexA] : a.sites())
    {
        for (const auto &[siteB, indexB] : b.sites())
        {
            if (siteB->molecule() != siteA->molecule())
                continue;
            if (siteB == siteA)
                continue;
            histAB.bin(targetConfiguration_->box()->minimumDistance(siteA->origin(), siteB->origin()));
        }
    }

    // Accumulate histogram
    histAB.accumulate();

    // Distance(A-B)
    auto &dataNormalisedHisto = processingData.realise<Data1D>("NormalisedHistogram", name(), GenericItem::InRestartFileFlag);
    dataNormalisedHisto = histAB.accumulatedData();

    // Normalise
    DataNormaliser1D histogramNormaliser(dataNormalisedHisto);
    // Normalise by value
    histogramNormaliser.normaliseByValue();

    // Save data?
    if (exportFileAndFormat_.hasFilename())
    {
        if (moduleContext.processPool().isMaster())
        {
            if (exportFileAndFormat_.exportData(dataNormalisedHisto))
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
