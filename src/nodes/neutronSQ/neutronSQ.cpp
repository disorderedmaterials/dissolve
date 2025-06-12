// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include "nodes/neutronSQ/neutronSQ.h"
#include "keywords/atomTypeVector.h"
#include "keywords/bool.h"
#include "keywords/double.h"
#include "keywords/fileAndFormat.h"
#include "keywords/isotopologueSet.h"
#include "keywords/module.h"
#include "keywords/optionalDouble.h"
#include "nodes/sq/sq.h"

NeutronSQNode::NeutronSQNode(Graph *parentGraph) : Node(parentGraph)
{
    addInput<GRNode::SpeciesPopulations *>("SpeciesPopulations", "Species populations", speciesPopulations_);
    addInput<PartialSet *>("UnweightedSQ", "Unweighted partials for target configuration", unweightedSQ_);
    addInput<PartialSet *>("UnweightedGR", "Unweighted partials for target configuration", unweightedGR_);
    addInput<IsotopologueSet>("Isotopologue", "Set/add an isotopologue and its population for a particular species",
                              isotopologueSet_);
    addInput<Data1DImportFileFormat>("Reference", "F(Q) reference data", referenceFQ_);
    /*
    addOption<std::vector<std::shared_ptr<AtomType>>>(
        "Exchangeable", "A set of atom types in the system that are exchangeable with each other", exchangeable_);

    addOption<IsotopologueSet>("Isotopologue", "Set/add an isotopologue and its population for a particular species",
                               isotopologueSet_);
    addOption<Data1DImportFileFormat>("Reference", "F(Q) reference data", referenceFQ_);
    */
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
}

// Return file and format for reference total F(Q)
const Data1DImportFileFormat &NeutronSQNode::referenceFQFileAndFormat() { return referenceFQ_; }
