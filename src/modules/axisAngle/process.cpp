// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "analyser/dataExporter.h"
#include "analyser/dataOperator1D.h"
#include "analyser/dataOperator2D.h"
#include "analyser/siteSelector.h"
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

    // RDF(A-B)
    auto [rAB, rABStatus] = processingData.realiseIf<Histogram1D>("rAB", name(), GenericItem::InRestartFileFlag);
    if (rABStatus == GenericItem::ItemStatus::Created)
        rAB.initialise(distanceRange_.x, distanceRange_.y, distanceRange_.z);

    // AxisAngle(A-B)
    auto [aAB, aABStatus] = processingData.realiseIf<Histogram1D>("aAB", name(), GenericItem::InRestartFileFlag);
    if (aABStatus == GenericItem::ItemStatus::Created)
        aAB.initialise(angleRange_.x, angleRange_.y, angleRange_.z);

    // DAxisAngle(A-B)
    auto [dAxisAngle, dAxisAngleStatus] =
        processingData.realiseIf<Histogram2D>("dAxisAngle", name(), GenericItem::InRestartFileFlag);
    if (dAxisAngleStatus == GenericItem::ItemStatus::Created)
        dAxisAngle.initialise(distanceRange_.x, distanceRange_.y, distanceRange_.z, angleRange_.x, angleRange_.y,
                              angleRange_.z);

    rAB.zeroBins();
    aAB.zeroBins();
    dAxisAngle.zeroBins();

    for (const auto &[siteA, indexA] : a.sites())
    {
        for (const auto &[siteB, indexB] : b.sites())
        {
            if (excludeSameMolecule_ && (siteA->molecule() == siteB->molecule()))
                continue;

            auto distanceAB = targetConfiguration_->box()->minimumDistance(siteA->origin(), siteB->origin());
            auto axisAngle = Box::angleInDegrees(siteA->axes().columnAsVec3(axisA_), siteB->axes().columnAsVec3(axisB_));
            if (symmetric_ && axisAngle > 90.0)
                axisAngle = 180.0 - axisAngle;

            if (dAxisAngle.bin(distanceAB, axisAngle))
            {
                rAB.bin(distanceAB);
                aAB.bin(axisAngle);
            }
        }
    }

    // Accumulate histograms
    rAB.accumulate();
    aAB.accumulate();
    dAxisAngle.accumulate();

    // RDF(A-B)
    auto &rABNormalised = processingData.realise<Data1D>("RDF(AB)", name(), GenericItem::InRestartFileFlag);
    rABNormalised = rAB.accumulatedData();
    DataOperator1D rABNormaliser(rABNormalised);

    // Normalise by A site population
    rABNormaliser.normaliseDivide(double(a.sites().size()));
    // Normalise by B site population density
    rABNormaliser.normaliseDivide(double(b.sites().size()) / targetConfiguration_->box()->volume());
    // Normalise by spherical shell
    rABNormaliser.normaliseBySphericalShell();

    // AxisAngle(A-B)
    auto &aABNormalised = processingData.realise<Data1D>("AxisAngle(AB)", name(), GenericItem::InRestartFileFlag);
    aABNormalised = aAB.accumulatedData();
    DataOperator1D aABNormaliser(aABNormalised);
    // Normalise by value / sin(x)
    aABNormaliser.normalise([](const auto &x, const auto &xDelta, const auto &value) { return value / sin(x / DEGRAD); });
    // Normalise to 1.0
    aABNormaliser.normaliseTo();

    // DAxisAngle(A-B)
    auto &dAxisAngleNormalised = processingData.realise<Data2D>("DAxisAngle", name(), GenericItem::InRestartFileFlag);
    dAxisAngleNormalised = dAxisAngle.accumulatedData();
    DataOperator2D dAxisAngleNormaliser(dAxisAngleNormalised);
    // Normalise by value / sin(y) / sin(yDelta)
    dAxisAngleNormaliser.normalise([&](const auto &x, const auto &xDelta, const auto &y, const auto &yDelta, const auto &value)
                                   { return (symmetric_ ? value : value * 2.0) / sin(y / DEGRAD) / sin(yDelta / DEGRAD); });
    // Normalise by A site population
    dAxisAngleNormaliser.normaliseDivide(double(a.sites().size()));
    // Normalise by B site population density
    dAxisAngleNormaliser.normaliseDivide(double(b.sites().size()) / targetConfiguration_->box()->volume());
    // Normalise by spherical shell
    dAxisAngleNormaliser.normaliseBySphericalShell();

    // Save RDF(A-B) data?
    if (!DataExporter<Data1D, Data1DExportFileFormat>::exportData(rABNormalised, exportFileAndFormatRDF_,
                                                                  moduleContext.processPool()))
        return ExecutionResult::Failed;

    // Save AxisAngle(A-B) data?
    if (!DataExporter<Data1D, Data1DExportFileFormat>::exportData(aABNormalised, exportFileAndFormatAxisAngle_,
                                                                  moduleContext.processPool()))
        return ExecutionResult::Failed;

    // Save DAxisAngle(A-B) data?
    if (!DataExporter<Data2D, Data2DExportFileFormat>::exportData(dAxisAngleNormalised, exportFileAndFormatDAxisAngle_,
                                                                  moduleContext.processPool()))
        return ExecutionResult::Failed;

    return ExecutionResult::Success;
}
