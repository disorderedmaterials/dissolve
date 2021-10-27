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
    keywords_.add<AtomTypeVectorKeyword>(
        "Control", "Exchangeable", "A set of atom types in the system that are exchangeable with each other", exchangeable_);
    keywords_.add("Control", new IsotopologueSetKeyword(), "Isotopologue",
                  "Set/add an isotopologue and its population for a particular species");
    keywords_.add("Control",
                  new EnumOptionsKeyword<StructureFactors::NormalisationType>(StructureFactors::normalisationTypes() =
                                                                                  StructureFactors::NoNormalisation),
                  "Normalisation", "Normalisation to apply to total weighted F(Q)");

    // Reference Data
    keywords_.add("Reference Data", new FileAndFormatKeyword(referenceFQ_, "EndReference"), "Reference", "F(Q) reference data",
                  KeywordBase::ModificationRequiresSetUpOption);
    keywords_.add("Reference Data",
                  new EnumOptionsKeyword<StructureFactors::NormalisationType>(StructureFactors::normalisationTypes() =
                                                                                  StructureFactors::NoNormalisation),
                  "ReferenceNormalisation", "Normalisation to remove from reference data before use",
                  KeywordBase::ModificationRequiresSetUpOption);
    keywords_.add<DoubleKeyword>("Reference Data", "ReferenceFTQMin",
                                 "Set the minimum Q value to use when Fourier-transforming the data", referenceFTQMin_);
    keywords_.add<DoubleKeyword>("Reference Data", "ReferenceFTQMax",
                                 "Set the maximum Q value to use when Fourier-transforming the data", referenceFTQMax_);
    keywords_.add<DoubleKeyword>("Reference Data", "ReferenceFTDeltaR",
                                 "Set the spacing in r to use when generating the Fourier-transformed data",
                                 referenceFTDeltaR_);
    keywords_.add("Reference Data",
                  new EnumOptionsKeyword<WindowFunction::Form>(WindowFunction::forms() = WindowFunction::Form::Lorch0),
                  "ReferenceWindowFunction", "Window function to apply when Fourier-transforming reference S(Q) to g(r)",
                  KeywordBase::ModificationRequiresSetUpOption);

    // Export
    keywords_.add<BoolKeyword>("Export", "SaveGR", "Save weighted g(r) and G(r)", saveGR_);
    keywords_.add<BoolKeyword>("Export", "SaveReference", "Save the reference data and its Fourier transform", saveReference_);
    keywords_.add<BoolKeyword>("Export", "SaveRepresentativeGR",
                               "Save representative G(r), obtained from Fourier transform of the calculated F(Q)",
                               saveRepresentativeGR_);
    keywords_.add<BoolKeyword>("Export", "SaveSQ", "Save weighted partial and total structure factors", saveSQ_);
}

// Return file and format for reference total F(Q)
const Data1DImportFileFormat &NeutronSQModule::referenceFQFileAndFormat() { return referenceFQ_; }
