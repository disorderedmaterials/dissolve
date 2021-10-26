// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "keywords/types.h"
#include "math/averaging.h"
#include "modules/rdf/rdf.h"

// Return enum option info for NormalisationType
EnumOptions<RDFModule::PartialsMethod> RDFModule::partialsMethods()
{
    return EnumOptions<RDFModule::PartialsMethod>("PartialsMethod", {{RDFModule::AutoMethod, "Auto"},
                                                                     {RDFModule::CellsMethod, "Cells"},
                                                                     {RDFModule::SimpleMethod, "Simple"},
                                                                     {RDFModule::TestMethod, "TestSquaredAverage"}});
}

// Perform any necessary initialisation for the Module
void RDFModule::initialise()
{
    // Control
    keywords_.add<DoubleKeyword>("Control", "BinWidth", "Bin width (spacing in r) to use", binWidth_, 1.0e-3);
    keywords_.add<DoubleKeyword>("Control", "Range", "Maximum r to calculate g(r) out to, unless UseHalfCellRange is true",
                                 requestedRange_, 1.0);
    keywords_.add<BoolKeyword>("Control", "UseHalfCellRange",
                               "Whether to use the maximal RDF range possible that avoids periodic images", useHalfCellRange_);
    keywords_.add<IntegerKeyword>("Control", "Averaging", "Number of historical partial sets to combine into final partials",
                                  averagingLength_, 0);
    keywords_.add(
        "Control",
        new EnumOptionsKeyword<Averaging::AveragingScheme>(Averaging::averagingSchemes() = Averaging::LinearAveraging),
        "AveragingScheme", "Weighting scheme to use when averaging partials");
    keywords_.add("Control", new Function1DKeyword(Functions::Function1DWrapper(), FunctionProperties::Normalisation),
                  "IntraBroadening", "Type of broadening to apply to intramolecular g(r)");
    keywords_.add("Control",
                  new EnumOptionsKeyword<RDFModule::PartialsMethod>(RDFModule::partialsMethods() = RDFModule::AutoMethod),
                  "Method", "Calculation method for partial radial distribution functions");
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
