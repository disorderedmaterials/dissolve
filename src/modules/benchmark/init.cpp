// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "keywords/types.h"
#include "modules/benchmark/benchmark.h"

// Perform any necessary initialisation for the Module
void BenchmarkModule::initialise()
{
    keywords_.add("Control", new IntegerKeyword(5, 1, 1000), "N",
                  "Number of times to run each benchmark in order to form average");
    keywords_.add("Control", new BoolKeyword(true), "Save", "Whether to save new timings to the restart file");
    keywords_.add("Tests", new BoolKeyword(true), "TestIntraEnergy", "Whether to benchmark intramolecular energy calculation");
    keywords_.add("Tests", new BoolKeyword(true), "TestInterEnergy", "Whether to benchmark interatomic energy calculation");
    keywords_.add("Tests", new BoolKeyword(true), "TestGenerator", "Whether to benchmark configuration generator");
    keywords_.add("Tests", new BoolKeyword(true), "TestRDFCells",
                  "Whether to benchmark the RDF cells method (to half-cell range)");
    keywords_.add("Tests", new BoolKeyword(false), "TestRDFSimple",
                  "Whether to benchmark the RDF simple method (to half-cell range)");
    keywords_.add("Tests", new BoolKeyword(true), "TestDistributors", "Whether to benchmark molecule distributors");
}
