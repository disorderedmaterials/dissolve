// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "modules/siteRDF/siteRDF.h"
#include "common/problems.h"
#include "module/context.h"
#include <benchmark/benchmark.h>

namespace Benchmarks
{
template <SpeciesType speciesType, SpeciesPopulation population> static void BM_Module_SiteRDF(benchmark::State &state)
{
    Problem<speciesType, population> problemDef;
    SiteRDFModule module;
    std::vector<const SpeciesSite *> sites;
    sites.push_back(problemDef.dissolve().coreData().species().front()->sites().front().get());
    module.keywords().set("Configuration", problemDef.configuration());
    module.keywords().set("SiteA", sites);
    module.keywords().set("SiteB", sites);
    ModuleContext context(problemDef.dissolve().worldPool(), problemDef.dissolve());
    for (auto _ : state)
    {
        problemDef.configuration()->incrementContentsVersion();
        module.executeProcessing(context);
    }
}

BENCHMARK_TEMPLATE(BM_Module_SiteRDF, SpeciesType::SmallMolecule, SpeciesPopulation::Small)->Unit(benchmark::kMillisecond);
BENCHMARK_TEMPLATE(BM_Module_SiteRDF, SpeciesType::Atomic, SpeciesPopulation::Large)->Unit(benchmark::kMillisecond);
} // namespace Benchmarks

BENCHMARK_MAIN();
