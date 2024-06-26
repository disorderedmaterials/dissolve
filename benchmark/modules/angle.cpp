// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "modules/angle/angle.h"
#include "common/problems.h"
#include "module/context.h"
#include <benchmark/benchmark.h>

namespace Benchmarks
{
template <SpeciesType speciesType, SpeciesPopulation population> static void BM_Module_Angle(benchmark::State &state)
{
    Problem<speciesType, population> problemDef;
    AngleModule module;
    auto *sp = problemDef.coreData().species().front().get();
    std::vector<const SpeciesSite *> siteO, siteH;
    siteO.push_back(sp->findSite("O"));
    siteH.push_back(sp->findSite("H"));
    module.keywords().set("Configuration", problemDef.configuration());
    module.keywords().set("SiteA", siteO);
    module.keywords().set("SiteB", siteH);
    module.keywords().set("SiteC", siteO);
    module.keywords().set("RangeAB", Vec3<double>{0.9, 1.1, 0.01});
    module.keywords().set("RangeAB", Vec3<double>{0.0, 5.0, 0.01});
    module.keywords().set("ExcludeSameMoleculeAB", false);
    module.keywords().set("ExcludeSameMoleculeBC", true);
    module.keywords().set("ExcludeSameSiteAC", false);
    ModuleContext context(problemDef.dissolve().worldPool(), problemDef.dissolve());
    for (auto _ : state)
    {
        problemDef.configuration()->incrementContentsVersion();
        module.executeProcessing(context);
    }
}

BENCHMARK_TEMPLATE(BM_Module_Angle, SpeciesType::SmallMolecule, SpeciesPopulation::Small)->Unit(benchmark::kMillisecond);
} // namespace Benchmarks

BENCHMARK_MAIN();
