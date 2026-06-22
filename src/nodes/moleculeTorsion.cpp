// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodes/moleculeTorsion.h"
#include "analyser/dataOperator1D.h"
#include "math/histogram1D.h"

MoleculeTorsionNode::MoleculeTorsionNode(Graph *parentGraph) : Node(parentGraph)
{
    // Inputs
    addInput("Configuration", "Set target configuration for the module", configuration_)->setFlags({ParameterBase::Required});

    // Options
    addOption("Species", "Target species containing the torsion", species_);
    addOption("I", "Index of atom i in the torsion i-j-k-l", i_);
    addOption("J", "Index of atom j in the torsion i-j-k-l", j_);
    addOption("K", "Index of atom k in the torsion i-j-k-l", k_);
    addOption("L", "Index of atom l in the torsion i-j-k-l", l_);
}

/*
 * Definition
 */

std::string_view MoleculeTorsionNode::type() const { return "MoleculeTorsion"; };

std::string_view MoleculeTorsionNode::summary() const
{
    return "Calculate a specific torsion distribution in a molecule type";
};

/*
 * Data
 */

// Clear any local data
void MoleculeTorsionNode::clearData()
{
    histogram_.reset();
    frequency_.clear();
}

// Temporary accessors to data for testing
const Data1D &MoleculeTorsionNode::frequency() const { return frequency_; }

/*
 * Processing
 */

// Perform processing
NodeConstants::ProcessResult MoleculeTorsionNode::process()
{
    // Initialise histograms if required
    if (!histogram_)
        histogram_.emplace().initialise(-180.0, 180.0, 1.0);

    histogram_->zeroBins();

    // Loop over molecules in the Configuration
    const auto &box = configuration_->box();
    for (const auto &mol : configuration_->molecules())
    {
        if (mol->species() != species_)
            continue;

        histogram_->bin(box.torsionInDegrees(mol->atom(i_.asInteger())->r(), mol->atom(j_.asInteger())->r(),
                                             mol->atom(k_.asInteger())->r(), mol->atom(l_.asInteger())->r()));
    }

    // Accumulate histogram
    histogram_->accumulate();

    // Update frequency data
    frequency_ = histogram_->accumulatedData();

    // Normalise
    DataOperator1D histogramNormaliser(frequency_);
    // Normalise by value
    histogramNormaliser.normaliseSumTo();

    return NodeConstants::ProcessResult::Success;
}
