// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "analyser/dataNormaliser1D.h"
#include "base/sysFunc.h"
#include "io/export/data1D.h"
#include "main/dissolve.h"
#include "math/integrator.h"
#include "math/sampledDouble.h"
#include "module/context.h"
#include "modules/siteRDF/siteRDF.h"
#include "procedure/nodes/collect1D.h"
#include "procedure/nodes/operateSitePopulationNormalise.h"
#include "procedure/nodes/select.h"
#include "procedure/nodes/sequence.h"
#include "procedure/nodes/sum1D.h"

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
    auto [hist, status] = processingData.realiseIf<Histogram1D>("Histo-AB", name(), GenericItem::InRestartFileFlag);
    if (status == GenericItem::ItemStatus::Created)
    {
        hist.initialise(distanceRange_.x, distanceRange_.y, distanceRange_.z);
    }

    hist.zeroBins();
    for (const auto &[siteA, indexA] : a.sites())
    {
        for (const auto &[siteB, indexB] : b.sites())
        {
            if (siteB->molecule() != siteA->molecule())
                hist.bin(targetConfiguration_->box()->minimumDistance(siteA->origin(), siteB->origin()));
        }
    }
    hist.accumulate();

    // RDF
    auto &dataRDF = processingData.realise<Data1D>("Process1D//RDF", name(), GenericItem::InRestartFileFlag);
    dataRDF = hist.accumulatedData();
    DataNormaliser1D normaliserRDF(&dataRDF);

    // Normalise by A site population
    normaliserRDF.normaliseBySitePopulation(double(a.sites().size()));

    // Normalise by B site population density
    normaliserRDF.normaliseByNumberDensity(double(b.sites().size()), targetConfiguration_);

    // Normalise by spherical shell
    normaliserRDF.normaliseBySphericalShell();

    // CN
    auto &dataCN = processingData.realise<Data1D>("Process1D//HistogramNorm", name(), GenericItem::InRestartFileFlag);
    dataCN = hist.accumulatedData();
    DataNormaliser1D normaliserCN(&dataCN);
    normaliserCN.normaliseBySitePopulation(double(a.sites().size()));

    const std::vector<std::string> rangeNames = {"A", "B", "C"};
    for (int i = 0; i < 3; ++i)
        if (rangeEnabled_[i])
        {
            auto &sumN = processingData.realise<SampledDouble>(fmt::format("Sum1D//CN//{}", rangeNames[i]), name(),
                                                               GenericItem::InRestartFileFlag);
            sumN += Integrator::sum(dataCN, range_[i]);
        }

    return ExecutionResult::Success;
}
