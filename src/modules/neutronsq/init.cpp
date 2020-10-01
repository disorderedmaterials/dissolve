// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#include "keywords/types.h"
#include "modules/neutronsq/neutronsq.h"
#include "modules/sq/sq.h"

// Return enum option info for NormalisationType
EnumOptions<NeutronSQModule::NormalisationType> NeutronSQModule::normalisationTypes()
{
    static EnumOptionsList NormalisationTypeOptions =
        EnumOptionsList() << EnumOption(NeutronSQModule::NoNormalisation, "None")
                          << EnumOption(NeutronSQModule::AverageOfSquaresNormalisation, "AverageSquared")
                          << EnumOption(NeutronSQModule::SquareOfAverageNormalisation, "SquaredAverage");

    static EnumOptions<NeutronSQModule::NormalisationType> options("NormalisationType", NormalisationTypeOptions,
                                                                   NeutronSQModule::NoNormalisation);

    return options;
}

// Perform any necessary initialisation for the Module
void NeutronSQModule::initialise()
{
    // Calculation
    keywords_.add("Calculation", new ModuleKeyword<const SQModule>("SQ"), "SourceSQs",
                  "Source unweighted S(Q) to transform into neutron-weighted S(Q)");
    keywords_.add("Calculation", new AtomTypeSelectionKeyword(exchangeableTypes_, targetConfigurations_), "Exchangeable",
                  "Specify AtomTypes that are exchangeable", "<AtomType> [AtomType...]");
    keywords_.add("Calculation", new IsotopologueCollectionKeyword(isotopologues_, targetConfigurations()), "Isotopologue",
                  "Set Isotopologue (and its population) to use for a particular Species in a given Configuration");
    keywords_.add("Calculation",
                  new EnumOptionsKeyword<NeutronSQModule::NormalisationType>(NeutronSQModule::normalisationTypes() =
                                                                                 NeutronSQModule::NoNormalisation),
                  "Normalisation", "Normalisation to apply to total weighted F(Q)");

    // Reference Data
    keywords_.add("Reference Data", new FileAndFormatKeyword(referenceFQ_, "EndReference"), "Reference", "F(Q) reference data",
                  "<format> <filename>", KeywordBase::ModificationRequiresSetUpOption);
    keywords_.add("Reference Data",
                  new EnumOptionsKeyword<NeutronSQModule::NormalisationType>(NeutronSQModule::normalisationTypes() =
                                                                                 NeutronSQModule::NoNormalisation),
                  "ReferenceNormalisation", "Normalisation to remove from reference data before use",
                  KeywordBase::ModificationRequiresSetUpOption);
    keywords_.add("Reference Data", new BoolKeyword(false), "ReferenceIgnoreFirst",
                  "Ignore the first point in the supplied reference data", KeywordBase::ModificationRequiresSetUpOption);
    keywords_.add("Reference Data", new WindowFunctionKeyword(WindowFunction(WindowFunction::Lorch0Window)),
                  "ReferenceWindowFunction", "Window function to apply when Fourier-transforming reference S(Q) to g(r)",
                  KeywordBase::ModificationRequiresSetUpOption);

    // Export
    keywords_.add("Export", new BoolKeyword(false), "SaveReference",
                  "Whether to save the reference data and its Fourier transform", "<True|False>");
    keywords_.add("Export", new BoolKeyword(false), "SaveUnweighted",
                  "Whether to save unweighted totals / partials to disk after calculation", "<True|False>");
    keywords_.add("Export", new BoolKeyword(false), "SaveWeighted",
                  "Whether to save weighted totals / partials to disk after calculation", "<True|False>");
}

// Return file and format for reference total F(Q)
const Data1DImportFileFormat &NeutronSQModule::referenceFQFileAndFormat() { return referenceFQ_; }
