// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "analyser/dataExporter.h"
#include "analyser/dataOperator1D.h"
#include "base/sysFunc.h"
#include "io/export/data1D.h"
#include "main/dissolve.h"
#include "math/integrator.h"
#include "math/sampledDouble.h"
#include "module/context.h"
#include "modules/siteRDF/siteRDF.h"

// Run main processing
Module::ExecutionResult SiteRDFModule::process(ModuleContext &moduleContext)
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
            if (excludeSameMolecule_ && (siteB->molecule() == siteA->molecule()))
                continue;
            histAB.bin(targetConfiguration_->box()->minimumDistance(siteA->origin(), siteB->origin()));
        }
    }

    // Accumulate histogram
    histAB.accumulate();

    // RDF
    auto &dataRDF = processingData.realise<Data1D>("RDF", name(), GenericItem::InRestartFileFlag);
    dataRDF = histAB.accumulatedData();

    // Normalise
    DataOperator1D normaliserRDF(dataRDF);
    // Normalise by A site population
    normaliserRDF.divide(double(a.sites().size()));

    // Normalise by B site population density
    normaliserRDF.divide(double(b.sites().size()) / targetConfiguration_->box()->volume());

    // Normalise by spherical shell
    normaliserRDF.normaliseBySphericalShell();

    // CN
    auto &dataCN = processingData.realise<Data1D>("HistogramNorm", name(), GenericItem::InRestartFileFlag);
    dataCN = histAB.accumulatedData();

    // Normalise
    DataOperator1D normaliserCN(dataCN);
    // Normalise by A site population
    normaliserCN.divide(double(a.sites().size()));
    DataNormaliser1D normaliserRunningCN(runningCN);
    normaliserRunningCN.normaliseDivide(double(a.sites().size()));

    const std::vector<std::string> rangeNames = {"A", "B", "C"};
    for (int i = 0; i < 3; ++i)
        if (rangeEnabled_[i])
        {
            auto &sumN = processingData.realise<SampledDouble>(fmt::format("CN//{}", rangeNames[i]), name(),
                                                               GenericItem::InRestartFileFlag);
            sumN += Integrator::sum(dataCN, range_[i]);
            if (instantaneous_)
            {
                auto &sumNInst = processingData.realise<Data1D>(fmt::format("CN//{}Inst", rangeNames[i]), name(),
                                                                GenericItem::InRestartFileFlag);
                sumNInst.addPoint(moduleContext.dissolve().iteration(), sumN.value());
                if (exportInstantaneous_)
                {
                    Data1DExportFileFormat exportFormat(fmt::format("{}_Sum{}.txt", name(), rangeNames[i]));
                    if (!DataExporter<Data1D, Data1DExportFileFormat>::exportData(sumNInst, exportFormat,
                                                                                  moduleContext.processPool()))
                    {
                        Messenger::error("Failed to write instantaneous coordination number data for range {}.\n",
                                         rangeNames[i]);
                        return ExecutionResult::Failed;
                    }
                }
            }
        }
    // Setup RunningCN Histogram
    auto [runningCNHist, runCNstatus] =
        processingData.realiseIf<Histogram1D>("RunningCNHist", name(), GenericItem::InRestartFileFlag);
    if (runCNstatus == GenericItem::ItemStatus::Created)
        runningCNHist.initialise(distanceRange_.x, distanceRange_.y, distanceRange_.z);
    runningCNHist.zeroBins();

    // Zip over all distanceRange and calculate running CN
    double countCN{};

    for (const auto &&[x, currentCN] : zip(runningCNHist.binCentres(), histAB.data().values()))
    {
        countCN += currentCN;
        runningCNHist.bin(countCN);
    }

    // Accumulate histogram averages
    runningCNHist.accumulate();

    // Averaged values for RunningCN
    Data1D accumulatedRunningCNData = runningCNHist.accumulatedData();
    DataNormaliser1D normaliserRunningCN(accumulatedRunningCNData);

    // Normalise by A site population
    normaliserRunningCN.normaliseDivide(double(a.sites().size()));

    // Create the display data
    processingData.realise<Data1D>("RunningCN", name(), GenericItem::InRestartFileFlag) = accumulatedRunningCNData;

    // Save RDF data?
    if (!DataExporter<Data1D, Data1DExportFileFormat>::exportData(dataRDF, exportFileAndFormat_, moduleContext.processPool()))
        return ExecutionResult::Failed;

    return ExecutionResult::Success;
}
