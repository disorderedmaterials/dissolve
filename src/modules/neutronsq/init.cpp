// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#include "keywords/types.h"
#include "modules/neutronsq/neutronsq.h"
#include "modules/sq/sq.h"

// Perform any necessary initialisation for the Module
void NeutronSQModule::initialise()
{
    // Calculation
    keywords_.add("Calculation", new ModuleKeyword<const SQModule>("SQ"), "SourceSQs",
                  "Source unweighted S(Q) to transform into neutron-weighted S(Q)");
    keywords_.add("Calculation", new AtomTypeSelectionKeyword(exchangeableTypes_, targetConfigurations_), "Exchangeable",
                  "Specify AtomTypes that are exchangeable", "<AtomType> [AtomType...]");
    keywords_.add("Calculation", new IsotopologueSetKeyword(isotopologues_), "Isotopologue",
                  "Set Isotopologue (and its population) to use for a particular Species");
    keywords_.add("Calculation",
                  new EnumOptionsKeyword<StructureFactors::NormalisationType>(StructureFactors::normalisationTypes() =
                                                                                  StructureFactors::NoNormalisation),
                  "Normalisation", "Normalisation to apply to total weighted F(Q)");

    // Reference Data
    keywords_.add("Reference Data", new FileAndFormatKeyword(referenceFQ_, "EndReference"), "Reference", "F(Q) reference data",
                  "<format> <filename>", KeywordBase::ModificationRequiresSetUpOption);
    keywords_.add("Reference Data",
                  new EnumOptionsKeyword<StructureFactors::NormalisationType>(StructureFactors::normalisationTypes() =
                                                                                  StructureFactors::NoNormalisation),
                  "ReferenceNormalisation", "Normalisation to remove from reference data before use",
                  KeywordBase::ModificationRequiresSetUpOption);
    keywords_.add("Reference Data", new WindowFunctionKeyword(WindowFunction(WindowFunction::Lorch0Window)),
                  "ReferenceWindowFunction", "Window function to apply when Fourier-transforming reference S(Q) to g(r)",
                  KeywordBase::ModificationRequiresSetUpOption);

    // Export
    keywords_.add("Export", new BoolKeyword(false), "SaveGR",
                  "Whether to save weighted g(r) and G(r) to disk after calculation", "<True|False>");
    keywords_.add("Export", new BoolKeyword(false), "SaveReference",
                  "Whether to save the reference data and its Fourier transform", "<True|False>");
    keywords_.add("Export", new BoolKeyword(false), "SaveSQ",
                  "Whether to save weighted S(Q) and F(Q) to disk after calculation", "<True|False>");
}

// Return file and format for reference total F(Q)
const Data1DImportFileFormat &NeutronSQModule::referenceFQFileAndFormat() { return referenceFQ_; }
