// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodes/orientedSDF.h"
#include "analyser/dataOperator3D.h"
#include "analyser/siteSelector.h"

OrientedSDFNode::OrientedSDFNode(Graph *parentGraph) : Node(parentGraph)
{
    // Inputs
    addInput("Configuration", "Set target configuration for the module", configuration_)->setFlags({ParameterBase::Required});

    // Options
    addOption("SiteA", "Set the site(s) 'A' which are to represent the origin of the OrientedSDF", a_);
    addOption("AxisA", "Axis to use from site A", axisA_);
    addOption("SiteB", "Set the site(s) 'B' for which the distribution around the origin sites 'A' should be calculated", b_);
    addOption("AxisB", "Axis to use from site B", axisB_);
    addOption("RangeX", "Range along X axis", rangeX_);
    addOption("RangeY", "Range along Y axis", rangeY_);
    addOption("RangeZ", "Range along Z axis", rangeZ_);
    addOption("AngleRange", "Axis angle range required to permit a site to be binned in the SDF", angleRange_);
    addOption("ExcludeSameMolecule", "Whether to exclude correlations between sites on the same molecule",
              excludeSameMolecule_);
    addOption("Symmetric", "Whether the calculated angle should be mapped to 0 - 90 (i.e. is symmetric about 90)", symmetric_);
}

std::string_view OrientedSDFNode::type() const { return "OrientedSDF"; }

std::string_view OrientedSDFNode::summary() const
{
    return "Calculate spatial density functions around oriented sites, restricted by relative molecule orientation";
}

/*
 * Data
 */

// Clear any local data
void OrientedSDFNode::clearData()
{
    histogram_.reset();
    sdf_.clear();
}

// Temporary accessors to data for testing
const Data3D &OrientedSDFNode::sdf() const { return sdf_; }

/*
 * Processing
 */

// Perform processing
NodeConstants::ProcessResult OrientedSDFNode::process()
{
    // Select site A
    const SiteSelector a(configuration_, a_.getSpeciesSites());

    // Select site B
    const SiteSelector b(configuration_, b_.getSpeciesSites());

    // Initialise sdata storage if required
    if (!histogram_)
        histogram_.emplace().initialise(rangeX_.x, rangeX_.y, rangeX_.z, rangeY_.x, rangeY_.y, rangeY_.z, rangeZ_.x, rangeZ_.y,
                                        rangeZ_.z);

    histogram_->zeroBins();

    for (const auto &[siteA, indexA] : a.sites())
    {
        for (const auto &[siteB, indexB] : b.sites())
        {
            if (excludeSameMolecule_ && siteB->molecule() == siteA->molecule())
                continue;

            if (siteB == siteA)
                continue;

            auto axisAngle = siteA->axes().columnAsVec3(axisA_).angleInDegrees(siteB->axes().columnAsVec3(axisB_));
            if (symmetric_ && axisAngle > 90.0)
                axisAngle = 180.0 - axisAngle;
            if (angleRange_.contains(axisAngle))
            {
                auto vBA = configuration_->box().minimumVector(siteA->origin(), siteB->origin());
                vBA = siteA->axes().transposeMultiply(vBA);
                histogram_->bin(vBA);
            }
        }
    }

    // Accumulate histogram
    histogram_->accumulate();

    // OrientedSDF
    sdf_ = histogram_->accumulatedData();

    // Normalise
    DataOperator3D normaliserOrientedSDF(sdf_);
    // Normalise by A site population
    normaliserOrientedSDF.divide(double(a.sites().size()));
    // Normalise by grid
    normaliserOrientedSDF.normaliseByGrid();

    return NodeConstants::ProcessResult::Success;
}
