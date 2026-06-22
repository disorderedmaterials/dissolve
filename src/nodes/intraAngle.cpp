// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodes/intraAngle.h"
#include "analyser/dataExporter.h"
#include "analyser/dataOperator1D.h"
#include "analyser/siteSelector.h"
#include "math/mathFunc.h"

IntraAngleNode::IntraAngleNode(Graph *parentGraph) : Node(parentGraph)
{
    // Inputs
    addInput<Configuration *>("Configuration", "Set target configuration for the node", targetConfiguration_);

    // Options
    addOption("SiteA", "Specify site(s) which represent 'A' in the interaction A-B-C", a_);
    addOption("SiteB", "Specify site(s) which represent 'B' in the interaction A-B-C", b_);
    addOption("SiteC", "Specify site(s) which represent 'C' in the interaction A-B-C", c_);
    addOption("RangeAB", "Range (min, max, binwidth) of A-B distance binning", rangeAB_);
    addOption("RangeBC", "Range (min, max, binwidth) of B-C distance binning", rangeBC_);
    addOption<Vector3>("AngleRange", "Range (min, max, binwidth) of angle binning", angleRange_);
    addOption<bool>("Symmetric", "Whether the calculated angle should be mapped to 0 - 90 (i.e. is symmetric about 90)",
                    symmetric_);

    // Outputs
    addOutput<Configuration *>("Configuration", "Output configuration", targetConfiguration_);
}

/*
 * Definition
 */

// Return type of the node
std::string_view IntraAngleNode::type() const { return "IntraAngle"; }

// Return short summary of the node's purpose
std::string_view IntraAngleNode::summary() const
{
    return "Calculate angle distributions between sites within the same molecule";
}

/*
 * Data
 */

// Clear any local data
void IntraAngleNode::clearData()
{
    intraAngleHist_.reset();
    angleABC_.clear();
}

/*
 * Processing
 */

// Perform processing
NodeConstants::ProcessResult IntraAngleNode::process()
{
    // Select site A
    SiteSelector a(targetConfiguration_, a_.getSpeciesSites());

    // Select site B
    SiteSelector b(targetConfiguration_, b_.getSpeciesSites());

    // Select site C
    SiteSelector c(targetConfiguration_, c_.getSpeciesSites());

    // Intramolecular angle histogram
    if (!intraAngleHist_)
        intraAngleHist_.emplace(angleRange_);

    intraAngleHist_->zeroBins();

    for (const auto &[siteA, indexA] : a.sites())
    {
        for (const auto &[siteB, indexB] : b.sites())
        {

            if (siteB->molecule() != siteA->molecule())
                continue;

            if (siteB == siteA)
                continue;

            if (!rangeAB_.contains(targetConfiguration_->box().minimumDistance(siteA->origin(), siteB->origin())))
                continue;

            for (const auto &[siteC, indexC] : c.sites())
            {

                if (siteC->molecule() != siteA->molecule())
                    continue;

                if (siteC == siteA)
                    continue;

                if (siteC == siteB)
                    continue;

                if (!rangeBC_.contains(targetConfiguration_->box().minimumDistance(siteB->origin(), siteC->origin())))
                    continue;

                auto angle = targetConfiguration_->box().angleInDegrees(siteA->origin(), siteB->origin(), siteC->origin());
                if (symmetric_ && angle > 90.0)
                    angle = 180.0 - angle;
                intraAngleHist_->bin(angle);
            }
        }
    }

    // Accumulate histogram
    intraAngleHist_->accumulate();

    // Angle(ABC)
    angleABC_ = intraAngleHist_->accumulatedData();

    // Normalise
    DataOperator1D normaliser(angleABC_);
    // Normalise by sin(x)
    normaliser.operate([](const auto &x, const auto &xDelta, const auto &value)
                       { return value / sin(DissolveMath::toRadians(x)); });
    // Normalise by value
    normaliser.normaliseSumTo();

    // Save Angle(A-B-C) data?
    /*
    if (!DataExporter::exportData(angleABC_, exportFileAndFormat_))
        return NodeConstants::ProcessResult::Failed;
    */
    return NodeConstants::ProcessResult::Success;
}

// Temporary accessors to data for testing
const Data1D &IntraAngleNode::intraAngleData() const { return angleABC_; }
