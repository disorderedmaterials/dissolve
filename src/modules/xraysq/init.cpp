// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "keywords/types.h"
#include "modules/sq/sq.h"
#include "modules/xraysq/xraysq.h"

// Perform any necessary initialisation for the Module
void XRaySQModule::initialise()
{
    // Control
    keywords_.add("Control", new ModuleKeyword<const SQModule>("SQ"), "SourceSQs",
                  "Source unweighted S(Q) to transform into xray-weighted S(Q)");
    keywords_.add("Control",
                  new EnumOptionsKeyword<XRayFormFactors::XRayFormFactorData>(XRayFormFactors::xRayFormFactorData() =
                                                                                  XRayFormFactors::WaasmaierKirfel1995),
                  "FormFactors", "Atomic form factors to use for weighting");
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
                  "ReferenceNormalisation", "Normalisation to remove from reference data");
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
    keywords_.add<BoolKeyword>("Export", "SaveFormFactors",
                               "Whether to save combined form factor weightings for atomtype pairs", saveFormFactors_);
    keywords_.add<BoolKeyword>("Export", "SaveGR", "Whether to save weighted g(r) and G(r) to disk after calculation", saveGR_);
    keywords_.add<BoolKeyword>("Export", "SaveReference", "Whether to save the reference data and its Fourier transform",
                               saveReference_);
    keywords_.add<BoolKeyword>("Export", "SaveRepresentativeGR",
                               "Save representative G(r), obtained from Fourier transform of the calculated F(Q)",
                               saveRepresentativeGR_);
    keywords_.add<BoolKeyword>("Export", "SaveSQ", "Whether to save weighted S(Q) and F(Q) to disk after calculation", saveSQ_);
}

// Return file and format for reference total F(Q)
const Data1DImportFileFormat &XRaySQModule::referenceFQFileAndFormat() { return referenceFQ_; }
