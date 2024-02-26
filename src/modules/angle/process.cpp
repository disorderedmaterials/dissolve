// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "analyser/dataExporter.h"
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

    // RDF(A-B)
    auto [rAB, rABStatus] = processingData.realiseIf<Histogram1D>("rAB", name(), GenericItem::InRestartFileFlag);
    if (rABStatus == GenericItem::ItemStatus::Created)
        rAB.initialise(rangeAB_.x, rangeAB_.y, rangeAB_.z);
    
    // RDF(B-C)
    auto [rBC, rBCStatus] = processingData.realiseIf<Histogram1D>("rBC", name(), GenericItem::InRestartFileFlag);
    if (rBCStatus == GenericItem::ItemStatus::Created)
        rBC.initialise(rangeBC_.x, rangeBC_.y, rangeBC_.z);

    // Angle(A-B-C)
    auto [aABC, aABCStatus] = processingData.realiseIf<Histogram1D>("aABC", name(), GenericItem::InRestartFileFlag);
    if (aABCStatus == GenericItem::ItemStatus::Created)
        aABC.initialise(angleRange_.x, angleRange_.y, angleRange_.z);

    // DAngle((A-B)-C)
    auto [dAngleAB, dAngleABStatus] = processingData.realiseIf<Histogram2D>("DAngleAB", name(), GenericItem::InRestartFileFlag);
    if (dAngleABStatus == GenericItem::ItemStatus::Created)
        dAngleAB.initialise(rangeAB_.x, rangeAB_.y, rangeAB_.z, angleRange_.x, angleRange_.y, angleRange_.z);

    // DAngle(A-(B-C))
    auto [dAngleBC, dAngleBCStatus] = processingData.realiseIf<Histogram2D>("DAngleBC", name(), GenericItem::InRestartFileFlag);
    if (dAngleBCStatus == GenericItem::ItemStatus::Created)
        dAngleBC.initialise(rangeBC_.x, rangeBC_.y, rangeBC_.z, angleRange_.x, angleRange_.y, angleRange_.z);

    // DDAngle(A-B-C)
    auto [dAngleABC, dAngleABCStatus] = processingData.realiseIf<Histogram3D>("DAngleABC", name(), GenericItem::InRestartFileFlag);
    if (dAngleABCStatus == GenericItem::ItemStatus::Created)
        dAngleABC.initialise(rangeAB_.x, rangeAB_.y, rangeAB_.z, rangeBC_.x, rangeBC_.y, rangeBC_.z, angleRange_.x, angleRange_.y,
                       angleRange_.z);

    rAB.zeroBins();
    rBC.zeroBins();
    aABC.zeroBins();
    dAngleAB.zeroBins();
    dAngleBC.zeroBins();
    dAngleABC.zeroBins();

    auto nAAvailable = a.sites().size(), nACumulative = a.sites().size();
    auto nASelections = 1;
    auto nBAvailable = 0, nBCumulative = 0, nBSelections = 0;
    auto nCAvailable = 0, nCCumulative = 0, nCSelections = 0;

    for (const auto &[siteA, indexA] : a.sites())
    {
        nBSelections++;
        for (const auto &[siteB, indexB] : b.sites())
        {

            if (excludeSameMoleculeAB_ && (siteB->molecule() == siteA->molecule()))
                continue;

            auto distAB = targetConfiguration_->box()->minimumDistance(siteA->origin(), siteB->origin());

            nBAvailable++;

            if (!Range(rangeAB_.x, rangeAB_.y).contains(distAB))
                continue;

            rAB.bin(distAB);

            nBCumulative++;
            nCSelections++;

            for (const auto &[siteC, indexC] : c.sites())
            {

                if (excludeSameMoleculeBC_ && (siteC->molecule() == siteB->molecule()))
                    continue;

                if (excludeSameSiteAC_ && (siteC == siteA))
                    continue;

                nCAvailable++;

                auto distBC = targetConfiguration_->box()->minimumDistance(siteB->origin(), siteC->origin());


                if (!Range(rangeBC_.x, rangeBC_.y).contains(distBC))
                    continue;

                nCCumulative++;

                auto angle = targetConfiguration_->box()->angleInDegrees(siteA->origin(), siteB->origin(), siteC->origin());
                if (symmetric_ && angle > 90.0)
                    angle = 180.0 - angle;

                rBC.bin(distBC);
                aABC.bin(angle);
                dAngleAB.bin(distAB, angle);
                dAngleBC.bin(distBC, angle);
                dAngleABC.bin(distAB, distBC, angle);
     
            }
        }
    }

    // Accumulate histograms
    rAB.accumulate();
    rBC.accumulate();
    aABC.accumulate();
    dAngleAB.accumulate();
    dAngleBC.accumulate();
    dAngleABC.accumulate();

    // RDF(A-B)
    auto &normalisedAB = processingData.realise<Data1D>("RDF(AB)", name(), GenericItem::InRestartFileFlag);
    normalisedAB = rAB.accumulatedData();
    DataNormaliser1D normaliserAB(normalisedAB);
    // Normalise by A site population
    normaliserAB.normaliseDivide(double(nACumulative) / nASelections);
    // Normalise by B site population density
    normaliserAB.normaliseDivide((double(nBCumulative) / nBSelections) / targetConfiguration_->box()->volume());
    // Normalise by spherical shell
    normaliserAB.normaliseBySphericalShell();

    // RDF(B-C)
    auto &normalisedBC = processingData.realise<Data1D>("RDF(BC)", name(), GenericItem::InRestartFileFlag);
    normalisedBC = rBC.accumulatedData();
    DataNormaliser1D normaliserBC(normalisedBC);
    // Normalise by A site population
    normaliserBC.normaliseDivide(double(nACumulative) / nASelections);
    // Normalise by B site population
    normaliserBC.normaliseDivide(double(nBCumulative) / nBSelections);
    // Normalise by C site population density
    normaliserBC.normaliseDivide((double(nCAvailable) / nCSelections) / targetConfiguration_->box()->volume());
    // Normalise by spherical shell
    normaliserBC.normaliseBySphericalShell();

    // Angle(A-B-C)
    auto &normalisedAngle = processingData.realise<Data1D>("Angle(ABC)", name(), GenericItem::InRestartFileFlag);
    normalisedAngle = aABC.accumulatedData();
    DataNormaliser1D normaliserAngle(normalisedAngle);
    // Normalise by value / sin(x)
    normaliserAngle.normalise([](const auto &x, const auto &xDelta, const auto &value) { return value / sin(x / DEGRAD); });
    // Normalise to 1.0
    normaliserAngle.normaliseTo();

    // DAngle((A-B)-C)
    auto &normalisedDAngleAB = processingData.realise<Data2D>("DAngle((A-B)-C)", name(), GenericItem::InRestartFileFlag);
    normalisedDAngleAB = dAngleAB.accumulatedData();
    DataNormaliser2D normaliserDAngleAB(normalisedDAngleAB);
    // Normalise by value / sin(y) / sin(yDelta)
    normaliserDAngleAB.normalise([&](const auto &x, const auto &xDelta, const auto &y, const auto &yDelta, const auto &value)
                                 { return (symmetric_ ? value : value * 2.0) / sin(y / DEGRAD) / sin(yDelta / DEGRAD); });
    // Normalise by A site population
    normaliserDAngleAB.normaliseDivide(double(nACumulative) / nASelections);
    // Normalise by C site population
    normaliserDAngleAB.normaliseDivide(double(nCCumulative) / nCSelections);
    // Normalise by B site population density
    normaliserDAngleAB.normaliseDivide((double(nBAvailable) / nBSelections) / targetConfiguration_->box()->volume());
    // Normalise by spherical shell
    normaliserDAngleAB.normaliseBySphericalShell();

    // DAngle(A-(B-C))
    auto &normalisedDAngleBC = processingData.realise<Data2D>("DAngle(A-(B-C))", name(), GenericItem::InRestartFileFlag);
    normalisedDAngleBC = dAngleBC.accumulatedData();
    DataNormaliser2D normaliserDAngleBC(normalisedDAngleBC);
    // Normalise by value / sin(y) / sin(yDelta)
    normaliserDAngleBC.normalise([&](const auto &x, const auto &xDelta, const auto &y, const auto &yDelta, const auto &value)
                                 { return (symmetric_ ? value : value * 2.0) / sin(y / DEGRAD) / sin(yDelta / DEGRAD); });
    // Normalise by A site population
    normaliserDAngleBC.normaliseDivide(double(nACumulative) / nASelections);
    // Normalise by B site population
    normaliserDAngleBC.normaliseDivide(double(nBCumulative) / nBSelections);
    // Normalise by C site population density
    normaliserDAngleBC.normaliseDivide((double(nCAvailable) / nCSelections) / targetConfiguration_->box()->volume());
    // Normalise by spherical shell
    normaliserDAngleBC.normaliseBySphericalShell();


    // Save RDF(A-B) data?
    if (!DataExporter<Data1D, Data1DExportFileFormat>::exportData(normalisedAB, exportFileAndFormatAB_, moduleContext.processPool()))
        return ExecutionResult::Failed;
    
    // Save RDF(B-C) data?
    if (!DataExporter<Data1D, Data1DExportFileFormat>::exportData(normalisedBC, exportFileAndFormatBC_, moduleContext.processPool()))
        return ExecutionResult::Failed;

    // Save Angle(A-B-C) data?
    if (!DataExporter<Data1D, Data1DExportFileFormat>::exportData(normalisedAngle, exportFileAndFormatAngle_, moduleContext.processPool()))
        return ExecutionResult::Failed;
    
    // Save DAngle((A-B)-C) data?
    if (!DataExporter<Data2D, Data2DExportFileFormat>::exportData(normalisedDAngleAB, exportFileAndFormatDAngleAB_, moduleContext.processPool()))
        return ExecutionResult::Failed;

    // Save DAngle(A-(B-C)) data?
    if (!DataExporter<Data2D, Data2DExportFileFormat>::exportData(normalisedDAngleBC, exportFileAndFormatDAngleBC_, moduleContext.processPool()))
        return ExecutionResult::Failed;

    return ExecutionResult::Success;
}
