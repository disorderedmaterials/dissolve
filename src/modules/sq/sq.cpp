// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "modules/sq/sq.h"
#include "keywords/bool.h"
#include "keywords/double.h"
#include "keywords/function1D.h"
#include "keywords/module.h"
#include "keywords/optionalInt.h"
#include "modules/bragg/bragg.h"
#include "modules/gr/gr.h"

SQModule::SQModule() : Module(ModuleTypes::SQ)
{
    keywords_.addTarget<ModuleKeyword<const GRModule>>("SourceGR", "Source G(r) to transform into S(Q)", sourceGR_,
                                                       ModuleTypes::GR);

    keywords_.setOrganisation("Options", "Range", "Range over which to calculate the total and partial structure factors.");
    keywords_.add<DoubleKeyword>("QMin", "Minimum Q for calculated S(Q)", qMin_, 0.0);
    keywords_.add<DoubleKeyword>("QMax", "Maximum Q for calculated S(Q)", qMax_, 0.0);
    keywords_.add<DoubleKeyword>("QDelta", "Step size in Q for S(Q) calculation", qDelta_, 1.0e-5);

    keywords_.setOrganisation("Options", "Broadening & Windowing",
                              "Broadening and windowing to apply when Fourier transforming the g(r) into S(Q).");
    keywords_.add<Function1DKeyword>("QBroadening", "Instrument broadening function to apply when calculating S(Q)",
                                     qBroadening_);
    keywords_.add<EnumOptionsKeyword<WindowFunction::Form>>(
        "WindowFunction", "Window function to apply when Fourier-transforming reference S(Q) to g(r)", windowFunction_,
        WindowFunction::forms());

    keywords_.setOrganisation(
        "Options", "Bragg Scattering",
        "Control whether output from a Bragg module should be included in the calculated structure factors.");
    keywords_.add<ModuleKeyword<const BraggModule>>("IncludeBragg", "Include Bragg scattering from specified module",
                                                    sourceBragg_, ModuleTypes::Bragg);
    keywords_.add<Function1DKeyword>(
        "BraggQBroadening", "Broadening function to apply to Bragg reflections when generating S(Q)", braggQBroadening_);

    keywords_.setOrganisation("Options", "Averaging");
    keywords_.add<OptionalIntegerKeyword>("Averaging", "Number of historical partial sets to combine into final partials",
                                          averagingLength_, 1, std::nullopt, 1, "Off");
    keywords_.add<EnumOptionsKeyword<Averaging::AveragingScheme>>(
        "AveragingScheme", "Weighting scheme to use when averaging partials", averagingScheme_, Averaging::averagingSchemes());

    keywords_.setOrganisation("Export");
    keywords_.add<BoolKeyword>("Save", "Whether to save partials to disk after calculation", save_);

    // Deprecated
    keywords_.addDeprecated<ModuleKeyword<const GRModule>>("SourceRDFs", "Source RDFs to transform into S(Q)", sourceGR_,
                                                           ModuleTypes::GR);
}

// Return source G(r) for main calculation
const GRModule *SQModule::sourceGR() const { return sourceGR_; }
