// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "modules/rdf/rdf.h"
#include "keywords/atomtypevector.h"
#include "keywords/bool.h"
#include "keywords/double.h"
#include "keywords/function1d.h"
#include "keywords/integer.h"
#include "keywords/module.h"

RDFModule::RDFModule() : Module("RDF")
{
    // Targets
    keywords_.addTarget<ConfigurationVectorKeyword>("Configuration", "Set target configuration(s) for the module",
                                                    targetConfigurations_);

    // Control
    keywords_.add<DoubleKeyword>("Control", "BinWidth", "Bin width (spacing in r) to use", binWidth_, 1.0e-3);
    keywords_.add<DoubleKeyword>("Control", "Range", "Maximum r to calculate g(r) out to, unless UseHalfCellRange is true",
                                 requestedRange_, 1.0);
    keywords_.add<BoolKeyword>("Control", "UseHalfCellRange",
                               "Whether to use the maximal RDF range possible that avoids periodic images", useHalfCellRange_);
    keywords_.add<IntegerKeyword>("Control", "Averaging", "Number of historical partial sets to combine into final partials",
                                  averagingLength_, 0);
    keywords_.add<EnumOptionsKeyword<Averaging::AveragingScheme>>("Control", "AveragingScheme",
                                                                  "Weighting scheme to use when averaging partials",
                                                                  averagingScheme_, Averaging::averagingSchemes());
    keywords_.add<Function1DKeyword>("Control", "IntraBroadening", "Type of broadening to apply to intramolecular g(r)",
                                     intraBroadening_, FunctionProperties::Normalisation);
    keywords_.add<EnumOptionsKeyword<RDFModule::PartialsMethod>>("Control", "Method",
                                                                 "Calculation method for partial radial distribution functions",
                                                                 partialsMethod_, RDFModule::partialsMethods());
    keywords_.add<IntegerKeyword>(
        "Control", "Smoothing",
        "Specifies the degree of smoothing 'n' to apply to calculated g(r), where 2n+1 controls the length in "
        "the applied Spline smooth",
        nSmooths_, 0, 100);

    // Test
    keywords_.add<BoolKeyword>(
        "Test", "InternalTest",
        "Perform internal check of calculated partials against a set calculated by a simple unoptimised double-loop",
        internalTest_);

    // Export
    keywords_.add<BoolKeyword>("Export", "Save", "Whether to save partials and total functions to disk", save_);
}

// Return enum option info for NormalisationType
EnumOptions<RDFModule::PartialsMethod> RDFModule::partialsMethods()
{
    return EnumOptions<RDFModule::PartialsMethod>("PartialsMethod", {{RDFModule::AutoMethod, "Auto"},
                                                                     {RDFModule::CellsMethod, "Cells"},
                                                                     {RDFModule::SimpleMethod, "Simple"},
                                                                     {RDFModule::TestMethod, "Test"}});
}
