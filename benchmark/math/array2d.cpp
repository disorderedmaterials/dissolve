// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "templates/array2D.h"
#include <benchmark/benchmark.h>
#include <numeric>
#include <random>

// Benchmark array

// Benchmarks random access into the array structures

static void BM_Array_2d(benchmark::State &state)
{
    int bytes = state.range(0);
    int numVals = (bytes / sizeof(int)) / 2;
    std::default_random_engine generator;
    std::uniform_int_distribution<int> distribution(std::numeric_limits<int>::min(), std::numeric_limits<int>::max());
    int dim = sqrt(numVals);
    std::vector<int> indices(dim * dim);
    std::iota(indices.begin(), indices.end(), 0);
    std::shuffle(indices.begin(), indices.end(), generator);

    Array2D<int> array(dim, dim);
    for (int i = 0; i < array.nRows(); ++i)
        for (int j = 0; j < array.nColumns(); ++j)
            array[{i, j}] = distribution(generator);

    for (auto _ : state)
    {
        long long int sum = 0;
        for (int i : indices)
        {
            int row = i / dim;
            int column = i - row * dim;
            sum += array[{row, column}];
        }
        benchmark::DoNotOptimize(sum);
    }
    state.SetBytesProcessed(long(state.iterations()) * (long(bytes)));
    state.SetLabel(std::to_string(bytes / 1024) + "kb");
}

BENCHMARK(BM_Array_2d)->RangeMultiplier(4)->Range(1 << 10, 1 << 24);

BENCHMARK_MAIN();
