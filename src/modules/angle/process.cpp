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

    auto &processingData = moduleContext.dissolve().processingData();

    // Select site A
    SiteSelector a(targetConfiguration_, a_);

    // Select site B
    SiteSelector b(targetConfiguration_, b_);

    // Select site C
    SiteSelector c(targetConfiguration_, c_);

    auto [rAB, status] = processingData.realiseIf<Histogram1D>("rAB", name(), GenericItem::InRestartFileFlag);
    if (status == GenericItem::ItemStatus::Created)
        rAB.initialise(rangeAB_.x, rangeAB_.y, rangeAB_.z);
    auto [rBC, status] = processingData.realiseIf<Histogram1D>("rBC", name(), GenericItem::InRestartFileFlag);
    if (status == GenericItem::ItemStatus::Created)
        rBC.initialise(rangeBC_.x, rangeBC_.y, rangeBC_.z);

    auto [aABC, status] = processingData.realiseIf<Histogram1D>("aABC", name(), GenericItem::InRestartFileFlag);
    if (status == GenericItem::ItemStatus::Created)
        aABC.initialise(angleRange_.x, angleRange_.y, angleRange_.z);

    auto [daABc, status] = processingData.realiseIf<Histogram2D>("daABc", name(), GenericItem::InRestartFileFlag);
    if (status == GenericItem::ItemStatus::Created)
        daABc.initialise(rangeAB_.x, rangeAB_.y, rangeAB_.z, angleRange_.x, angleRange_.y, angleRange_.z);

    auto [daaBC, status] = processingData.realiseIf<Histogram2D>("daaBC", name(), GenericItem::InRestartFileFlag);
    if (status == GenericItem::ItemStatus::Created)
        daaBC.initialise(rangeBC_.x, rangeBC_.y, rangeBC_.z, angleRange_.x, angleRange_.y, angleRange_.z);

    auto [dda, status] = processingData.realiseIf<Histogram3D>("dda", name(), GenericItem::InRestartFileFlag);
    if (status == GenericItem::ItemStatus::Created)
        dda.initialise(rangeAB_.x, rangeAB_.y, rangeAB_.z, rangeBC_.x, rangeBC_.y, rangeBC_.z, angleRange_.x, angleRange_.y,
                       angleRange_.z);

    for (const auto &siteA : a.sites())
    {

        for (const auto &siteB : b.sites())
        {

            if (excludeSameMoleculeAB_ && siteB->molecule() == siteA->molecule())
                continue;

            if (!rangeAB_.contains(targetConfiguration_->box()->minimumDistance(siteB->origin(), siteA->origin())))
                continue;

            rAB.zeroBins();
            rAB.bin(targetConfiguration_->box()->minimumDistance(siteB->origin(), siteA->origin()));
            rAB.accumulate();

            for (const auto &siteC : c.sites())
            {

                if (excludeSameMoleculeBC_ && siteC->molecule() == siteB->molecule())
                    continue;

                if (excludeSameSiteAC_ && siteC == siteA)
                    continue;

                if (!rangeBC_.contains(targetConfiguration_->box()->minimumDistance(siteC->origin(), siteB->origin())))
                    continue;

                rBC.zeroBins();
                rBC.bin(targetConfiguration_->box()->minimumDistance(siteC->origin(), siteB->origin()));
                rBC.accumulate();

                auto angle = targetConfiguration_->box()->angleInDegrees(siteA->origin(), siteB->origin(), siteC->origin());
                if (symmetric_ && angle > 90.0)
                    angle = 180.0 - angle;
                aABC.zeroBins();
                aABC.bin(angle);
                aABC.accumulate();

                daABc.zeroBins();
                daABC.bin(targetConfiguration_->box()->minimumDistance(siteB->origin(), siteA->origin()), angle);
                daABc.accumulate();

                daaBC.zeroBins();
                daaBC.bin(targetConfiguration_->box()->minimumDistance(siteC->origin(), siteB->origin()), angle);
                daaBC.accumulate();

                dda.zeroBins();
                dda.bin(targetConfiguration_->box()->minimumDistance(siteB->origin(), siteA->origin()),
                        targetConfiguration_->box()->minimumDistance(siteC->origin(), siteB->origin()), angle);
                dda.accumulate();
            }
        }
    }

    auto &normalisedAB = processingData.realise<Histogram1D>("RDF(AB)", name(), GenericItem::InRestartFileFlag);
    normalisedAB = rAB.accumulatedData();
    DataNormaliser1D normaliserAB(normalisedAB);
    normaliserAB.normaliseBySitePopulation(a.sites().size());
    normaliserAB.normaliseByNumberDensity(b.sites().size(), targetConfiguration_);
    normalisedAB.normaliseBySphericalShell();

    auto &normalisedBC = processingData.realise<Histogram1D>("RDF(BC)", name(), GenericItem::InRestartFileFlag);
    normalisedBC = rBC.accumulatedData();
    DataNormaliser1D normaliserBC(normalisedBC);
    normaliserBC.normaliseBySitePopulation(b.sites().size());
    normaliserBC.normaliseBySitePopulation(a.sites().size());
    normalierBC.normaliseByNumberDensity(c.sites().size(), targetConfiguration_);
    normalisedBC.normaliseBySphericalShell();

    auto &normalisedAngle = processingData.realise<Histogram1D>("Angle(ABC)", name(), GenericItem::InRestartFileFlag);
    normalisedAngle = aABC.accumulatedData();
    DataNormaliser1D normaliserAngle(normalisedAngle);
    normaliserAngle.normaliseByExpression("value/sin(toRad(x))");
    normaliserAngle.normaliseByValue();

    auto &normalisedDAngleAB = processingData.realise<Histogram2D>("DAngle((A-B)-C)", name(), GenericItem::InRestartFileFlag);
    normalisedDAngleAB = daABc.accumulatedData();
    DataNormaliser2D normaliserDAngleAB(normalisedDAngleAB);
    normaliserDAngleAB.normaliseByExpression(
        fmt::format("{} * value/sin(toRad(y))/sin(toRad(yDelta))", symmetric_ ? 1.0 : 2.0));
    normaliserDAngleAB.normaliseBySitePopulation(a.sites().size());
    normaliserDAngleAB.normaliseBySitePopulation(c.sites().size());
    normaliserDAngleAB.normaliseByNumberDensity(b.sites().size(), targetConfiguration_);
    normaliserDAngleAB.normaliseBySphericalShell();

    auto &normalisedDAngleBC = processingData.realise<Histogram2D>("DAngle(A-(B-C))", name(), GenericItem::InRestartFileFlag);
    normalisedDAngleBC = daaBC.accumulatedData();
    DataNormaliser2D normaliserDAngleBC(normalisedDAngleBC);
    normaliserDAngleBC.normaliseByExpression(
        fmt::format("{} * value/sin(toRad(y))/sin(toRad(yDelta))", symmetric_ ? 1.0 : 2.0));
    normaliserDAngleBC.normaliseBySitePopulation(b.sites().size());
    normaliserDAngleBC.normaliseBySitePopulation(a.sites().size());
    normaliserDAngleBC.normaliseByNumberDensity(c.sites().size(), targetConfiguration_);
    normaliserDAngleBC.normaliseBySphericalShell();

    return ExecutionResult::Success;
}
