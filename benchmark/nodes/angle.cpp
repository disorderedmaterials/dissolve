// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodes/angle.h"
#include "common/problems.h"
#include "classes/speciesSites.h"
#include <benchmark/benchmark.h>

namespace Benchmarks
{
template <SystemType systemType> static void BM_AngleNode(benchmark::State &state)
{
    GraphProblem<systemType> problemDef;

    // Create and set up an Angle node
    auto *angleNode = problemDef.createNode("Angle");
    problemDef.addEdge({"SetCoordinates", "Configuration", "Angle", "Configuration"});
    angleNode->setOption("SiteA", SpeciesSites{{problemDef.species()->findSite("Origin")}});
    angleNode->setOption("SiteB", SpeciesSites{{problemDef.species()->findSite("Origin")}});
    angleNode->setOption("SiteC", SpeciesSites{{problemDef.species()->findSite("Origin")}});
    
    angleNode->setOption("ExcludeSameMoleculeAB", true);
    angleNode->setOption("ExcludeSameMoleculeBC", true);
    angleNode->setOption("ExcludeSameSiteAC", true);
    for (auto _ : state)
    {
        problemDef.configuration()->notifyAtomicPositionsChanged();
        angleNode->run();
        angleNode->setUpdateRequired();
    }
}

BENCHMARK_TEMPLATE(BM_AngleNode, SystemType::ArgonCubic)->Unit(benchmark::kMillisecond);
BENCHMARK_TEMPLATE(BM_AngleNode, SystemType::Water1000)->Unit(benchmark::kMillisecond);
} // namespace Benchmarks

BENCHMARK_MAIN();
