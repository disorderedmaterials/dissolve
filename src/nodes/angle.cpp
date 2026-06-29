// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodes/angle.h"
#include "analyser/dataOperator1D.h"
#include "analyser/dataOperator2D.h"
#include "analyser/siteSelector.h"
#include "math/mathFunc.h"

AngleNode::AngleNode(Graph *parentGraph) : Node(parentGraph)
{
    // Inputs
    addInput("Configuration", "Target configuration for the calculation", configuration_)->setFlags({ParameterBase::Required});

    // Outputs
    addOutput("Configuration", "Output configuration", configuration_);

    // Options
    addOption("SiteA", "Specify site(s) which represent 'A' in the interaction A-B-C", a_);
    addOption("SiteB", "Specify site(s) which represent 'B' in the interaction A-B-C", b_);
    addOption("SiteC", "Specify site(s) which represent 'C' in the interaction A-B-C", c_);
    addOption("RangeAB", "Range (min, max, binwidth) of A-B distance binning", rangeAB_);
    addOption("RangeBC", "Range (min, max, binwidth) of B-C distance binning", rangeBC_);
    addOption("AngleRange", "Range (min, max, binwidth) of angle binning", angleRange_);
    addOption("ExcludeSameMoleculeAB", "Whether to exclude correlations between A and B sites on the same molecule",
              excludeSameMoleculeAB_);
    addOption("ExcludeSameMoleculeBC", "Whether to exclude correlations between B and C sites on the same molecule",
              excludeSameMoleculeBC_);
    addOption("ExcludeSameSiteAC", "Whether to exclude correlations between A and C sites on the same molecule",
              excludeSameSiteAC_);
    addOption("Symmetric", "Whether the calculated angle should be mapped to 0 - 90 (i.e. is symmetric about 90)", symmetric_);

    // Serialisables
    addSerialisable("distanceHistogramAB", distanceHistogramAB_);
    addSerialisable("rdfAB", rdfAB_);
    addSerialisable("distanceHistogramBC", distanceHistogramBC_);
    addSerialisable("rdfBC", rdfBC_);
    addSerialisable("angleHistogramABC", angleHistogramABC_);
    addSerialisable("angleABC", angleABC_);
    addSerialisable("dAngleHistogramAB", dAngleHistogramAB_);
    addSerialisable("dAngleAB", dAngleAB_);
    addSerialisable("dAngleHistogramBC", dAngleHistogramBC_);
    addSerialisable("dAngleBC", dAngleBC_);
    addSerialisable("dDAngleHistogramABC", dDAngleHistogramABC_);
    addSerialisable("dDAngleABC", dDAngleABC_);
}

/*
 * Definition
 */

// Return type of the node
std::string_view AngleNode::type() const { return "Angle"; }

// Return short summary of the node's purpose
std::string_view AngleNode::summary() const { return "Calculate details of a specific angle A-B-C"; }

/*
 * Data
 */

// Clear any local data
void AngleNode::clearData()
{
    distanceHistogramAB_.reset();
    distanceHistogramBC_.reset();
    angleHistogramABC_.reset();
    dAngleHistogramAB_.reset();
    dAngleHistogramBC_.reset();
    dDAngleHistogramABC_.reset();
}

// Temporary accessors to data for testing
const Data1D &AngleNode::rdfBC() const { return rdfBC_; }

const Data1D &AngleNode::angleABC() const { return angleABC_; }

/*
 * Processing
 */

