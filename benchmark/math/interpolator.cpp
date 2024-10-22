// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "math/interpolator.h"
#include "math/data1D.h"
#include <algorithm>
#include <benchmark/benchmark.h>
#include <random>
#include <vector>

namespace Benchmarks
{
static void BM_Interpolator(benchmark::State &state)
{
    int bytes = state.range(0);
    int numVals = (bytes / sizeof(double));

    // Set up rng
    std::uniform_real_distribution<double> unif(-100, 100);
    std::default_random_engine re;

    // Generate data
    Data1D data;
    data.initialise(numVals);
    std::iota(data.xAxis().begin(), data.xAxis().end(), 0.0);
    std::generate(data.values().begin(), data.values().end(), [&]() { return unif(re); });

    // Generate sampling
    std::vector<double> samples(numVals);
    for (auto &s : samples)
        s = unif(re);

    Interpolator interp(data, Interpolator::SplineInterpolation);

    for (auto _ : state)
    {
        for (auto s : samples)
            auto result = interp.y(s);
    }

    state.SetBytesProcessed(long(state.iterations()) * (long(bytes)));
    state.SetLabel(std::to_string(bytes / 1024) + "kb");
}

BENCHMARK(BM_Interpolator)->RangeMultiplier(4)->Range(1 << 10, 1 << 24);
} // namespace Benchmarks

BENCHMARK_MAIN();
