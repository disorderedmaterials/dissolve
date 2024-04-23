// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "analyser/dataExporter.h"
#include "analyser/dataOperator1D.h"
#include "classes/productIterator.h"
#include "base/sysFunc.h"
#include "io/export/data1D.h"
#include "main/dissolve.h"
#include "math/integrator.h"
#include "math/sampledData1D.h"
#include "math/sampledDouble.h"
#include "module/context.h"
#include "modules/siteRDF/siteRDF.h"
#include "templates/algorithms.h"
#include "templates/combinable.h"

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

    ProductIterator pairs(a.sites().begin(), a.sites().end(), b.sites().begin(), b.sites().end());
    auto combinableHistograms = dissolve::CombinableValue<Histogram1D>(
        [&histAB]()
        {
            return histAB;
        });

    dissolve::for_each(std::execution::par_unseq, pairs.begin(), pairs.end(), [this, &combinableHistograms](const auto& pair) {
        const auto [aIndex, bIndex] = pair;
        const auto &[siteA, indexA] = aIndex;
        const auto &[siteB, indexB] = bIndex;
        if (excludeSameMolecule_ && (siteB->molecule() == siteA->molecule()))
          return;
        auto &hist = combinableHistograms.local();
        hist.bin(targetConfiguration_->box()->minimumDistance(siteA->origin(), siteB->origin()));
      });

    histAB = combinableHistograms.finalize();

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

    auto &dataRunningCN = processingData.realise<SampledData1D>("RunningCNTest", name(), GenericItem::InRestartFileFlag);
    std::vector<double> runningCN;

    // Accumulate instantaneous binValues
    auto instBinValues = histAB.data();

    // Normalise Data
    DataOperator1D normaliserInstBinValues(instBinValues);

    // Normalise by A site population
    normaliserInstBinValues.divide(double(a.sites().size()));

    auto sum = 0.0;
    std::transform(instBinValues.values().begin(), instBinValues.values().end(), instBinValues.values().begin(),
                   [&](const auto &currentBin)
                   {
                       sum += currentBin;
                       return sum;
                   });

    // Add normalised data
    dataRunningCN += instBinValues;

    // Create the display data
    processingData.realise<SampledData1D>("RunningCN", name(), GenericItem::InRestartFileFlag) = dataRunningCN;

    // Save RDF data?
    if (!DataExporter<Data1D, Data1DExportFileFormat>::exportData(dataRDF, exportFileAndFormat_, moduleContext.processPool()))
        return ExecutionResult::Failed;

    return ExecutionResult::Success;
}
