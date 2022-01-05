// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#include "modules/sq/sq.h"
#include "keywords/bool.h"
#include "keywords/double.h"
#include "keywords/function1d.h"
#include "keywords/integer.h"
#include "keywords/module.h"
#include "modules/bragg/bragg.h"
#include "modules/rdf/rdf.h"

SQModule::SQModule() : Module("SQ")
{
    // Targets
    keywords_.addTarget<ModuleKeyword<const RDFModule>>("SourceRDFs", "Source RDFs to transform into S(Q)", sourceRDF_, "RDF");

    // Control
    keywords_.add<DoubleKeyword>("Control", "QDelta", "Step size in Q for S(Q) calculation", qDelta_, 1.0e-5);
    keywords_.add<DoubleKeyword>("Control", "QMax", "Maximum Q for calculated S(Q)", qMax_, 0.0);
    keywords_.add<DoubleKeyword>("Control", "QMin", "Minimum Q for calculated S(Q)", qMin_, 0.0);
    keywords_.add<Function1DKeyword>("Control", "QBroadening", "Instrument broadening function to apply when calculating S(Q)",
                                     qBroadening_);
    keywords_.add<EnumOptionsKeyword<WindowFunction::Form>>(
        "Control", "WindowFunction", "Window function to apply when Fourier-transforming reference S(Q) to g(r)",
        windowFunction_, WindowFunction::forms());
    keywords_.add<IntegerKeyword>("Control", "Averaging", "Number of historical partial sets to combine into final partials",
                                  averagingLength_, 1);
    keywords_.add<EnumOptionsKeyword<Averaging::AveragingScheme>>("Control", "AveragingScheme",
                                                                  "Weighting scheme to use when averaging partials",
                                                                  averagingScheme_, Averaging::averagingSchemes());

    // Bragg Scattering
    keywords_.add<ModuleKeyword<const BraggModule>>("Bragg Scattering", "IncludeBragg",
                                                    "Include Bragg scattering from specified module", sourceBragg_, "Bragg");
    keywords_.add<Function1DKeyword>("Bragg Scattering", "BraggQBroadening",
                                     "Broadening function to apply to Bragg reflections when generating S(Q)",
                                     braggQBroadening_);

    // Export
    keywords_.add<BoolKeyword>("Export", "Save", "Whether to save partials to disk after calculation", save_);
}

// Return source module for main calculation
const RDFModule *SQModule::sourceRDF() const { return sourceRDF_; }
