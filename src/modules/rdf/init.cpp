/*
    *** RDF Module - Initialisation
    *** src/modules/rdf/init.cpp
    Copyright T. Youngs 2012-2020

    This file is part of Dissolve.

    Dissolve is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Dissolve is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Dissolve.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "keywords/types.h"
#include "math/averaging.h"
#include "modules/rdf/rdf.h"

// Return enum option info for NormalisationType
EnumOptions<RDFModule::PartialsMethod> RDFModule::partialsMethods()
{
    static EnumOptionsList PartialsMethodOptions = EnumOptionsList() << EnumOption(RDFModule::AutoMethod, "Auto")
                                                                     << EnumOption(RDFModule::CellsMethod, "Cells")
                                                                     << EnumOption(RDFModule::SimpleMethod, "Simple")
                                                                     << EnumOption(RDFModule::TestMethod, "TestSquaredAverage");

    static EnumOptions<RDFModule::PartialsMethod> options("PartialsMethod", PartialsMethodOptions, RDFModule::AutoMethod);

    return options;
}

// Perform any necessary initialisation for the Module
void RDFModule::initialise()
{
    // Calculation
    keywords_.add("Calculation", new DoubleKeyword(0.025, 0.001), "BinWidth",
                  "Specific RDF range to use (if UseHalfCellRange == false)");
    keywords_.add("Calculation", new DoubleKeyword(15.0, 0.1), "Range",
                  "Specific RDF range to use (if UseHalfCellRange == false)");
    keywords_.add("Calculation", new BoolKeyword(true), "UseHalfCellRange",
                  "Use the maximal RDF range possible, avoiding periodic images", "<True|False>");
    keywords_.add("Calculation", new IntegerKeyword(5, 1), "Averaging",
                  "Number of historical partial sets to combine into final partials", "<5>");
    keywords_.add(
        "Calculation",
        new EnumOptionsKeyword<Averaging::AveragingScheme>(Averaging::averagingSchemes() = Averaging::LinearAveraging),
        "AveragingScheme", "Weighting scheme to use when averaging partials", "<Linear>");
    keywords_.add("Calculation", new PairBroadeningFunctionKeyword(PairBroadeningFunction()), "IntraBroadening",
                  "Type of broadening to apply to intramolecular g(r)");
    keywords_.add("Calculation",
                  new EnumOptionsKeyword<RDFModule::PartialsMethod>(RDFModule::partialsMethods() = RDFModule::AutoMethod),
                  "Method", "Calculation method for partial radial distribution functions");
    keywords_.add("Calculation", new IntegerKeyword(0, 0, 100), "Smoothing",
                  "Specifies the degree of smoothing 'n' to apply to calculated g(r), where 2n+1 controls the length in "
                  "the applied Spline smooth");

    // Test
    keywords_.add("Test", new BoolKeyword(false), "InternalTest",
                  "Perform internal check of calculated partials (relative to Test method)");
    keywords_.add("Test", new BoolKeyword(false), "Test", "Test calculated total and partials against supplied reference data",
                  "<True|False>");
    keywords_.add("Test", new Data1DStoreKeyword(testData_), "TestReference", "Specify test reference data",
                  "<target> <fileformat> <filename> [x=1] [y=2]");
    keywords_.add("Test", new DoubleKeyword(0.1, 1.0e-5), "TestThreshold", "Test threshold (%error) above which test fails",
                  "<threshold[0.1]>");

    // Export
    keywords_.add("Export", new BoolKeyword(false), "Save", "Whether to save partials to disk after calculation",
                  "<True|False>");
}
