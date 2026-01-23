// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodes/neutronSQ/neutronSQ.h"
#include "nodes/sq/sq.h"

NeutronSQNode::NeutronSQNode(Graph *parentGraph) : Node(parentGraph)
{
    // Inputs
    addInput<PartialSet *>("UnweightedSQ", "Unweighted partial S(Q)", unweightedSQ_);
    addInput<PartialSet *>("UnweightedGR", "Unweighted partials g(r)", unweightedGR_);
    addInput<std::optional<Data1D>>("ReferenceData", "Reference F(Q) data", referenceFQ_);

    // Outputs
    addOptionalPointerOutput<PartialSet>("WeightedSQ", "Weighted partial structure factors for target configuration",
                                         weightedSQ_);
    addOptionalPointerOutput<PartialSet>(
        "WeightedGR", "Weighted partial radial distribution functions for target configuration", weightedGR_);
    addOutput<Data1D>("ReferenceGR", "Fourier transform of reference data", referenceGR_);

    // Options
    addOption<IsotopologueSet>("Isotopologues", "Isotopologues to use when calculating weights matrix", isotopologues_);
    addOption<StructureFactors::NormalisationType>("NormaliseTo", "Normalisation to apply to total weighted F(Q)",
                                                   normaliseTo_);
    addOption<StructureFactors::NormalisationType>(
        "ReferenceNormalisedTo", "Normalisation that has been applied to the reference data", referenceNormalisedTo_);
    addOption<std::optional<double>>("ReferenceFTQMin",
                                     "Minimum Q value to use when Fourier-transforming reference data (0.0 for no minimum)",
                                     referenceFTQMin_);
    addOption<std::optional<double>>("ReferenceFTQMax",
                                     "Maximum Q value to use when Fourier-transforming reference data (0.0 for no maximum)",
                                     referenceFTQMax_);
    addOption<double>("ReferenceFTDeltaR", "Spacing in r to use when generating the Fourier-transformed data",
                      referenceFTDeltaR_);
    addOption<WindowFunction::Form>("ReferenceWindowFunction",
                                    "Window function to apply when Fourier-transforming reference S(Q) to g(r)",
                                    referenceWindowFunction_);
    addOption<bool>("SaveGR", "Save weighted g(r) and G(r)", saveGR_);
    addOption<bool>("SaveReference", "Save the reference data and its Fourier transform", saveReference_);
    addOption<bool>("SaveRepresentativeGR", "Save representative G(r), obtained from Fourier transform of the calculated F(Q)",
                    saveRepresentativeGR_);
    addOption<bool>("SaveSQ", "Save weighted partial and total structure factors", saveSQ_);

    // Serialisables
    addSerialisable("weightedGR", weightedGR_);
    addSerialisable("weightedSQ", weightedSQ_);
    addSerialisable("representativeGR", representativeGR_);
}

std::string_view NeutronSQNode::type() const { return "NeutronSQ"; }

std::string_view NeutronSQNode::summary() const { return "Calculate neutron-weighted S(Q) and G(r)"; }
