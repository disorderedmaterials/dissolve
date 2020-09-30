// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#include "keywords/types.h"
#include "modules/sq/sq.h"
#include "modules/xraysq/xraysq.h"

// Return enum option info for NormalisationType
EnumOptions<XRaySQModule::NormalisationType> XRaySQModule::normalisationTypes()
{
    static EnumOptionsList NormalisationTypeOptions =
        EnumOptionsList() << EnumOption(XRaySQModule::NoNormalisation, "None")
                          << EnumOption(XRaySQModule::AverageOfSquaresNormalisation, "AverageOfSquares")
                          << EnumOption(XRaySQModule::SquareOfAverageNormalisation, "SquareOfAverage");

    static EnumOptions<XRaySQModule::NormalisationType> options("NormalisationType", NormalisationTypeOptions,
                                                                XRaySQModule::NoNormalisation);

    return options;
}

// Perform any necessary initialisation for the Module
void XRaySQModule::initialise()
{
    // Calculation
    keywords_.add("Calculation", new ModuleKeyword<const SQModule>("SQ"), "SourceSQs",
                  "Source unweighted S(Q) to transform into xray-weighted S(Q)");
    keywords_.add("Calculation", new DoubleKeyword(0.05, 1.0e-5), "QDelta", "Step size in Q for S(Q) calculation");
    keywords_.add("Calculation", new DoubleKeyword(-1.0, -1.0), "QMax",
                  "Maximum Q for calculated S(Q) (and limit at which reference data will be truncated)");
    keywords_.add("Calculation", new DoubleKeyword(0.01, 0.0), "QMin", "Minimum Q for calculated S(Q)");
    keywords_.add("Calculation", new BroadeningFunctionKeyword(BroadeningFunction()), "QBroadening",
                  "Broadening function to apply when calculating S(Q)");
    keywords_.add("Calculation",
                  new EnumOptionsKeyword<XRayFormFactors::XRayFormFactorData>(XRayFormFactors::xRayFormFactorData() =
                                                                                  XRayFormFactors::WaasmaierKirfel1995),
                  "FormFactors", "Form factors to use for weighting");
    keywords_.add("Calculation",
                  new EnumOptionsKeyword<XRaySQModule::NormalisationType>(XRaySQModule::normalisationTypes() =
                                                                              XRaySQModule::NoNormalisation),
                  "Normalisation", "Normalisation to apply to total weighted F(Q)");
    keywords_.add("Calculation", new WindowFunctionKeyword(WindowFunction(WindowFunction::NoWindow)), "WindowFunction",
                  "Window function to apply when Fourier-transforming g(r) to S(Q)");

    // Bragg Scattering
    keywords_.add("Bragg Scattering", new BoolKeyword(false), "IncludeBragg",
                  "Include Bragg scattering (if reflection data are present in the Configuration)");
    keywords_.add("Bragg Scattering", new BroadeningFunctionKeyword(BroadeningFunction()), "BraggQBroadening",
                  "Broadening function to apply, on top of any QBroadening, to Bragg scattering");

    // Reference Data
    keywords_.add("Reference Data", new FileAndFormatKeyword(referenceFQ_, "EndReference"), "Reference", "F(Q) reference data",
                  "<format> <filename>", KeywordBase::ModificationRequiresSetUpOption);
    keywords_.add("Reference Data", new BoolKeyword(false), "ReferenceIgnoreFirst",
                  "Ignore the first point in the supplied reference data", KeywordBase::ModificationRequiresSetUpOption);
    keywords_.add("Reference Data", new WindowFunctionKeyword(WindowFunction(WindowFunction::Lorch0Window)),
                  "ReferenceWindowFunction", "Window function to apply when Fourier-transforming reference S(Q) to g(r)",
                  KeywordBase::ModificationRequiresSetUpOption);

    // Export
    keywords_.add("Export", new BoolKeyword(false), "SaveFormFactors",
                  "Whether to save combined form factor weightings for atomtype pairs", "<True|False>");
    keywords_.add("Export", new BoolKeyword(false), "SaveReference",
                  "Whether to save the reference data and its Fourier transform", "<True|False>");
    keywords_.add("Export", new BoolKeyword(false), "SaveUnweighted",
                  "Whether to save unweighted totals / partials to disk after calculation", "<True|False>");
    keywords_.add("Export", new BoolKeyword(false), "SaveWeighted",
                  "Whether to save weighted totals / partials to disk after calculation", "<True|False>");
}

// Return file and format for reference total F(Q)
const Data1DImportFileFormat &XRaySQModule::referenceFQFileAndFormat() { return referenceFQ_; }
