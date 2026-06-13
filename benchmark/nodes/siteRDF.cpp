// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodes/siteRDF.h"
#include "classes/speciesSites.h"
#include "common/problems.h"
#include <benchmark/benchmark.h>

namespace Benchmarks
{
template <SystemType systemType> static void BM_SiteRDFNode(benchmark::State &state)
{
    GraphProblem<systemType> problemDef;

    // Create and set up a SiteRDF node
    auto *siteRDFNode = problemDef.createNode("SiteRDF");
    problemDef.addEdge({"SetCoordinates", "Configuration", "SiteRDF", "Configuration"});
    siteRDFNode->setOption("SiteA", SpeciesSites{{problemDef.species()->findSite("Origin")}});
    siteRDFNode->setOption("SiteB", SpeciesSites{{problemDef.species()->findSite("Origin")}});

    for (auto _ : state)
    {
        problemDef.configuration()->notifyAtomicPositionsChanged();
        siteRDFNode->run();
        siteRDFNode->setUpdateRequired();
    }
}

BENCHMARK_TEMPLATE(BM_SiteRDFNode, SystemType::ArgonCubic)->Unit(benchmark::kMicrosecond);;
BENCHMARK_TEMPLATE(BM_SiteRDFNode, SystemType::Water1000)->Unit(benchmark::kMicrosecond);

} // namespace Benchmarks

BENCHMARK_MAIN();
