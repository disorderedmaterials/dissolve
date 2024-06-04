// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "benchmark/benchmark.h"
#include "common/problems.h"
#include "module/context.h"
#include "modules/siteRDF/siteRDF.h"

template <ProblemType problem, Population population>
static void BM_Module_SiteRDF(benchmark::State &state)
{
    Problem<problem, population> problemDef;
    SiteRDFModule module;
    std::vector<const SpeciesSite *> sites;
    sites.push_back(problemDef.dissolve_.coreData().species().front()->sites().front().get());
    module.keywords().set("Configuration", problemDef.cfg_);
    module.keywords().set("SiteA", sites);
    module.keywords().set("SiteB", sites);
    ModuleContext context(problemDef.dissolve_.worldPool(), problemDef.dissolve_);
    for (auto _ : state)
    {
        problemDef.cfg_->incrementContentsVersion();
        module.executeProcessing(context);
    }
}

BENCHMARK_TEMPLATE(BM_Module_SiteRDF, ProblemType::smallMolecule, Population::small)->Unit(benchmark::kMillisecond);;
BENCHMARK_TEMPLATE(BM_Module_SiteRDF, ProblemType::atomic, Population::large)->Unit(benchmark::kMillisecond);;
