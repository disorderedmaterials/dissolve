// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodes/dAngle.h"
#include "analyser/dataExporter.h"
#include "analyser/dataOperator1D.h"
#include "analyser/dataOperator2D.h"
#include "analyser/siteSelector.h"
#include "math/mathFunc.h"

DAngleNode::DAngleNode(Graph *parentGraph) : Node(parentGraph)
{
    // Inputs
    addInput("Configuration", "Set target configuration for the module", configuration_)->setFlags({ParameterBase::Required});

    // Options
    addOption("SiteA", "Add site(s) which represent 'A' in the interaction A-B...C", a_);
    addOption("SiteB", "Add site(s) which represent 'B' in the interaction A-B...C", b_);
    addOption("SiteC", "Add site(s) which represent 'C' in the interaction A-B...C", c_);
    addOption("DistanceRange", "Range (min, max, binwidth) of distance binning", distanceRange_);
    addOption("AngleRange", "Range (min, max, binwidth) of angle binning", angleRange_);
    addOption("ExcludeSameMolecule", "Whether to exclude correlations between B and C sites on the same molecule",
              excludeSameMolecule_);
}

std::string_view DAngleNode::type() const { return "DAngle"; }

std::string_view DAngleNode::summary() const { return "Calculate distance-angle map (A-B)...C"; }

/*
 * Data
 */

// Clear any local data
void DAngleNode::clearData() {}

// Temporary accessors to data for testing
const Data1D &DAngleNode::rdfBC() const { return rdfBC_; }
const Data1D &DAngleNode::angle() const { return angle_; }
const Histogram2D &DAngleNode::distanceAngleMap() const { return *distanceAngleMap_; }
const Data2D &DAngleNode::dAngle() const { return dAngle_; }

/*
 * Processing
 */

// Run main processing
NodeConstants::ProcessResult DAngleNode::process()
{
    // Select site A
    const SiteSelector a(configuration_, a_.getSpeciesSites());

    // Select site B
    const SiteSelector b(configuration_, b_.getSpeciesSites());

    // Select site C
    const SiteSelector c(configuration_, c_.getSpeciesSites());

    // Initialise data storage if required
    if (!distanceHistogramBC_)
        distanceHistogramBC_.emplace().initialise(distanceRange_.x(), distanceRange_.y(), distanceRange_.z());
    if (!angleHistogram_)
        angleHistogram_.emplace().initialise(angleRange_.x(), angleRange_.y(), angleRange_.z());
    if (!distanceAngleMap_)
        distanceAngleMap_.emplace().initialise(distanceRange_.x(), distanceRange_.y(), distanceRange_.z(), angleRange_.x(),
                                               angleRange_.y(), angleRange_.z());

    distanceHistogramBC_->zeroBins();
    angleHistogram_->zeroBins();
    distanceAngleMap_->zeroBins();

    // Site statistics
    auto nASelections = 1;
    auto nAAvailable = a.sites().size(), nACumulative = a.sites().size();
    auto nBSelections = nAAvailable;
    auto nBAvailable = 0l, nBCumulative = 0l;
    auto nCSelections = 0;
    auto nCAvailable = 0l, nCCumulative = 0l;

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

                auto distanceBC = configuration_->box().minimumDistance(siteB->origin(), siteC->origin());
                auto angle = configuration_->box().angleInDegrees(siteA->origin(), siteB->origin(), siteC->origin());
                if (symmetric_ && angle > 90.0)
                    angle = 180.0 - angle;

                if (distanceAngleMap_->bin(distanceBC, angle))
                {
                    distanceHistogramBC_->bin(distanceBC);
                    angleHistogram_->bin(angle);
                }
            }
        }
    }

    // Accumulate histograms
    distanceAngleMap_->accumulate();
    distanceHistogramBC_->accumulate();
    angleHistogram_->accumulate();

    // RDF(B-C)
    rdfBC_ = distanceHistogramBC_->accumulatedData();
    DataOperator1D rBCNormaliser(rdfBC_);
    // Normalise by A site population
    rBCNormaliser.divide(double(nACumulative) / nASelections);
    // Normalise by B site population
    rBCNormaliser.divide(double(nBCumulative) / nBSelections);
    // Normalise by C site population density
    rBCNormaliser.divide((double(nCAvailable) / nCSelections) / configuration_->box().volume());
    // Normalise by spherical shell
    rBCNormaliser.normaliseBySphericalShell();

    // Angle(A-B-C)
    angle_ = angleHistogram_->accumulatedData();
    DataOperator1D aABCNormaliser(angle_);
    // Normalise by value / sin(x)
    aABCNormaliser.operate([](const auto &x, const auto &xDelta, const auto &value)
                           { return value / sin(DissolveMath::toRadians(x)); });
    // Normalise to 1.0
    aABCNormaliser.normaliseSumTo();

    // Distance-angle map
    dAngle_ = distanceAngleMap_->accumulatedData();
    DataOperator2D dAngleNormaliser(dAngle_);
    // Normalise by value / sin(y) / sin(yDelta)
    dAngleNormaliser.operate(
        [&](const auto &x, const auto &xDelta, const auto &y, const auto &yDelta, const auto &value)
        {
            return (symmetric_ ? value : value * 2.0) / sin(DissolveMath::toRadians(y)) / sin(DissolveMath::toRadians(yDelta));
        });
    // Normalise by A site population
    dAngleNormaliser.divide(double(nACumulative) / nASelections);
    // Normalise by B site population density
    dAngleNormaliser.divide((double(nBAvailable) / nBSelections) / configuration_->box().volume());
    // Normalise by spherical shell
    dAngleNormaliser.normaliseBySphericalShell();

    // // Save RDF(A-B) data?
    // if (!DataExporter::exportData(rBCNormalised, exportFileAndFormatRDF_))
    //     return ExecutionResult::Failed;
    //
    // // Save Angle(A-B-C) data?
    // if (!DataExporter::exportData(aABCNormalised, exportFileAndFormatAngle_))
    //     return ExecutionResult::Failed;
    //
    // // Save DAngle(A-(B-C)) data?
    // if (!DataExporter::exportData(dAngleNormalised, exportFileAndFormatDAngle_))
    //     return ExecutionResult::Failed;

    return NodeConstants::ProcessResult::Success;
}
