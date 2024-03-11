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

    // RDF(B-C)
    auto [rBC, rABStatus] = processingData.realiseIf<Histogram1D>("rBC", name(), GenericItem::InRestartFileFlag);
    if (rABStatus == GenericItem::ItemStatus::Created)
        rBC.initialise(distanceRange_.x, distanceRange_.y, distanceRange_.z);

    // Angle(A-B-C)
    auto [aABC, aABCStatus] = processingData.realiseIf<Histogram1D>("aABC", name(), GenericItem::InRestartFileFlag);
    if (aABCStatus == GenericItem::ItemStatus::Created)
        aABC.initialise(angleRange_.x, angleRange_.y, angleRange_.z);

    // DAngle(A-BC)
    auto [dAngle, dAngleStatus] = processingData.realiseIf<Histogram2D>("dAngle", name(), GenericItem::InRestartFileFlag);
    if (dAngleStatus == GenericItem::ItemStatus::Created)
        dAngle.initialise(distanceRange_.x, distanceRange_.y, distanceRange_.z, angleRange_.x, angleRange_.y, angleRange_.z);

    dAngle.zeroBins();
    rBC.zeroBins();
    aABC.zeroBins();

    // Site statistics
    auto nASelections = 1;
    auto nAAvailable = a.sites().size(), nACumulative = a.sites().size();
    auto nBSelections = nAAvailable;
    auto nBAvailable = 0, nBCumulative = 0;
    auto nCSelections = 0, nCAvailable = 0, nCCumulative = 0;

    for (const auto &[siteA, indexA] : a.sites())
    {
        for (const auto &[siteB, indexB] : b.sites())
        {

            if (siteB->molecule() != siteA->molecule())
                continue;

            ++nBCumulative;
            ++nCSelections;
            ++nBAvailable;

            for (const auto &[siteC, indexC] : c.sites())
            {

                if (excludeSameMolecule_ && (siteC->molecule() == siteB->molecule()))
                    continue;

                ++nCCumulative;
                ++nCAvailable;

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

    // Accumulate histograms
    dAngle.accumulate();
    rBC.accumulate();
    aABC.accumulate();

    // RDF(B-C)
    auto &rBCNormalised = processingData.realise<Data1D>("RDF(BC)", name(), GenericItem::InRestartFileFlag);
    rBCNormalised = rBC.accumulatedData();
    DataOperator1D rBCNormaliser(rBCNormalised);
    // Normalise by A site population
    rBCNormaliser.divide(double(nACumulative) / nASelections);
    // Normalise by B site population
    rBCNormaliser.divide(double(nBCumulative) / nBSelections);
    // Normalise by C site population density
    rBCNormaliser.divide((double(nCAvailable) / nCSelections) / targetConfiguration_->box()->volume());
    // Normalise by spherical shell
    rBCNormaliser.normaliseBySphericalShell();

    auto &aABCNormalised = processingData.realise<Data1D>("Angle(ABC)", name(), GenericItem::InRestartFileFlag);
    aABCNormalised = aABC.accumulatedData();
    DataOperator1D aABCNormaliser(aABCNormalised);
    // Normalise by value / sin(x)
    aABCNormaliser.operate([](const auto &x, const auto &xDelta, const auto &value) { return value / sin(x / DEGRAD); });
    // Normalise to 1.0
    aABCNormaliser.normaliseTo();

    auto &dAngleNormalised = processingData.realise<Data2D>("DAngle(A-BC)", name(), GenericItem::InRestartFileFlag);
    dAngleNormalised = dAngle.accumulatedData();
    DataOperator2D dAngleNormaliser(dAngleNormalised);
    // Normalise by value / sin(y) / sin(yDelta)
    dAngleNormaliser.operate([&](const auto &x, const auto &xDelta, const auto &y, const auto &yDelta, const auto &value)
                             { return (symmetric_ ? value : value * 2.0) / sin(y / DEGRAD) / sin(yDelta / DEGRAD); });
    // Normalise by A site population
    dAngleNormaliser.divide(double(nACumulative) / nASelections);
    // Normalise by B site population density
    dAngleNormaliser.divide((double(nBAvailable) / nBSelections) / targetConfiguration_->box()->volume());
    // Normalise by spherical shell
    dAngleNormaliser.normaliseBySphericalShell();

    // Save RDF(A-B) data?
    if (!DataExporter<Data1D, Data1DExportFileFormat>::exportData(rBCNormalised, exportFileAndFormatRDF_,
                                                                  moduleContext.processPool()))
        return ExecutionResult::Failed;

    // Save Angle(A-B-C) data?
    if (!DataExporter<Data1D, Data1DExportFileFormat>::exportData(aABCNormalised, exportFileAndFormatAngle_,
                                                                  moduleContext.processPool()))
        return ExecutionResult::Failed;

    // Save DAngle(A-(B-C)) data?
    if (!DataExporter<Data2D, Data2DExportFileFormat>::exportData(dAngleNormalised, exportFileAndFormatDAngle_,
                                                                  moduleContext.processPool()))
        return ExecutionResult::Failed;

    return ExecutionResult::Success;
}
