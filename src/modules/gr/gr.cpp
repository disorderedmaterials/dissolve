// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "modules/gr/gr.h"
#include "keywords/atomTypeVector.h"
#include "keywords/bool.h"
#include "keywords/double.h"
#include "keywords/function1D.h"
#include "keywords/integer.h"
#include "keywords/module.h"
#include "keywords/optionalDouble.h"
#include "keywords/optionalInt.h"

GRModule::GRModule() : Module(ModuleTypes::GR)
{
    keywords_.addTarget<ConfigurationVectorKeyword>("Configurations", "Set target configuration(s) for the module",
                                                    targetConfigurations_);

    keywords_.setOrganisation("Options", "Range");
    keywords_.add<DoubleKeyword>("BinWidth", "Bin width (spacing in r) to use", binWidth_, 1.0e-3);
    keywords_.add<OptionalDoubleKeyword>("Range", "Maximum r to calculate g(r) out to", requestedRange_, 0.0, std::nullopt, 1.0,
                                         "Use Half Cell Range");

    keywords_.setOrganisation("Options", "Averaging & Smoothing");
    keywords_.add<OptionalIntegerKeyword>("Averaging", "Number of historical partial sets to combine into final partials",
                                          averagingLength_, 1, std::nullopt, 1, "Off");
    keywords_.add<EnumOptionsKeyword<Averaging::AveragingScheme>>(
        "AveragingScheme", "Weighting scheme to use when averaging partials", averagingScheme_, Averaging::averagingSchemes());
    keywords_.add<Function1DKeyword>("IntraBroadening", "Type of broadening to apply to intramolecular g(r)", intraBroadening_,
                                     FunctionProperties::Normalisation);
    keywords_.add<OptionalIntegerKeyword>("Smoothing", "Specifies the degree of smoothing to apply to calculated g(r)",
                                          nSmooths_, 0, 100, 1, "Off");

    keywords_.setOrganisation("Export");
    keywords_.add<BoolKeyword>("Save", "Whether to save partials and total functions to disk", save_);
    keywords_.add<BoolKeyword>("SaveOriginal", "Whether to save original (unbroadened) partials and total functions to disk",
                               saveOriginal_);

    keywords_.setOrganisation("Advanced");
    keywords_.add<BoolKeyword>(
        "InternalTest",
        "Perform internal check of calculated partials against a set calculated by a simple unoptimised double-loop",
        internalTest_);
    keywords_.add<EnumOptionsKeyword<GRModule::PartialsMethod>>(
        "Method", "Calculation method for partial radial distribution functions", partialsMethod_, GRModule::partialsMethods());

    // Deprecated
    static bool deprecatedBool_{false};
    keywords_.addDeprecated<BoolKeyword>(
        "UseHalfCellRange", "Whether to use the maximal RDF range possible that avoids periodic images", deprecatedBool_);
}

// Return enum option info for NormalisationType
EnumOptions<GRModule::PartialsMethod> GRModule::partialsMethods()
{
    return EnumOptions<GRModule::PartialsMethod>("PartialsMethod", {{GRModule::AutoMethod, "Auto"},
                                                                    {GRModule::CellsMethod, "Cells"},
                                                                    {GRModule::SimpleMethod, "Simple"},
                                                                    {GRModule::TestMethod, "Test"}});
}
