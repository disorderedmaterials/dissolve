// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "templates/array3D.h"
#include <benchmark/benchmark.h>
#include <numeric>
#include <random>

namespace Benchmarks
{
// Benchmarks random access into the Array3D structure
static void BM_Array_3d(benchmark::State &state)
{
    int bytes = state.range(0);
    int numVals = (bytes / sizeof(int)) / 2;
    std::default_random_engine generator;
    std::uniform_int_distribution<int> distribution(std::numeric_limits<int>::min(), std::numeric_limits<int>::max());
    int dim = cbrt(numVals);
    std::vector<int> indices(dim * dim * dim);
    std::iota(indices.begin(), indices.end(), 0);
    std::shuffle(indices.begin(), indices.end(), generator);

    Array3D<int> array(dim, dim, dim);
    for (int i = 0; i < array.nX(); ++i)
        for (int j = 0; j < array.nY(); ++j)
            for (int k = 0; k < array.nZ(); ++k)
                array[std::tuple{i, j, k}] = distribution(generator);

    for (auto _ : state)
    {
        long long int sum = 0;
        for (int idx : indices)
        {
            int i = idx / (dim * dim);
            int j = (idx - i * (dim * dim)) / dim;
            int k = idx - i * (dim * dim) - j * dim;
            sum += array[std::tuple{i, j, k}];
        }
        benchmark::DoNotOptimize(sum);
    }
    state.SetBytesProcessed(long(state.iterations()) * (long(bytes)));
    state.SetLabel(std::to_string(bytes / 1024) + "kb");
}

BENCHMARK(BM_Array_3d)->RangeMultiplier(4)->Range(1 << 10, 1 << 24);
} // namespace Benchmarks
BENCHMARK_MAIN();
