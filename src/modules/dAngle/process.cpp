// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "analyser/dataNormaliser1D.h"
#include "analyser/dataNormaliser2D.h"
#include "analyser/siteSelector.h"
#include "base/sysFunc.h"
#include "main/dissolve.h"
#include "math/histogram1D.h"
#include "math/histogram2D.h"
#include "module/context.h"
#include "modules/dAngle/dAngle.h"

// Run main processing
Module::ExecutionResult DAngleModule::process(ModuleContext &moduleContext)
{
    // Check for Configuration target
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

    auto [rBC, rABStatus] = processingData.realiseIf<Histogram1D>("rBC", name(), GenericItem::InRestartFileFlag);
    if (rABStatus == GenericItem::ItemStatus::Created)
        rBC.initialise(distanceRange_.x, distanceRange_.y, distanceRange_.z);

    auto [aABC, aABCStatus] = processingData.realiseIf<Histogram1D>("aABC", name(), GenericItem::InRestartFileFlag);
    if (aABCStatus == GenericItem::ItemStatus::Created)
        aABC.initialise(angleRange_.x, angleRange_.y, angleRange_.z);

    auto [dAngle, dAngleStatus] = processingData.realiseIf<Histogram2D>("dAngle", name(), GenericItem::InRestartFileFlag);
    if (dAngleStatus == GenericItem::ItemStatus::Created)
        dAngle.initialise(distanceRange_.x, distanceRange_.y, distanceRange_.z, angleRange_.x, angleRange_.y, angleRange_.z);

    dAngle.zeroBins();
    rBC.zeroBins();
    aABC.zeroBins();

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

            if (siteB->molecule() != siteA->molecule())
                continue;
            nBCumulative++;
            nCSelections++;
            nBAvailable++;
            for (const auto &[siteC, indexC] : c.sites())
            {

                if (excludeSameMolecule_ && (siteC->molecule() == siteB->molecule()))
                    continue;

                nCCumulative++;
                nCAvailable++;

                auto distanceBC = targetConfiguration_->box()->minimumDistance(siteB->origin(), siteC->origin());
                auto angle = targetConfiguration_->box()->angleInDegrees(siteA->origin(), siteB->origin(), siteC->origin());
                if (symmetric_ && angle > 90.0)
                    angle = 180.0 - angle;

                if (dAngle.bin(distanceBC, angle))
                {
                    rBC.bin(distanceBC);
                    aABC.bin(angle);
                }
            }
        }
    }

    dAngle.accumulate();
    rBC.accumulate();
    aABC.accumulate();

    auto &rBCNormalised = processingData.realise<Data1D>("RDF(BC)", name(), GenericItem::InRestartFileFlag);
    rBCNormalised = rBC.accumulatedData();
    DataNormaliser1D rBCNormaliser(rBCNormalised);
    rBCNormaliser.normaliseDivide(double(nACumulative) / nASelections);
    rBCNormaliser.normaliseDivide(double(nBCumulative) / nBSelections);
    rBCNormaliser.normaliseDivide((double(nCAvailable) / nCSelections) / targetConfiguration_->box()->volume());
    rBCNormaliser.normaliseBySphericalShell();

    auto &aABCNormalised = processingData.realise<Data1D>("Angle(ABC)", name(), GenericItem::InRestartFileFlag);
    aABCNormalised = aABC.accumulatedData();
    DataNormaliser1D aABCNormaliser(aABCNormalised);
    aABCNormaliser.normalise([](const auto &x, const auto &value) { return value / sin(x / DEGRAD); });
    aABCNormaliser.normaliseTo();

    auto &dAngleNormalised = processingData.realise<Data2D>("DAngle(A-BC)", name(), GenericItem::InRestartFileFlag);
    dAngleNormalised = dAngle.accumulatedData();
    DataNormaliser2D dAngleNormaliser(dAngleNormalised);
    dAngleNormaliser.normaliseByExpression(fmt::format("{} * value/sin(toRad(y))/sin(toRad(yDelta))", symmetric_ ? 1.0 : 2.0));
    dAngleNormaliser.normaliseDivide(double(nACumulative) / nASelections);
    dAngleNormaliser.normaliseDivide((double(nBAvailable) / nBSelections) / targetConfiguration_->box()->volume());
    dAngleNormaliser.normaliseBySphericalShell();

    return ExecutionResult::Success;
}
