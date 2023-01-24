// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "modules/neutronsq/neutronsq.h"
#include "keywords/atomtypevector.h"
#include "keywords/bool.h"
#include "keywords/double.h"
#include "keywords/fileandformat.h"
#include "keywords/isotopologueset.h"
#include "keywords/module.h"
#include "keywords/optionaldouble.h"
#include "modules/sq/sq.h"

NeutronSQModule::NeutronSQModule() : Module("NeutronSQ")
{
    keywords_.addTarget<ModuleKeyword<const SQModule>>(
        "SourceSQs", "Source unweighted S(Q) to transform into neutron-weighted S(Q)", sourceSQ_, "SQ");

    keywords_.setOrganisation("Options", "Isotopes & Normalisation");
    keywords_.add<AtomTypeVectorKeyword>(
        "Exchangeable", "A set of atom types in the system that are exchangeable with each other", exchangeable_);
    keywords_.add<IsotopologueSetKeyword>("Isotopologue", "Set/add an isotopologue and its population for a particular species",
                                          isotopologueSet_);
    keywords_.add<EnumOptionsKeyword<StructureFactors::NormalisationType>>(
        "Normalisation", "Normalisation to apply to total weighted F(Q)", normalisation_,
        StructureFactors::normalisationTypes());

    keywords_.setOrganisation("Options", "Reference Data");
    keywords_.add<FileAndFormatKeyword>("Reference", "F(Q) reference data", referenceFQ_, "EndReference")
        ->setEditSignals({KeywordBase::ReloadExternalData, KeywordBase::RecreateRenderables});
    keywords_
        .add<EnumOptionsKeyword<StructureFactors::NormalisationType>>(
            "ReferenceNormalisation", "Normalisation to remove from reference data before use", referenceNormalisation_,
            StructureFactors::normalisationTypes())
        ->setEditSignals({KeywordBase::ReloadExternalData, KeywordBase::RecreateRenderables});
    keywords_
        .add<OptionalDoubleKeyword>("ReferenceFTQMin",
                                    "Minimum Q value to use when Fourier-transforming reference data (0.0 for no minimum)",
                                    referenceFTQMin_, 0.0, std::nullopt, 0.1, "No Minimum Limit")
        ->setEditSignals({KeywordBase::ReloadExternalData, KeywordBase::RecreateRenderables});
    keywords_
        .add<OptionalDoubleKeyword>("ReferenceFTQMax",
                                    "Maximum Q value to use when Fourier-transforming reference data (0.0 for no maximum)",
                                    referenceFTQMax_, 0.0, std::nullopt, 0.1, "No Maximum Limit")
        ->setEditSignals({KeywordBase::ReloadExternalData, KeywordBase::RecreateRenderables});
    keywords_
        .add<DoubleKeyword>("ReferenceFTDeltaR", "Spacing in r to use when generating the Fourier-transformed data",
                            referenceFTDeltaR_, 1.0e-4, 1.0)
        ->setEditSignals({KeywordBase::ReloadExternalData, KeywordBase::RecreateRenderables});
    keywords_
        .add<EnumOptionsKeyword<WindowFunction::Form>>(
            "ReferenceWindowFunction", "Window function to apply when Fourier-transforming reference S(Q) to g(r)",
            referenceWindowFunction_, WindowFunction::forms())
        ->setEditSignals({KeywordBase::ReloadExternalData, KeywordBase::RecreateRenderables});

    keywords_.setOrganisation("Export");
    keywords_.add<BoolKeyword>("SaveGR", "Save weighted g(r) and G(r)", saveGR_);
    keywords_.add<BoolKeyword>("SaveReference", "Save the reference data and its Fourier transform", saveReference_);
    keywords_.add<BoolKeyword>("SaveRepresentativeGR",
                               "Save representative G(r), obtained from Fourier transform of the calculated F(Q)",
                               saveRepresentativeGR_);
    keywords_.add<BoolKeyword>("SaveSQ", "Save weighted partial and total structure factors", saveSQ_);
}

// Return file and format for reference total F(Q)
const Data1DImportFileFormat &NeutronSQModule::referenceFQFileAndFormat() { return referenceFQ_; }
