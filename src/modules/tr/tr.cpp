// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include "modules/tr/tr.h"
#include "keywords/bool.h"
#include "keywords/function1D.h"
#include "keywords/module.h"
#include "keywords/optionalDouble.h"
#include "modules/neutronSQ/neutronSQ.h"

TRModule::TRModule() : Module(ModuleTypes::TR)
{
    keywords_.addTarget<ModuleKeyword<const NeutronSQModule>>(
        "SourceNeutronSQs", "Source NeutronSQ to calculate total T(r) from", sourceNeutronSQ_, ModuleTypes::NeutronSQ);
    keywords_.setOrganisation("Options", "Reference T(r) Broadening & Windowing",
                              "Broadening and windowing to apply when Fourier transforming the S(Q) into g(r).");
    keywords_.add<Function1DKeyword>("QBroadening", "Instrument broadening function to apply when calculating S(Q)",
                                     refQBroadening_);
    keywords_
        .add<OptionalDoubleKeyword>("FTQMin", "Minimum Q value to use when Fourier-transforming S(Q) data (0.0 for no minimum)",
                                    refQMin_, 0.0, std::nullopt, 0.1, "No Minimum Limit")
        ->setEditSignals({KeywordBase::ReloadExternalData, KeywordBase::RecreateRenderables});
    keywords_
        .add<OptionalDoubleKeyword>("FTQMax", "Maximum Q value to use when Fourier-transforming S(Q) data (0.0 for no maximum)",
                                    refQMax_, 0.0, std::nullopt, 0.1, "No Maximum Limit")
        ->setEditSignals({KeywordBase::ReloadExternalData, KeywordBase::RecreateRenderables});
    keywords_
        .add<EnumOptionsKeyword<WindowFunction::Form>>("WindowFunction",
                                                       "Window function to apply when Fourier-transforming S(Q) to g(r)",
                                                       refWindowFunction_, WindowFunction::forms())
        ->setEditSignals({KeywordBase::ReloadExternalData, KeywordBase::RecreateRenderables});
    keywords_.setOrganisation("Options", "Representative T(r) Broadening & Windowing",
                              "Broadening and windowing to apply when Fourier transforming the S(Q) into g(r).");
    keywords_.add<Function1DKeyword>("RepQBroadening", "Instrument broadening function to apply when calculating S(Q)",
                                     repQBroadening_);
    keywords_
        .add<OptionalDoubleKeyword>("RepFTQMin",
                                    "Minimum Q value to use when Fourier-transforming S(Q) data (0.0 for no minimum)", repQMin_,
                                    0.0, std::nullopt, 0.1, "No Minimum Limit")
        ->setEditSignals({KeywordBase::ReloadExternalData, KeywordBase::RecreateRenderables});
    keywords_
        .add<OptionalDoubleKeyword>("RepFTQMax",
                                    "Maximum Q value to use when Fourier-transforming S(Q) data (0.0 for no maximum)", repQMax_,
                                    0.0, std::nullopt, 0.1, "No Maximum Limit")
        ->setEditSignals({KeywordBase::ReloadExternalData, KeywordBase::RecreateRenderables});
    keywords_
        .add<EnumOptionsKeyword<WindowFunction::Form>>("RepWindowFunction",
                                                       "Window function to apply when Fourier-transforming S(Q) to g(r)",
                                                       repWindowFunction_, WindowFunction::forms())
        ->setEditSignals({KeywordBase::ReloadExternalData, KeywordBase::RecreateRenderables});
    keywords_.setOrganisation("Export");
    keywords_.add<BoolKeyword>("SaveTR", "Save weighted g(r) and G(r)", saveTR_);
}
