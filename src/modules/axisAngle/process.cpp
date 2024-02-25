// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "analyser/dataNormaliser1D.h"
#include "analyser/dataNormaliser2D.h"
#include "base/sysFunc.h"
#include "main/dissolve.h"
#include "math/histogram1D.h"
#include "math/histogram2D.h"
#include "module/context.h"
#include "modules/axisAngle/axisAngle.h"

// Run main processing
Module::ExecutionResult AxisAngleModule::process(ModuleContext &moduleContext)
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

    auto [rAB, rABStatus] = processingData.realiseIf<Histogram1D>("rAB", name(), GenericItem::InRestartFileFlag);
    if (rABStatus == GenericItem::ItemStatus::Created)
        rAB.initialise(distanceRange_.x, distanceRange_.y, distanceRange_.z);

    auto [aABC, aABCStatus] = processingData.realiseIf<Histogram1D>("aABC", name(), GenericItem::InRestartFileFlag);
    if (aABCStatus == GenericItem::ItemStatus::Created)
        aABC.initialise(angleRange_.x, angleRange_.y, angleRange_.z);

    auto [dAngle, dAngleStatus] = processingData.realiseIf<Histogram2D>("dAngle", name(), GenericItem::InRestartFileFlag);
    if (dAngleStatus == GenericItem::ItemStatus::Created)
        dAngle.initialise(distanceRange_.x, distanceRange_.y, distanceRange_.z, angleRange_.x, angleRange_.y, angleRange_.z);

    rAB.zeroBins();
    aABC.zeroBins();
    dAngle.zeroBins();

    for (const auto &[siteA, indexA] : a.sites())
    {
        for (const auto &[siteB, indexB] : b.sites())
        {
            if (excludeSameMolecule_ && siteA->molecule() == siteB->molecule())
                continue;

            auto distanceAB = targetConfiguration_->box()->minimumDistance(siteA->origin(), siteB->origin());
            auto axisAngle = Box::angleInDegrees(siteA->axes().columnAsVec3(axisA_), siteB->axes().columnAsVec3(axisB_));
            if (symmetric_ && axisAngle > 90.0)
                axisAngle = 180.0 - axisAngle;

            if (dAngle.bin(distanceAB, axisAngle))
            {
                rAB.bin(distanceAB);
                aABC.bin(axisAngle);
            }
        }
    }

    rAB.accumulate();
    aABC.accumulate();
    dAngle.accumulate();

    auto &rABNormalised = processingData.realise<Data1D>("RDF(AB)", name(), GenericItem::InRestartFileFlag);
    rABNormalised = rAB.accumulatedData();
    DataNormaliser1D rABNormaliser(rABNormalised);
    rABNormaliser.normaliseDivide(double(a.sites().size()));
    rABNormaliser.normaliseDivide(double(b.sites().size()) / targetConfiguration_->box()->volume());
    rABNormaliser.normaliseBySphericalShell();

    auto &aABCNormalised = processingData.realise<Data1D>("AxisAngle(AB)", name(), GenericItem::InRestartFileFlag);
    aABCNormalised = aABC.accumulatedData();
    DataNormaliser1D aABCNormaliser(aABCNormalised);
    aABCNormaliser.normalise([](const auto &x, const auto &xDelta, const auto &value) { return value / sin(x / DEGRAD); });
    aABCNormaliser.normaliseTo();

    auto &dAngleNormalised = processingData.realise<Data2D>("DAxisAngle", name(), GenericItem::InRestartFileFlag);
    dAngleNormalised = dAngle.accumulatedData();
    DataNormaliser2D dAngleNormaliser(dAngleNormalised);
    dAngleNormaliser.normalise([&](const auto &x, const auto &xDelta, const auto &y, const auto &yDelta, const auto &value)
                               { return (symmetric_ ? value : value * 2.0) / sin(y / DEGRAD) / sin(yDelta / DEGRAD); });
    dAngleNormaliser.normaliseDivide(double(a.sites().size()));
    dAngleNormaliser.normaliseDivide(double(b.sites().size()) / targetConfiguration_->box()->volume());
    dAngleNormaliser.normaliseBySphericalShell();

    return ExecutionResult::Success;
}
