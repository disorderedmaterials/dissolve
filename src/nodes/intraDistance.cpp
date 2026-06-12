// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodes/intraDistance.h"
#include "analyser/dataExporter.h"
#include "analyser/dataOperator1D.h"
#include "analyser/siteSelector.h"
#include "math/mathFunc.h"

IntraDistanceNode::IntraDistanceNode(Graph *parentGraph) : Node(parentGraph)
{
    // Inputs
    addInput<Configuration *>("Configuration", "Set target configuration for the node", targetConfiguration_);

    // Options
    addOption("SiteA", "Specify site(s) which represent 'A' in the interaction A-B-C", a_);
    addOption("SiteB", "Specify site(s) which represent 'B' in the interaction A-B-C", b_);
    addOption<Vector3>("DistanceRange", "Range (min, max, delta) of distance axis", distanceRange_);

    // Outputs
    addOutput<Configuration *>("Configuration", "Output configuration", targetConfiguration_);
}

std::string_view IntraDistanceNode::type() const { return "IntraDistance"; }

std::string_view IntraDistanceNode::summary() const { return "Calculate a site-site distance histogram within molecules"; }

// Run main processing
NodeConstants::ProcessResult IntraDistanceNode::process()
{
    // Select site A
    SiteSelector a(targetConfiguration_, a_.getSpeciesSites());

    // Select site B
    SiteSelector b(targetConfiguration_, b_.getSpeciesSites());

    // Calculate rAB
    if (!histAB_)
        histAB_.emplace(distanceRange_);

    histAB_->zeroBins();

    for (const auto &[siteA, indexA] : a.sites())
    {
        for (const auto &[siteB, indexB] : b.sites())
        {
            if (siteB->molecule() != siteA->molecule())
                continue;
            if (siteB == siteA)
                continue;
            histAB_->bin(targetConfiguration_->box().minimumDistance(siteA->origin(), siteB->origin()));
        }
    }

    // Accumulate histogram
    histAB_->accumulate();

    // Distance(A-B)
    rdfAB_ = histAB_->accumulatedData();

    // Normalise
    DataOperator1D histogramNormaliser(rdfAB_);
    // Normalise by value
    histogramNormaliser.normaliseSumTo();

    // Save Distance(A-B) data?
    /*
    if (!DataExporter::exportData(rdfAB_, exportFileAndFormat_))
        return NodeConstants::ProcessResult::Failed;
    */
    return NodeConstants::ProcessResult::Success;
}

// Temporary accessors to data for testing
const Data1D &IntraDistanceNode::rdfAB() const { return rdfAB_; }
