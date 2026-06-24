// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodes/sdf.h"
#include "analyser/dataOperator3D.h"
#include "analyser/siteSelector.h"

SDFNode::SDFNode(Graph *parentGraph) : Node(parentGraph)
{
    // Inputs
    addInput("Configuration", "Set target configuration for the module", configuration_)->setFlags({ParameterBase::Required});

    // Options
    addOption("SiteA", "Set the site(s) 'A' which are to represent the origin of the SDF", a_);
    addOption("SiteB", "Set the site(s) 'B' for which the distribution around the origin sites 'A' should be calculated", b_);
    addOption("RangeX", "Range along X axis", rangeX_);
    addOption("RangeY", "Range along Y axis", rangeY_);
    addOption("RangeZ", "Range along Z axis", rangeZ_);
    addOption("ExcludeSameMolecule", "Whether to exclude correlations between sites on the same molecule",
              excludeSameMolecule_);
}

/*
 * Definition
 */

// Return type of the node
std::string_view SDFNode::type() const { return "SDF"; }

// Return short summary of the node's purpose
std::string_view SDFNode::summary() const { return "Calculate spatial density functions around oriented sites"; }

/*
 * Data
 */

// Clear any local data
void SDFNode::clearData()
{
    histogram_.reset();
    sdf_.clear();
}

// Temporary accessors to data for testing
const Data3D &SDFNode::sdf() const { return sdf_; }

/*
 * Processing
 */

// Perform processing
NodeConstants::ProcessResult SDFNode::process()
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

            auto vBA = configuration_->box().minimumVector(siteA->origin(), siteB->origin());
            vBA = siteA->axes().transposeMultiply(vBA);
            histogram_->bin(vBA);
        }
    }

    // Accumulate histogram
    histogram_->accumulate();

    // SDF
    sdf_ = histogram_->accumulatedData();

    // Normalise
    DataOperator3D normaliserSDF(sdf_);
    // Normalise by A site population
    normaliserSDF.divide(double(a.sites().size()));
    // Normalise by grid
    normaliserSDF.normaliseByGrid();

    return NodeConstants::ProcessResult::Success;
}
