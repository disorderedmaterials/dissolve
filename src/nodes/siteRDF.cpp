// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodes/siteRDF.h"
#include "analyser/dataExporter.h"
#include "analyser/dataOperator1D.h"
#include "io/export/data1D.h"
#include "main/dissolve.h"
#include "math/histogram1D.h"
#include "math/integrator.h"
#include "math/sampledData1D.h"
#include "math/sampledDouble.h"
#include "templates/algorithms.h"
#include "templates/combinable.h"

SiteRDFNode::SiteRDFNode(Graph *parentGraph) : Node(parentGraph)
{
    // Inputs
    addInput<Configuration *>("Configuration", "Set target configuration for the module", configuration_)
        ->setFlags({ParameterBase::Required, ParameterBase::ClearData});

    // Outputs
    addOutput<Configuration *>("Configuration", "Output configuration", configuration_);

    // Options
    addOption("SiteA", "Set the site(s) 'A' which are to represent the origin of the RDF", a_);
    addOption("SiteB", "Set the site(s) 'B' for which the distribution around the origin sites 'A' should be calculated", b_);
    addOption("DistanceRange", "Range (min, max, delta) of distance axis", distanceRange_);
    addOption("ExcludeSameMolecule", "Whether to exclude correlations between sites on the same molecule",
              excludeSameMolecule_);
    addOption("RangeAEnabled", "Whether calculation of the second coordination number is enabled", rangeEnabled_[0]);
    addOption("RangeA", "Distance range for first coordination number", rangeA_);
    addOption("RangeBEnabled", "Whether calculation of the second coordination number is enabled", rangeEnabled_[1]);
    addOption("RangeB", "Distance range for second coordination number", rangeB_);
    addOption("RangeCEnabled", "Whether calculation of the third coordination number is enabled", rangeEnabled_[2]);
    addOption("RangeC", "Distance range for third coordination number", rangeC_);
    addOption("Instantaneous", "Whether to calculate instantaneous coordination numbers rather than forming an average",
              instantaneous_);
}

std::string_view SiteRDFNode::type() const { return "SiteRDF"; }

std::string_view SiteRDFNode::summary() const
{
    return "Calculate a site-site radial distribution function and associated coordination numbers";
}

// Run main processing
NodeConstants::ProcessResult SiteRDFNode::process()
{
    // Select site A
    SiteSelector a(configuration_, a_.getSpeciesSites());

    // Select site B
    SiteSelector b(configuration_, b_.getSpeciesSites());

    // Calculate rAB
    if (!histAB_)
        histAB_.emplace(distanceRange_.value());
    histAB_->zeroBins();

    auto combinableHistograms = dissolve::CombinableValue<Histogram1D>(*histAB_);

    dissolve::for_each(std::execution::par, a.sites().begin(), a.sites().end(),
                       [this, &b, &combinableHistograms](const auto &pair)
                       {
                           const auto &[siteA, indexA] = pair;

                           auto &hist = combinableHistograms.local();
                           for (const auto &[siteB, indexB] : b.sites())
                           {
                               if (excludeSameMolecule_ && (siteB->molecule() == siteA->molecule()))
                                   continue;
                               hist.bin(configuration_->box()->minimumDistance(siteA->origin(), siteB->origin()));
                           }
                       });

    histAB_ = combinableHistograms.finalize();

    // Accumulate histogram
    histAB_->accumulate();

    // RDF
    dataRDF_ = histAB_->accumulatedData();

    // Normalise
    DataOperator1D normaliserRDF(dataRDF_);
    // Normalise by A site population
    normaliserRDF.divide(double(a.sites().size()));

    // Normalise by B site population density
    normaliserRDF.divide(double(b.sites().size()) / configuration_->box()->volume());

    // Normalise by spherical shell
    normaliserRDF.normaliseBySphericalShell();

    // CN
    Data1D dataCN;
    dataCN = histAB_->accumulatedData();

    // Normalise
    DataOperator1D normaliserCN(dataCN);
    // Normalise by A site population
    normaliserCN.divide(double(a.sites().size()));

    const std::vector<std::string> rangeNames = {"A", "B", "C"};
    Range ranges[3] = {rangeA_, rangeB_, rangeC_};
    for (int i = 0; i < 3; ++i)
        if (rangeEnabled_[i])
        {
            auto rangeName = rangeNames[i];
            sums_.try_emplace(rangeName, SampledDouble(), instantaneous_ ? std::optional<Data1D>{} : std::nullopt);
            auto &[sumN, sumNInst] = sums_[rangeName];
            sumN += Integrator::sum(dataCN, ranges[i]);
            if (sumNInst)
            {
                sumNInst->addPoint(dissolve().iteration(), sumN.value());
                if (exportInstantaneous_)
                {
                    Data1DExportFileFormat exportFormat(std::format("{}_Sum{}.txt", name(), rangeName));
                    if (!DataExporter::exportData(*sumNInst, exportFormat))
                    {
                        error("Failed to write instantaneous coordination number data for range {}.\n", rangeNames[i]);
                        return NodeConstants::ProcessResult::Failed;
                    }
                }
            }
        }

    // Accumulate instantaneous binValues
    auto instBinValues = histAB_->data();

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

    /*
    // Save RDF data?
    if (!DataExporter::exportData(dataRDF_, exportFileAndFormat_))
        return NodeConstants::ProcessResult::Failed;
    */

    return NodeConstants::ProcessResult::Success;
}

const Data1D &SiteRDFNode::dataRDF() { return dataRDF_; }

const SiteRDFNode::Sums &SiteRDFNode::sums() { return sums_; }