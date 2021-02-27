// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "keywords/types.h"
#include "modules/neutronsq/neutronsq.h"
#include "modules/sq/sq.h"

// Perform any necessary initialisation for the Module
void NeutronSQModule::initialise()
{
    // Control
    keywords_.add("Control", new ModuleKeyword<const SQModule>("SQ"), "SourceSQs",
                  "Source unweighted S(Q) to transform into neutron-weighted S(Q)");
    keywords_.add("Control", new AtomTypeSelectionKeyword(exchangeableTypes_, targetConfigurations_), "Exchangeable",
                  "A list of one or more atom types in the system that are exchangeable with each other",
                  "<AtomType> [AtomType...]");
    keywords_.add("Control", new IsotopologueSetKeyword(isotopologues_), "Isotopologue",
                  "Set/add an isotopologue and its population for a particular species");
    keywords_.add("Control",
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
    keywords_.add("Reference Data",
                  new EnumOptionsKeyword<WindowFunction::Form>(WindowFunction::forms() = WindowFunction::Form::Lorch0),
                  "ReferenceWindowFunction", "Window function to apply when Fourier-transforming reference S(Q) to g(r)",
                  KeywordBase::ModificationRequiresSetUpOption);

    // Export
    keywords_.add("Export", new BoolKeyword(false), "SaveGR", "Save weighted g(r) and G(r)", "<True|False>");
    keywords_.add("Export", new BoolKeyword(false), "SaveReference", "Save the reference data and its Fourier transform",
                  "<True|False>");
    keywords_.add("Export", new BoolKeyword(false), "SaveSQ", "Save weighted partial and total structure factors",
                  "<True|False>");
}

// Return file and format for reference total F(Q)
const Data1DImportFileFormat &NeutronSQModule::referenceFQFileAndFormat() { return referenceFQ_; }
