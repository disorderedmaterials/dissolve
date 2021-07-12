// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors
#include "benchmark/benchmark.h"
#include "math/data1d.h"
#include "templates/array.h"
#include "templates/array2d.h"
#include "templates/array3d.h"

#include <numeric>
#include <random>

// Benchmark array

// Benchmarks random access into the array structure

static void BM_Array_1d(benchmark::State &state)
{
    int bytes = state.range(0);
    int numVals = bytes / sizeof(int) / 2;
    std::default_random_engine generator;
    std::uniform_int_distribution<int> distribution(std::numeric_limits<int>::min(), std::numeric_limits<int>::max());
    std::vector<int> indices(numVals);
    std::iota(indices.begin(), indices.end(), 0);
    std::shuffle(indices.begin(), indices.end(), generator);
    Array<int> array(numVals);
    for (int i = 0; i < array.size(); ++i)
    {
        array[i] = distribution(generator);
    }

    for (auto _ : state)
    {
        long long int sum = 0;
        for (int i : indices)
            sum += array[i];
        benchmark::DoNotOptimize(sum);
    }
    state.SetBytesProcessed(long(state.iterations()) * long(bytes));
    state.SetLabel(std::to_string(bytes / 1024) + "kb");
}

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
                array[{i, j, k}] = distribution(generator);

    for (auto _ : state)
    {
        long long int sum = 0;
        for (int idx : indices)
        {
            int i = idx / (dim * dim);
            int j = (idx - i * (dim * dim)) / dim;
            int k = idx - i * (dim * dim) - j * dim;
            sum += array[{i, j, k}];
        }
        benchmark::DoNotOptimize(sum);
    }
    state.SetBytesProcessed(long(state.iterations()) * (long(bytes)));
    state.SetLabel(std::to_string(bytes / 1024) + "kb");
}

BENCHMARK(BM_Array_1d)->RangeMultiplier(4)->Range(1 << 10, 1 << 24);
BENCHMARK(BM_Array_2d)->RangeMultiplier(4)->Range(1 << 10, 1 << 24);
BENCHMARK(BM_Array_3d)->RangeMultiplier(4)->Range(1 << 10, 1 << 24);