// Perform processing
NodeConstants::ProcessResult AngleNode::process()
{
    // Select site A
    const SiteSelector a(configuration_, a_.getSpeciesSites());

    // Select site B
    const SiteSelector b(configuration_, b_.getSpeciesSites());

    // Select site C
    const SiteSelector c(configuration_, c_.getSpeciesSites());

    // Initialise histograms if required
    if (!distanceHistogramAB_)
        distanceHistogramAB_.emplace(rangeAB_.value());
    if (!distanceHistogramBC_)
        distanceHistogramBC_.emplace(rangeBC_.value());
    if (!angleHistogramABC_)
        angleHistogramABC_.emplace(angleRange_.value());
    if (!dAngleHistogramAB_)
        dAngleHistogramAB_.emplace(rangeAB_, angleRange_);
    if (!dAngleHistogramBC_)
        dAngleHistogramBC_.emplace(rangeBC_, angleRange_);
    if (!dDAngleHistogramABC_)
        dDAngleHistogramABC_.emplace(rangeAB_, rangeBC_, angleRange_);

    distanceHistogramAB_->zeroBins();
    distanceHistogramBC_->zeroBins();
    angleHistogramABC_->zeroBins();
    dAngleHistogramAB_->zeroBins();
    dAngleHistogramBC_->zeroBins();
    dDAngleHistogramABC_->zeroBins();

    auto nACumulative = a.sites().size();
    auto nASelections = 1;
    auto nBAvailable = 0l, nBCumulative = 0l, nBSelections = 0l;
    auto nCAvailable = 0l, nCCumulative = 0l, nCSelections = 0l;

    for (const auto &[siteA, indexA] : a.sites())
    {
        ++nBSelections;
        for (const auto &[siteB, indexB] : b.sites())
        {
            // Check for molecule exclusions (A != B)
            if (excludeSameMoleculeAB_ && (siteB->molecule() == siteA->molecule()))
                continue;

            // Get A-B distance and check against defined range
            auto distAB = configuration_->box().minimumDistance(siteA->origin(), siteB->origin());
            ++nBAvailable;
            if (distAB < rangeAB_.value().x || distAB > rangeAB_.value().y)
                continue;

            // Bin this distance
            distanceHistogramAB_->bin(distAB);
            ++nBCumulative;
            ++nCSelections;

            for (const auto &[siteC, indexC] : c.sites())
            {
                // Check for molecule exclusions (B != C) and site exclusion (A != C)
                if (excludeSameMoleculeBC_ && (siteC->molecule() == siteB->molecule()))
                    continue;
                if (excludeSameSiteAC_ && (siteC == siteA))
                    continue;

                // Get B-C distance and check against defined range
                auto distBC = configuration_->box().minimumDistance(siteB->origin(), siteC->origin());
                ++nCAvailable;
                if (distBC < rangeBC_.value().x || distBC > rangeBC_.value().y)
                    continue;

                ++nCCumulative;

                // Calculate angle and do final binning
                auto angle = configuration_->box().angleInDegrees(siteA->origin(), siteB->origin(), siteC->origin());
                if (symmetric_ && angle > 90.0)
                    angle = 180.0 - angle;

                distanceHistogramBC_->bin(distBC);
                angleHistogramABC_->bin(angle);
                dAngleHistogramAB_->bin(distAB, angle);
                dAngleHistogramBC_->bin(distBC, angle);
                dDAngleHistogramABC_->bin(distAB, distBC, angle);
            }
        }
    }

    // Accumulate histograms
    distanceHistogramAB_->accumulate();
    distanceHistogramBC_->accumulate();
    angleHistogramABC_->accumulate();
    dAngleHistogramAB_->accumulate();
    dAngleHistogramBC_->accumulate();
    dDAngleHistogramABC_->accumulate();

    // RDF(A-B)
    rdfAB_ = distanceHistogramAB_->accumulatedData();
    DataOperator1D normaliserAB_(rdfAB_);
    // Normalise by A site population
    normaliserAB_.divide(double(nACumulative) / nASelections);
    // Normalise by B site population density
    normaliserAB_.divide((double(nBCumulative) / nBSelections) / configuration_->box().volume());
    // Normalise by spherical shell
    normaliserAB_.normaliseBySphericalShell();

    // RDF(B-C)
    rdfBC_ = distanceHistogramBC_->accumulatedData();
    DataOperator1D normaliserBC_(rdfBC_);
    // Normalise by A site population
    normaliserBC_.divide(double(nACumulative) / nASelections);
    // Normalise by B site population
    normaliserBC_.divide(double(nBCumulative) / nBSelections);
    // Normalise by C site population density
    normaliserBC_.divide((double(nCAvailable) / nCSelections) / configuration_->box().volume());
    // Normalise by spherical shell
    normaliserBC_.normaliseBySphericalShell();

    // Angle(A-B-C)
    angleABC_ = angleHistogramABC_->accumulatedData();
    DataOperator1D normaliserAngle(angleABC_);
    // Normalise by value / sin(x)
    normaliserAngle.operate([](const auto &x, const auto &xDelta, const auto &value)
                            { return value / sin(DissolveMath::toRadians(x)); });
    // Normalise to 1.0
    normaliserAngle.normaliseSumTo();

    // DAngle((A-B)-C)
    dAngleAB_ = dAngleHistogramAB_->accumulatedData();
    DataOperator2D normaliserDAngleAB(dAngleAB_);
    // Normalise by value / sin(y) / sin(yDelta)
    normaliserDAngleAB.operate(
        [&](const auto &x, const auto &xDelta, const auto &y, const auto &yDelta, const auto &value)
        {
            return (symmetric_ ? value : value * 2.0) / sin(DissolveMath::toRadians(y)) / sin(DissolveMath::toRadians(yDelta));
        });
    // Normalise by A site population
    normaliserDAngleAB.divide(double(nACumulative) / nASelections);
    // Normalise by C site population
    normaliserDAngleAB.divide(double(nCCumulative) / nCSelections);
    // Normalise by B site population density
    normaliserDAngleAB.divide((double(nBAvailable) / nBSelections) / configuration_->box().volume());
    // Normalise by spherical shell
    normaliserDAngleAB.normaliseBySphericalShell();

    // DAngle(A-(B-C))
    dAngleBC_ = dAngleHistogramBC_->accumulatedData();
    DataOperator2D normaliserDAngleBC(dAngleBC_);
    // Normalise by value / sin(y) / sin(yDelta)
    normaliserDAngleBC.operate(
        [&](const auto &x, const auto &xDelta, const auto &y, const auto &yDelta, const auto &value)
        {
            return (symmetric_ ? value : value * 2.0) / sin(DissolveMath::toRadians(y)) / sin(DissolveMath::toRadians(yDelta));
        });
    // Normalise by A site population
    normaliserDAngleBC.divide(double(nACumulative) / nASelections);
    // Normalise by B site population
    normaliserDAngleBC.divide(double(nBCumulative) / nBSelections);
    // Normalise by C site population density
    normaliserDAngleBC.divide((double(nCAvailable) / nCSelections) / configuration_->box().volume());
    // Normalise by spherical shell
    normaliserDAngleBC.normaliseBySphericalShell();

    // // Save RDF(A-B) data?
    // if (!DataExporter::exportData(rdfAB_, exportFileAndFormatAB_))
    //     return NodeConstants::ProcessResult::Failed;

    // // Save RDF(B-C) data?
    // if (!DataExporter::exportData(rdfBC_, exportFileAndFormatBC_))
    //     return NodeConstants::ProcessResult::Failed;

    // // Save Angle(A-B-C) data?
    // if (!DataExporter::exportData(angleABC_, exportFileAndFormatAngle_))
    //     return NodeConstants::ProcessResult::Failed;

    // // Save DAngle((A-B)-C) data?
    // if (!DataExporter::exportData(dAngleAB_, exportFileAndFormatDAngleAB_))
    //     return NodeConstants::ProcessResult::Failed;

    // // Save DAngle(A-(B-C)) data?
    // if (!DataExporter::exportData(dAngleBC_, exportFileAndFormatDAngleBC_))
    //     return NodeConstants::ProcessResult::Failed;

    return NodeConstants::ProcessResult::Success;
}
