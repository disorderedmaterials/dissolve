// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include <tuple>
#include <string>
#include "analyser/dataExporter.h"
#include "analyser/dataOperator1D.h"
#include "analyser/dataOperator2D.h"
#include "analyser/dataOperator3D.h"
#include "analyser/siteSelector.h"
#include "main/dissolve.h"
#include "math/histogram1D.h"
#include "math/histogram2D.h"
#include "math/histogram3D.h"
#include "math/range.h"
#include "module/context.h"
#include "modules/angle/angle.h"
#include "templates/algorithms.h"
#include "templates/combinable.h"

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
    auto [dAngleABC, dAngleABCStatus] =
        processingData.realiseIf<Histogram3D>("DAngleABC", name(), GenericItem::InRestartFileFlag);
    if (dAngleABCStatus == GenericItem::ItemStatus::Created)
        dAngleABC.initialise(rangeAB_.x, rangeAB_.y, rangeAB_.z, rangeBC_.x, rangeBC_.y, rangeBC_.z, angleRange_.x,
                             angleRange_.y, angleRange_.z);

    auto initialiser = [](auto &hist)
    {
        hist.zeroBins();
        return hist;
    };

    std::map<std::string, dissolve::CombinableValue<Histogram1D>> combinableHistogramMap;
    for (auto &tup : {std::tuple<std::string, Histogram1D>{"rAB", rAB}, 
                    std::tuple<std::string, Histogram1D>{"rBC", rBC},
                    std::tuple<std::string, Histogram1D>{"aABC", aABC}})
    {
        combinableHistogramMap[std::get<0>(tup)] = dissolve::CombinableValue<Histogram1D>(initialiser(std::get<1>(tup)));
    }

    std::map<std::string, dissolve::CombinableValue<Histogram2D>> combinableHistogram1DMap;
    for (auto &tup : {std::tuple<std::string, Histogram2D>{"DAngleAB", dAngleAB},
                    std::tuple<std::string, Histogram2D>{"DAngleBC", dAngleBC}})
    {
        combinableHistogram2DMap[std::get<0>(tup)] = dissolve::CombinableValue<Histogram2D>(initialiser(std::get<1>(tup)));
    }

    auto combinableHistogram3D = dissolve::CombinableValue<Histogram3D>(initialiser(dAngleABC));

    std::map<std::string, int> siteCounterMap{
        {"nAAvailable", a.sites().size()}, {"nACumulative", a.sites().size()},
        {"nASelections", 1}, {"nBAvailable", 0}, {"nBCumulative", 0}, {"nBSelections", 0},
        {"nCAvailable", 0}, {"nCAvailable", 0}, {"nCCumulative", 0}, {"nCSelections", 0}  
    };

    auto unaryOp =
        [this, &b, &combinableHistogram1DMap, &combinableHistogram2DMap, &combinableHistogram3D, &siteCounterMap](const auto &pair)
    {
        auto rAB = combinableHistogram1DMap["rAB"].local(), rBC = combinableHistogram1DMap["rBC"].local(), aABC = combinableHistogram1DMap["aABC"].local();
        auto dAngleAB = combinableHistogram2DMap["DAngleAB"].local(), dAngleBC = combinableHistogram2DMap["DAngleBC"].local()
        auto dAngleABC = combinableHistogram3D.local();
        
        const auto &[siteA, indexA] = pair;
        
        ++siteCounterMap["nBSelections"];
        for (const auto &[siteB, indexB] : b.sites())
        {

            if (excludeSameMoleculeAB_ && (siteB->molecule() == siteA->molecule()))
                continue;

            auto distAB = targetConfiguration_->box()->minimumDistance(siteA->origin(), siteB->origin());

            ++siteCounterMap["nBAvailable"];

            if (!Range(rangeAB_.x, rangeAB_.y).contains(distAB))
                continue;

            rAB.bin(distAB);

            ++siteCounterMap["nBCumulative"];
            ++siteCounterMap["nCSelections"];

            for (const auto &[siteC, indexC] : c.sites())
            {

                if (excludeSameMoleculeBC_ && (siteC->molecule() == siteB->molecule()))
                    continue;

                if (excludeSameSiteAC_ && (siteC == siteA))
                    continue;

                ++siteCounterMap["nCAvailable"];

                auto distBC = targetConfiguration_->box()->minimumDistance(siteB->origin(), siteC->origin());

                if (!Range(rangeBC_.x, rangeBC_.y).contains(distBC))
                    continue;

                ++siteCounterMap["nCCumulative"];

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
    };

    dissolve::for_each(std::execution::par, a.sites.begin(), a.sites.end(), unaryOp);

    for (const auto &hist : combinableHistogramMap)
    {
        hist.finalize();
        hist.accumulate();
    }

    // RDF(A-B)
    auto &normalisedAB = processingData.realise<Data1D>("RDF(AB)", name(), GenericItem::InRestartFileFlag);
    normalisedAB = rAB.accumulatedData();
    DataOperator1D normaliserAB(normalisedAB);
    // Normalise by A site population
    normaliserAB.divide(double(nACumulative) / nASelections);
    // Normalise by B site population density
    normaliserAB.divide((double(nBCumulative) / nBSelections) / targetConfiguration_->box()->volume());
    // Normalise by spherical shell
    normaliserAB.normaliseBySphericalShell();

    // RDF(B-C)
    auto &normalisedBC = processingData.realise<Data1D>("RDF(BC)", name(), GenericItem::InRestartFileFlag);
    normalisedBC = rBC.accumulatedData();
    DataOperator1D normaliserBC(normalisedBC);
    // Normalise by A site population
    normaliserBC.divide(double(nACumulative) / nASelections);
    // Normalise by B site population
    normaliserBC.divide(double(nBCumulative) / nBSelections);
    // Normalise by C site population density
    normaliserBC.divide((double(nCAvailable) / nCSelections) / targetConfiguration_->box()->volume());
    // Normalise by spherical shell
    normaliserBC.normaliseBySphericalShell();

    // Angle(A-B-C)
    auto &normalisedAngle = processingData.realise<Data1D>("Angle(ABC)", name(), GenericItem::InRestartFileFlag);
    normalisedAngle = aABC.accumulatedData();
    DataOperator1D normaliserAngle(normalisedAngle);
    // Normalise by value / sin(x)
    normaliserAngle.operate([](const auto &x, const auto &xDelta, const auto &value) { return value / sin(x / DEGRAD); });
    // Normalise to 1.0
    normaliserAngle.normaliseSumTo();

    // DAngle((A-B)-C)
    auto &normalisedDAngleAB = processingData.realise<Data2D>("DAngle((A-B)-C)", name(), GenericItem::InRestartFileFlag);
    normalisedDAngleAB = dAngleAB.accumulatedData();
    DataOperator2D normaliserDAngleAB(normalisedDAngleAB);
    // Normalise by value / sin(y) / sin(yDelta)
    normaliserDAngleAB.operate([&](const auto &x, const auto &xDelta, const auto &y, const auto &yDelta, const auto &value)
                               { return (symmetric_ ? value : value * 2.0) / sin(y / DEGRAD) / sin(yDelta / DEGRAD); });
    // Normalise by A site population
    normaliserDAngleAB.divide(double(nACumulative) / nASelections);
    // Normalise by C site population
    normaliserDAngleAB.divide(double(nCCumulative) / nCSelections);
    // Normalise by B site population density
    normaliserDAngleAB.divide((double(nBAvailable) / nBSelections) / targetConfiguration_->box()->volume());
    // Normalise by spherical shell
    normaliserDAngleAB.normaliseBySphericalShell();

    // DAngle(A-(B-C))
    auto &normalisedDAngleBC = processingData.realise<Data2D>("DAngle(A-(B-C))", name(), GenericItem::InRestartFileFlag);
    normalisedDAngleBC = dAngleBC.accumulatedData();
    DataOperator2D normaliserDAngleBC(normalisedDAngleBC);
    // Normalise by value / sin(y) / sin(yDelta)
    normaliserDAngleBC.operate([&](const auto &x, const auto &xDelta, const auto &y, const auto &yDelta, const auto &value)
                               { return (symmetric_ ? value : value * 2.0) / sin(y / DEGRAD) / sin(yDelta / DEGRAD); });
    // Normalise by A site population
    normaliserDAngleBC.divide(double(nACumulative) / nASelections);
    // Normalise by B site population
    normaliserDAngleBC.divide(double(nBCumulative) / nBSelections);
    // Normalise by C site population density
    normaliserDAngleBC.divide((double(nCAvailable) / nCSelections) / targetConfiguration_->box()->volume());
    // Normalise by spherical shell
    normaliserDAngleBC.normaliseBySphericalShell();

    // Save RDF(A-B) data?
    if (!DataExporter<Data1D, Data1DExportFileFormat>::exportData(normalisedAB, exportFileAndFormatAB_,
                                                                  moduleContext.processPool()))
        return ExecutionResult::Failed;

    // Save RDF(B-C) data?
    if (!DataExporter<Data1D, Data1DExportFileFormat>::exportData(normalisedBC, exportFileAndFormatBC_,
                                                                  moduleContext.processPool()))
        return ExecutionResult::Failed;

    // Save Angle(A-B-C) data?
    if (!DataExporter<Data1D, Data1DExportFileFormat>::exportData(normalisedAngle, exportFileAndFormatAngle_,
                                                                  moduleContext.processPool()))
        return ExecutionResult::Failed;

    // Save DAngle((A-B)-C) data?
    if (!DataExporter<Data2D, Data2DExportFileFormat>::exportData(normalisedDAngleAB, exportFileAndFormatDAngleAB_,
                                                                  moduleContext.processPool()))
        return ExecutionResult::Failed;

    // Save DAngle(A-(B-C)) data?
    if (!DataExporter<Data2D, Data2DExportFileFormat>::exportData(normalisedDAngleBC, exportFileAndFormatDAngleBC_,
                                                                  moduleContext.processPool()))
        return ExecutionResult::Failed;

    return ExecutionResult::Success;
}
