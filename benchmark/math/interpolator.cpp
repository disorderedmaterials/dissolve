// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "math/interpolator.h"
#include <benchmark/benchmark.h>
#include <random>
#include <vector>

static void BM_Interpolator(benchmark::State &state)
{
    int bytes = state.range(0);
    int numVals = (bytes / sizeof(double));
    std::vector<double> xs(numVals), ys(numVals), samples(numVals);

    std::uniform_real_distribution<double> unif(-100, 100);
    std::default_random_engine re;

    for (auto &x : xs)
        x = unif(re);
    for (auto &y : ys)
        y = unif(re);
    for (auto &s : samples)
        s = unif(re);

    Interpolator interp(xs, ys, Interpolator::SplineInterpolation);

    for (auto _ : state)
    {
        auto result = interp.y(samples);
    }

    state.SetBytesProcessed(long(state.iterations()) * (long(bytes)));
    state.SetLabel(std::to_string(bytes / 1024) + "kb");
}

BENCHMARK(BM_Interpolator)->RangeMultiplier(4)->Range(1 << 10, 1 << 24);

BENCHMARK_MAIN();
