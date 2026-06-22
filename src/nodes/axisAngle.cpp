// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodes/axisAngle.h"
#include "analyser/dataOperator1D.h"
#include "analyser/dataOperator2D.h"
#include "analyser/siteSelector.h"
#include "math/mathFunc.h"

AxisAngleNode::AxisAngleNode(Graph *parentGraph) : Node(parentGraph)
{
    // Inputs
    addInput("Configuration", "Target configuration for the calculation", configuration_)->setFlags({ParameterBase::Required});

    // Options
    addOption("SiteA", "Specify site(s) which represent 'A' in the interaction A-B...C", a_);
    addOption("AxisA", "Axis to use from site A", axisA_);
    addOption("SiteB", "Specify site(s) which represent 'B' in the interaction A-B...C", b_);
    addOption("AxisB", "Axis to use from site B", axisB_);
    addOption("DistanceRange", "Range (min, max, binwidth) of distance binning", distanceRange_);
    addOption("AngleRange", "Range (min, max, binwidth) of angle binning", angleRange_);
    addOption("ExcludeSameMolecule", "Whether to exclude correlations between B and C sites on the same molecule",
              excludeSameMolecule_);
    addOption("Symmetric", "Whether the calculated angle should be mapped to 0 - 90 (i.e. is symmetric about 90)", symmetric_);
}

/*
 * Definition
 */

std::string_view AxisAngleNode::type() const { return "AxisAngle"; }

std::string_view AxisAngleNode::summary() const { return "Calculate distance/angle map between site axes"; }

/*
 * Data
 */

// Clear any local data
void AxisAngleNode::clearData()
{
    distanceHistogram_.reset();
    axisAngleHistogram_.reset();
    dAxisAngleHistogram_.reset();
}

// Temporary accessors to data for testing
const Data1D &AxisAngleNode::axisAngle() const { return axisAngle_; }

/*
 * Processing
 */

// Perform processing
NodeConstants::ProcessResult AxisAngleNode::process()
{
    // Select site A
    const SiteSelector a(configuration_, a_.getSpeciesSites());

    // Select site B
    const SiteSelector b(configuration_, b_.getSpeciesSites());

    // Initialise histograms if required
    if (!distanceHistogram_)
        distanceHistogram_.emplace(distanceRange_.value());
    if (!axisAngleHistogram_)
        axisAngleHistogram_.emplace(angleRange_.value());
    if (!dAxisAngleHistogram_)
        dAxisAngleHistogram_.emplace(distanceRange_, angleRange_);

    distanceHistogram_->zeroBins();
    axisAngleHistogram_->zeroBins();
    dAxisAngleHistogram_->zeroBins();

    for (const auto &[siteA, indexA] : a.sites())
    {
        for (const auto &[siteB, indexB] : b.sites())
        {
            if (excludeSameMolecule_ && (siteA->molecule() == siteB->molecule()))
                continue;

            auto distanceAB = configuration_->box().minimumDistance(siteA->origin(), siteB->origin());
            auto axisAngle = siteA->axes().columnAsVec3(axisA_).angleInDegrees(siteB->axes().columnAsVec3(axisB_));
            if (symmetric_ && axisAngle > 90.0)
                axisAngle = 180.0 - axisAngle;

            if (dAxisAngleHistogram_->bin(distanceAB, axisAngle))
            {
                distanceHistogram_->bin(distanceAB);
                axisAngleHistogram_->bin(axisAngle);
            }
        }
    }

    // Accumulate histograms
    distanceHistogram_->accumulate();
    axisAngleHistogram_->accumulate();
    dAxisAngleHistogram_->accumulate();

    // RDF
    rdf_ = distanceHistogram_->accumulatedData();
    DataOperator1D rdfNormaliser(rdf_);

    // Normalise by A site population
    rdfNormaliser.divide(double(a.sites().size()));
    // Normalise by B site population density
    rdfNormaliser.divide(double(b.sites().size()) / configuration_->box().volume());
    // Normalise by spherical shell
    rdfNormaliser.normaliseBySphericalShell();

    // AxisAngle(A-B)
    axisAngle_ = axisAngleHistogram_->accumulatedData();
    DataOperator1D axisAngleNormaliser(axisAngle_);
    // Normalise by value / sin(x)
    axisAngleNormaliser.operate([](const auto &x, const auto &xDelta, const auto &value)
                                { return value / sin(DissolveMath::toRadians(x)); });
    // Normalise to 1.0
    axisAngleNormaliser.normaliseSumTo();

    // DAxisAngle Map
    dAxisAngle_ = dAxisAngleHistogram_->accumulatedData();
    DataOperator2D dAxisAngleNormaliser(dAxisAngle_);
    // Normalise by value / sin(y) / sin(yDelta)
    dAxisAngleNormaliser.operate(
        [&](const auto &x, const auto &xDelta, const auto &y, const auto &yDelta, const auto &value)
        {
            return (symmetric_ ? value : value * 2.0) / sin(DissolveMath::toRadians(y)) / sin(DissolveMath::toRadians(yDelta));
        });
    // Normalise by A site population
    dAxisAngleNormaliser.divide(double(a.sites().size()));
    // Normalise by B site population density
    dAxisAngleNormaliser.divide(double(b.sites().size()) / configuration_->box().volume());
    // Normalise by spherical shell
    dAxisAngleNormaliser.normaliseBySphericalShell();

    return NodeConstants::ProcessResult::Success;
}
