// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodes/histogramCN.h"
#include "analyser/dataOperator1D.h"
#include "analyser/siteSelector.h"

HistogramCNNode::HistogramCNNode(Graph *parentGraph) : Node(parentGraph)
{
    // Inputs
    addInput("Configuration", "Target configuration for the calculation", configuration_)->setFlags({ParameterBase::Required});

    // Options
    addOption("SiteA", "Set the site(s) 'A' which are to represent the reference origin", a_);
    addOption("SiteB", "Set the site(s) 'B' for which the coordination number around the origin sites should be calculated",
              b_);
    addOption("RangeAB", "Distance range (min, max) over which to calculate coordination number from central site",
              distanceRange_);
}

/*
 * Definition
 */

// Return type of the node
std::string_view HistogramCNNode::type() const { return "HistogramCN"; }

// Return short summary of the node's purpose
std::string_view HistogramCNNode::summary() const { return "Produce a histogram of coordination numbers between sites"; }

/*
 * Data
 */

// Clear any local data
void HistogramCNNode::clearData() { histogram_.reset(); }

// Temporary accessors to data for testing
const Data1D &HistogramCNNode::cn() const { return cn_; }

/*
 * Processing
 */

// Perform processing
NodeConstants::ProcessResult HistogramCNNode::process()
{
    // Select site A
    const SiteSelector a(configuration_, a_.getSpeciesSites());

    // Select site B
    const SiteSelector b(configuration_, b_.getSpeciesSites());

    // Initialise histogram if required
    if (!histogram_)
        histogram_.emplace();

    histogram_->zeroBins();

    for (const auto &[siteA, indexA] : a.sites())
    {
        auto nSelected = 0;
        for (const auto &[siteB, indexB] : b.sites())
        {
            if (siteB == siteA)
                continue;
            if (!distanceRange_.contains(configuration_->box().minimumDistance(siteB->origin(), siteA->origin())))
                continue;
            ++nSelected;
        }
        histogram_->bin(nSelected);
    }

    // Accumulate histogram
    histogram_->accumulate();

    // CN
    cn_ = histogram_->accumulatedData();

    // Normalise CN
    DataOperator1D normaliserCN(cn_);
    // Normalise by value
    normaliserCN.normaliseSumTo();

    return NodeConstants::ProcessResult::Success;
}
