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
    SiteFilter filter(targetConfiguration_, a.sites());

    Messenger::print("Site A: {}, Site B: {}", a.sites().size(), b.sites().size());

    // Calculate rAB
    // auto data = processingData.realise<Data1D>("distances", name(), GenericItem::InRestartFileFlag);
    // Calculate Distance
    auto [hist, status] = processingData.realiseIf<Histogram1D>("Hist-AB", name(), GenericItem::InRestartFileFlag);
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
                // I think we need to ensure that these are on different molecules!
                hist.bin(targetConfiguration_->box()->minimumDistance(siteA->origin(), siteB->origin()));
        }
    }
    hist.accumulate();

    auto &RDFdata = processingData.realise<Data1D>("Process1D//RDF", name(), GenericItem::InRestartFileFlag);
    RDFdata = hist.accumulatedData();
    RDFdata /= double(a.sites().size());
    RDFdata /= (double(a.sites().size() - 1) / targetConfiguration_->box()->volume());

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

    // Calculate RDF

    // Process1D RDF

    /*



    // auto &&[rAB, neighbourMap] = filter.filterBySiteProximity(b.sites(), distanceRange_, 0, 2);
    // std::map<const Site*, std::vector<double>> distances;

    }
    auto [histogram, status] = processingData.realiseIf<Histogram1D>("Histo-AB", name(), GenericItem::InRestartFileFlag);
    histogram.zeroBins();
    for (auto& v : distances)
        histogram.bin(v);
    histogram.accumulate();
    const auto &xAxis = histogram->xAxis();
    auto &values = histogram->values();
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
            if (histogram->valuesHaveErrors())
                histogram->error(n) /= divisor;

            // Overwrite old values
            r1Cubed = r2Cubed;
            leftBin = rightBin;
        }
    }



    // for (auto &[k,v] : distances)
    // {
    //     histogram.bin(v);
    // }

    // histogram.accumulate();

    for (const auto node : a.sites())
        (*histogram) /= node->nAverageSites();
    for (const auto node : b.sites())
        (*histogram) /= (node->nAvailableSitesAverage() / targetConfiguration_->box()->volume());

    processingData.realise<Data1D>("RDF", name(), GenericItem::InRestartFileFlag) = histogram.data();

    if (exportFileAndFormat_.hasFilename())
    {
        if (moduleContext.processPool().isMaster())
        {
            if (exportFileAndFormat_.exportData(histogram.accumulatedData()))
                moduleContext.processPool().decideTrue();
            else
                moduleContext.processPool().decideFalse();
        }
    }

    return ExecutionResult::Success;*/
    /*
    // Check for zero Configuration targets
    if (!targetConfiguration_)
    {
        Messenger::error("No configuration target set for module '{}'.\n", name());
        return ExecutionResult::Failed;
    }

    // Ensure any parameters in our nodes are set correctly
    collectDistance_->keywords().set("RangeX", distanceRange_);
    if (excludeSameMolecule_)
        selectB_->setSameMoleculeExclusions({selectA_});
    else
        selectB_->setSameMoleculeExclusions({});
    cnNormaliser_->keywords().set("Site", ConstNodeVector<SelectProcedureNode>{selectA_});

    // Execute the analysis
    if (!analyser_.execute({moduleContext.dissolve(), targetConfiguration_, name()}))
    {
        Messenger::error("CalculateRDF experienced problems with its analysis.\n");
        return ExecutionResult::Failed;
    }

    // Accumulate instantaneous coordination number data
    if (instantaneous_)
    {
        if (isRangeEnabled(0))
        {
            auto &sumA =
                moduleContext.dissolve().processingModuleData().realise<Data1D>("SumA", name(), GenericItem::InRestartFileFlag);
            sumA.addPoint(moduleContext.dissolve().iteration(), sumCN_->sum(0).value());
            if (exportInstantaneous_)
            {
                Data1DExportFileFormat exportFormat(fmt::format("{}_SumA.txt", name()));
                if (!exportFormat.exportData(sumA))
                {
                    Messenger::error("Failed to write instantaneous coordination number data for range A.\n");
                    return ExecutionResult::Failed;
                }
            }
        }
        if (isRangeEnabled(1))
        {
            auto &sumB =
                moduleContext.dissolve().processingModuleData().realise<Data1D>("SumB", name(), GenericItem::InRestartFileFlag);
            sumB.addPoint(moduleContext.dissolve().iteration(), sumCN_->sum(1).value());
            if (exportInstantaneous_)
            {
                Data1DExportFileFormat exportFormat(fmt::format("{}_SumB.txt", name()));
                if (!exportFormat.exportData(sumB))
                {
                    Messenger::error("Failed to write instantaneous coordination number data for range B.\n");
                    return ExecutionResult::Failed;
                }
            }
        }
        if (isRangeEnabled(2))
        {
            auto &sumC =
                moduleContext.dissolve().processingModuleData().realise<Data1D>("SumC", name(), GenericItem::InRestartFileFlag);
            sumC.addPoint(moduleContext.dissolve().iteration(), sumCN_->sum(2).value());
            if (exportInstantaneous_)
            {
                Data1DExportFileFormat exportFormat(fmt::format("{}_SumC.txt", name()));
                if (!exportFormat.exportData(sumC))
                {
                    Messenger::error("Failed to write instantaneous coordination number data for range C.\n");
                    return ExecutionResult::Failed;
                }
            }
        }
    }

    return ExecutionResult::Success;*/
}
