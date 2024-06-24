// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "math/data1D.h"
#include "math/ft.h"
#include <benchmark/benchmark.h>
#include <numeric>
#include <random>

// Benchmark Fourier routines
static void BM_Fourier(benchmark::State &state)
{
    int bytes = state.range(0);
    int dim = bytes >> 3;
    std::default_random_engine generator;
    std::uniform_real_distribution distribution(0.0, 1.0);
    Data1D data;
    std::vector<double> x(dim), y(dim);
    std::iota(x.begin(), x.end(), 0);
    std::generate(y.begin(), y.end(), [&]() { return distribution(generator); });
    data.xAxis() = x;
    data.values() = y;

    for (auto _ : state)
    {
        Fourier::sineFT(data, 1, 1e-4, 1e-4, 1.0, WindowFunction(WindowFunction::Form::Lorch0));
    }
    state.SetBytesProcessed(long(state.iterations()) * (long(bytes)));
    state.SetLabel(std::to_string(bytes / 1024) + "kb");
}

BENCHMARK(BM_Fourier)->RangeMultiplier(4)->Range(1 << 10, 1 << 18);

BENCHMARK_MAIN();
