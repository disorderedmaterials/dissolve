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
    keywords_.add("Control", new DoubleKeyword(0.025, 0.001), "BinWidth", "Bin width (spacing in r) to use");
    keywords_.add("Control", new DoubleKeyword(15.0, 0.1), "Range",
                  "Maximum r to calculate g(r) out to, unless UseHalfCellRange is true");
    keywords_.add("Control", new BoolKeyword(true), "UseHalfCellRange",
                  "Whether to use the maximal RDF range possible that avoids periodic images", "<True|False>");
    keywords_.add("Control", new IntegerKeyword(5, 0), "Averaging",
                  "Number of historical partial sets to combine into final partials", "<5>");
    keywords_.add(
        "Control",
        new EnumOptionsKeyword<Averaging::AveragingScheme>(Averaging::averagingSchemes() = Averaging::LinearAveraging),
        "AveragingScheme", "Weighting scheme to use when averaging partials", "<Linear>");
    keywords_.add("Control", new PairBroadeningFunctionKeyword(PairBroadeningFunction()), "IntraBroadening",
                  "Type of broadening to apply to intramolecular g(r)");
    keywords_.add("Control",
                  new EnumOptionsKeyword<RDFModule::PartialsMethod>(RDFModule::partialsMethods() = RDFModule::AutoMethod),
                  "Method", "Calculation method for partial radial distribution functions");
    keywords_.add("Control", new IntegerKeyword(0, 0, 100), "Smoothing",
                  "Specifies the degree of smoothing 'n' to apply to calculated g(r), where 2n+1 controls the length in "
                  "the applied Spline smooth");

    // Test
    keywords_.add("Test", new BoolKeyword(false), "InternalTest",
                  "Perform internal check of calculated partials against a set calculated by a simple unoptimised double-loop",
                  "<True|False>");
    keywords_.add("Test", new BoolKeyword(false), "Test",
                  "Test calculated total and partials against reference data (specified with `TestReference`)", "<True|False>");
    keywords_.add("Test", new Data1DStoreKeyword(testData_), "TestReference", "Test reference data",
                  "<target> <fileformat> <filename> [x=1] [y=2]");
    keywords_.add("Test", new DoubleKeyword(0.1, 1.0e-5), "TestThreshold", "Test threshold (%error) above which test fails",
                  "<threshold[0.1]>");

    // Export
    keywords_.add("Export", new BoolKeyword(false), "Save", "Whether to save partials and total functions to disk",
                  "<True|False>");
}
