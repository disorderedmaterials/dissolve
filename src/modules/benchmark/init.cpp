// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "keywords/types.h"
#include "modules/benchmark/benchmark.h"

// Perform any necessary initialisation for the Module
void BenchmarkModule::initialise()
{
    // Targets
    keywords_.addTarget<ConfigurationVectorKeyword>("Configuration", "Set target configuration(s) for the module",
                                                    targetConfigurations_);

    // Control
    keywords_.add<IntegerKeyword>("Control", "N", "Number of times to run each benchmark in order to form average", nRepeats_,
                                  1);
    keywords_.add<BoolKeyword>("Control", "Save", "Whether to save new timings to the restart file", save_);
    keywords_.add<BoolKeyword>("Tests", "TestIntraEnergy", "Whether to benchmark intramolecular energy calculation",
                               testIntraEnergy_);
    keywords_.add<BoolKeyword>("Tests", "TestInterEnergy", "Whether to benchmark interatomic energy calculation",
                               testInterEnergy_);
    keywords_.add<BoolKeyword>("Tests", "TestGenerator", "Whether to benchmark configuration generator", testGenerator_);
    keywords_.add<BoolKeyword>("Tests", "TestRDFCells", "Whether to benchmark the RDF cells method (to half-cell range)",
                               testRDFCells_);
    keywords_.add<BoolKeyword>("Tests", "TestRDFSimple", "Whether to benchmark the RDF simple method (to half-cell range)",
                               testRDFSimple_);
    keywords_.add<BoolKeyword>("Tests", "TestDistributors", "Whether to benchmark molecule distributors", testDistributors_);
}
