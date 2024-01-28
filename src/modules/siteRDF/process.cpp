// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

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
    auto &RDFdata = processingData.realise<Data1D>("Process1D//RDF", name(), GenericItem::InRestartFileFlag);
    RDFdata = hist.accumulatedData();

    // Normalise by A site population
    RDFdata /= double(a.sites().size());

    // Normalise by B site population density
    RDFdata /= (double(b.sites().size()) / targetConfiguration_->box()->volume());

    // Normalise by spherical shell
    // We expect x values to be centre-bin values, and regularly spaced
    const auto &xAxis = RDFdata.xAxis();
    auto &values = RDFdata.values();
    if (xAxis.size() >= 2)
    {
        // Derive first left-bin boundary from the delta between points 0 and 1
        double leftBin = xAxis[0] - (xAxis[1] - xAxis[0]) * 0.5, rightBin, divisor;
        double r1Cubed = pow(leftBin, 3), r2Cubed;
        for (auto n = 0; n < xAxis.size(); ++n)
        {
            // Get new right-bin from existing left bin boundary and current bin centre
            rightBin = leftBin + 2 * (xAxis[n] - leftBin);
            r2Cubed = pow(rightBin, 3);
            divisor = (4.0 / 3.0) * PI * (r2Cubed - r1Cubed);
            values[n] /= divisor;
            if (RDFdata.valuesHaveErrors())
                RDFdata.error(n) /= divisor;

            // Overwrite old values
            r1Cubed = r2Cubed;
            leftBin = rightBin;
        }
    }

    // CN
    auto &HistogramNormdata =
        processingData.realise<Data1D>("Process1D//HistogramNorm", name(), GenericItem::InRestartFileFlag);
    HistogramNormdata = hist.accumulatedData();
    HistogramNormdata /= double(a.sites().size());

    OptionalReferenceWrapper<SampledDouble> sum_[3];
    const std::vector<std::string> rangeNames = {"A", "B", "C"};
    for (int i = 0; i < 3; ++i)
        if (rangeEnabled_[i])
        {
            if (!sum_[i].has_value())
                sum_[i] = processingData.realise<SampledDouble>(fmt::format("Sum1D//CN//{}", rangeNames[i]), name(),
                                                                GenericItem::InRestartFileFlag);
            sum_[i]->get() += Integrator::sum(HistogramNormdata, range_[i]);
        }

    return ExecutionResult::Success;
}
