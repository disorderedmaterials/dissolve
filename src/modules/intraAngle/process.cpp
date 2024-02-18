// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "analyser/dataNormaliser1D.h"
#include "analyser/siteSelector.h"
#include "expression/variable.h"
#include "main/dissolve.h"
#include "math/histogram1D.h"
#include "module/context.h"
#include "modules/intraAngle/intraAngle.h"

// Run main processing
Module::ExecutionResult IntraAngleModule::process(ModuleContext &moduleContext)
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

    // Select site C
    SiteSelector c(targetConfiguration_, c_);

    auto [hist, status] = processingData.realiseIf<Histogram1D>("Histo", name(), GenericItem::InRestartFileFlag);
    if (status == GenericItem::ItemStatus::Created)
        hist.initialise(angleRange_.x, angleRange_.y, angleRange_.z);

    hist.zeroBins();

    for (const auto &[siteA, indexA] : a.sites())
    {
        for (const auto &[siteB, indexB] : b.sites())
        {

            if (siteB->molecule() != siteA->molecule())
                continue;

            if (siteB == siteA)
                continue;

            if (!rangeAB_.contains(targetConfiguration_->box()->minimumDistance(siteA->origin(), siteB->origin())))
                continue;

            for (const auto &[siteC, indexC] : c.sites())
            {

                if (siteC->molecule() != siteA->molecule())
                    continue;

                if (siteC == siteA)
                    continue;

                if (siteC == siteB)
                    continue;

                if (!rangeBC_.contains(targetConfiguration_->box()->minimumDistance(siteB->origin(), siteC->origin())))
                    continue;

                auto angle = targetConfiguration_->box()->angleInDegrees(siteA->origin(), siteB->origin(), siteC->origin());
                if (symmetric_ && angle > 90.0)
                    angle = 180.0 - angle;
                hist.bin(angle);
            }
        }
    }
    hist.accumulate();

    auto &dataNormalisedHisto = processingData.realise<Data1D>("Angle(ABC)", name(), GenericItem::InRestartFileFlag);
    dataNormalisedHisto = hist.accumulatedData();
    DataNormaliser1D normaliser(dataNormalisedHisto);
    normaliser.normaliseByExpression("value/sin(toRad(x))");
    normaliser.normaliseByValue();

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
