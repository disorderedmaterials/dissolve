// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "modules/benchmark/benchmark.h"
#include "keywords/bool.h"
#include "keywords/configuration.h"
#include "keywords/integer.h"

BenchmarkModule::BenchmarkModule() : Module(ModuleTypes::Benchmark)
{
    keywords_.addTarget<ConfigurationKeyword>("Configuration", "Set target configuration for the module", targetConfiguration_);

    keywords_.setOrganisation("Options", "Control");
    keywords_.add<IntegerKeyword>("N", "Number of times to run each benchmark in order to form average", nRepeats_, 1);
    keywords_.add<BoolKeyword>("Save", "Whether to save new timings to the restart file", save_);

    keywords_.setOrganisation("Options", "Tests");
    keywords_.add<BoolKeyword>("TestIntraEnergy", "Whether to benchmark intramolecular energy calculation", testIntraEnergy_);
    keywords_.add<BoolKeyword>("TestInterEnergy", "Whether to benchmark interatomic energy calculation", testInterEnergy_);
    keywords_.add<BoolKeyword>("TestGenerator", "Whether to benchmark configuration generator", testGenerator_);
    keywords_.add<BoolKeyword>("TestRDFCells", "Whether to benchmark the RDF cells method (to half-cell range)", testRDFCells_);
    keywords_.add<BoolKeyword>("TestRDFSimple", "Whether to benchmark the RDF simple method (to half-cell range)",
                               testRDFSimple_);
    keywords_.add<BoolKeyword>("TestDistributors", "Whether to benchmark molecule distributors", testDistributors_);
}
