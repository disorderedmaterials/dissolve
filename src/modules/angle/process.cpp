// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "analyser/dataNormaliser1D.h"
#include "analyser/dataNormaliser2D.h"
#include "analyser/dataNormaliser3D.h"
#include "analyser/siteSelector.h"
#include "main/dissolve.h"
#include "math/histogram1D.h"
#include "math/histogram2D.h"
#include "math/histogram3D.h"
#include "math/range.h"
#include "module/context.h"
#include "modules/angle/angle.h"

// Run main processing
Module::ExecutionResult AngleModule::process(ModuleContext &moduleContext)
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

    auto [rAB, rABStatus] = processingData.realiseIf<Histogram1D>("rAB", name(), GenericItem::InRestartFileFlag);
    if (rABStatus == GenericItem::ItemStatus::Created)
        rAB.initialise(rangeAB_.x, rangeAB_.y, rangeAB_.z);
    auto [rBC, rBCStatus] = processingData.realiseIf<Histogram1D>("rBC", name(), GenericItem::InRestartFileFlag);
    if (rBCStatus == GenericItem::ItemStatus::Created)
        rBC.initialise(rangeBC_.x, rangeBC_.y, rangeBC_.z);

    auto [aABC, aABCStatus] = processingData.realiseIf<Histogram1D>("aABC", name(), GenericItem::InRestartFileFlag);
    if (aABCStatus == GenericItem::ItemStatus::Created)
        aABC.initialise(angleRange_.x, angleRange_.y, angleRange_.z);

    auto [daABc, daABCStatus] = processingData.realiseIf<Histogram2D>("daABc", name(), GenericItem::InRestartFileFlag);
    if (daABCStatus == GenericItem::ItemStatus::Created)
        daABc.initialise(rangeAB_.x, rangeAB_.y, rangeAB_.z, angleRange_.x, angleRange_.y, angleRange_.z);

    auto [daaBC, daaBCStatus] = processingData.realiseIf<Histogram2D>("daaBC", name(), GenericItem::InRestartFileFlag);
    if (daaBCStatus == GenericItem::ItemStatus::Created)
        daaBC.initialise(rangeBC_.x, rangeBC_.y, rangeBC_.z, angleRange_.x, angleRange_.y, angleRange_.z);

    auto [dda, ddaStatus] = processingData.realiseIf<Histogram3D>("dda", name(), GenericItem::InRestartFileFlag);
    if (ddaStatus == GenericItem::ItemStatus::Created)
        dda.initialise(rangeAB_.x, rangeAB_.y, rangeAB_.z, rangeBC_.x, rangeBC_.y, rangeBC_.z, angleRange_.x, angleRange_.y,
                       angleRange_.z);

    rAB.zeroBins();
    rBC.zeroBins();
    aABC.zeroBins();
    daABc.zeroBins();
    daaBC.zeroBins();
    dda.zeroBins();

    auto nACumulative = 0;
    auto nBCumulative = 0;
    auto nCCumulative = 0;
    auto nAAvailable = 0;
    auto nBAvailable = 0;
    auto nCAvailable = 0;
    auto nASelections = 1;
    auto nBSelections = 0;
    auto nCSelections = 0;

    for (const auto &[siteA, indexA] : a.sites())
    {
        nACumulative++;
        nBSelections++;
        nAAvailable++;
        for (const auto &[siteB, indexB] : b.sites())
        {

            if (excludeSameMoleculeAB_ && siteB->molecule() == siteA->molecule())
                continue;

            auto distAB = targetConfiguration_->box()->minimumDistance(siteB->origin(), siteA->origin());

            nBAvailable++;

            if (!Range(rangeAB_.x, rangeAB_.y)
                     .contains(targetConfiguration_->box()->minimumDistance(siteB->origin(), siteA->origin())))
                continue;

            rAB.bin(distAB);

            nBCumulative++;
            nCSelections++;

            for (const auto &[siteC, indexC] : c.sites())
            {

                if (excludeSameMoleculeBC_ && siteC->molecule() == siteB->molecule())
                    continue;

                if (excludeSameSiteAC_ && siteC == siteA)
                    continue;

                nCAvailable++;

                if (!Range(rangeBC_.x, rangeBC_.y)
                         .contains(targetConfiguration_->box()->minimumDistance(siteC->origin(), siteB->origin())))
                    continue;

                nCCumulative++;

                auto distBC = targetConfiguration_->box()->minimumDistance(siteC->origin(), siteB->origin());

                rBC.bin(distBC);

                auto angle = targetConfiguration_->box()->angleInDegrees(siteA->origin(), siteB->origin(), siteC->origin());
                if (symmetric_ && angle > 90.0)
                    angle = 180.0 - angle;
                aABC.bin(angle);

                daABc.bin(distAB, angle);

                daaBC.bin(distBC, angle);

                dda.bin(distAB, distBC, angle);
            }
        }
    }

    rAB.accumulate();
    rBC.accumulate();
    aABC.accumulate();
    daABc.accumulate();
    daaBC.accumulate();
    dda.accumulate();

    auto &normalisedAB = processingData.realise<Data1D>("RDF(AB)", name(), GenericItem::InRestartFileFlag);
    normalisedAB = rAB.accumulatedData();
    DataNormaliser1D normaliserAB(normalisedAB);
    normaliserAB.normaliseDivide(double(nACumulative) / nASelections);
    normaliserAB.normaliseDivide((double(nBCumulative) / nBSelections) / targetConfiguration_->box()->volume());
    normaliserAB.normaliseBySphericalShell();

    auto &normalisedBC = processingData.realise<Data1D>("RDF(BC)", name(), GenericItem::InRestartFileFlag);
    normalisedBC = rBC.accumulatedData();
    DataNormaliser1D normaliserBC(normalisedBC);
    normaliserBC.normaliseDivide(double(nACumulative) / nASelections);
    normaliserBC.normaliseDivide(double(nBCumulative) / nBSelections);
    normaliserBC.normaliseDivide((double(nCAvailable) / nCSelections) / targetConfiguration_->box()->volume());
    normaliserBC.normaliseBySphericalShell();

    auto &normalisedAngle = processingData.realise<Data1D>("Angle(ABC)", name(), GenericItem::InRestartFileFlag);
    normalisedAngle = aABC.accumulatedData();
    DataNormaliser1D normaliserAngle(normalisedAngle);
    normaliserAngle.normalise([](const auto &x, , const auto &xDelta, const auto &value) { return value / sin(x / DEGRAD); });
    normaliserAngle.normaliseTo();

    auto &normalisedDAngleAB = processingData.realise<Data2D>("DAngle((A-B)-C)", name(), GenericItem::InRestartFileFlag);
    normalisedDAngleAB = daABc.accumulatedData();
    DataNormaliser2D normaliserDAngleAB(normalisedDAngleAB);
    normaliserDAngleAB.normalise([](const auto &x, const auto &xDelta, const auto &y, const auto &yDelta, const auto &value)
                                 { return (symmetric_ ? value : value * 2.0) / sin(y / DEGRAD) / sin(yDelta / DEGRAD); });
    normaliserDAngleAB.normaliseDivide(double(nACumulative) / nASelections);
    normaliserDAngleAB.normaliseDivide(double(nCCumulative) / nCSelections);
    normaliserDAngleAB.normaliseDivide((double(nBAvailable) / nBSelections) / targetConfiguration_->box()->volume());
    normaliserDAngleAB.normaliseBySphericalShell();

    auto &normalisedDAngleBC = processingData.realise<Data2D>("DAngle(A-(B-C))", name(), GenericItem::InRestartFileFlag);
    normalisedDAngleBC = daaBC.accumulatedData();
    DataNormaliser2D normaliserDAngleBC(normalisedDAngleBC);
    normaliserDAngleBC.normalise([](const auto &x, const auto &xDelta, const auto &y, const auto &yDelta, const auto &value)
                                 { return (symmetric_ ? value : value * 2.0) / sin(y / DEGRAD) / sin(yDelta / DEGRAD); });
    normaliserDAngleBC.normaliseDivide(double(nACumulative) / nASelections);
    normaliserDAngleBC.normaliseDivide(double(nBCumulative) / nBSelections);
    normaliserDAngleBC.normaliseDivide((double(nCAvailable) / nCSelections) / targetConfiguration_->box()->volume());
    normaliserDAngleBC.normaliseBySphericalShell();

    return ExecutionResult::Success;
}
