// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "keywords/types.h"
#include "math/averaging.h"
#include "modules/bragg/bragg.h"
#include "modules/rdf/rdf.h"
#include "modules/sq/sq.h"

// Perform any necessary initialisation for the Module
void SQModule::initialise()
{
    // Control
    keywords_.add("Control", new ModuleKeyword<const RDFModule>("RDF"), "SourceRDFs", "Source RDFs to transform into S(Q)");
    keywords_.add<DoubleKeyword>("Control", "QDelta", "Step size in Q for S(Q) calculation", qDelta_, 1.0e-5);
    keywords_.add<DoubleKeyword>("Control", "QMax", "Maximum Q for calculated S(Q)", qMax_, 0.0);
    keywords_.add<DoubleKeyword>("Control", "QMin", "Minimum Q for calculated S(Q)", qMin_, 0.0);
    keywords_.add("Control", new Function1DKeyword({Functions::Function1D::GaussianC2, {0.0, 0.02}}), "QBroadening",
                  "Instrument broadening function to apply when calculating S(Q)");
    keywords_.add("Control", new EnumOptionsKeyword<WindowFunction::Form>(WindowFunction::forms()), "WindowFunction",
                  "Window function to apply in Fourier-transform of g(r) to S(Q)");
    keywords_.add<IntegerKeyword>("Control", "Averaging", "Number of historical partial sets to combine into final partials",
                                  averagingLength_, 1);
    keywords_.add(
        "Control",
        new EnumOptionsKeyword<Averaging::AveragingScheme>(Averaging::averagingSchemes() = Averaging::LinearAveraging),
        "AveragingScheme", "Weighting scheme to use when averaging partials");

    // Bragg Scattering
    keywords_.add("Bragg Scattering", new ModuleKeyword<const BraggModule>("Bragg"), "IncludeBragg",
                  "Include Bragg scattering from specified module");
    keywords_.add("Bragg Scattering", new Function1DKeyword, "BraggQBroadening",
                  "Broadening function to apply to Bragg reflections when generating S(Q)");

    // Export
    keywords_.add<BoolKeyword>("Export", "Save", "Whether to save partials to disk after calculation", save_);
}
