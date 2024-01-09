// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "benchmark/benchmark.h"
#include "math/data1D.h"
#include "math/ft.h"
#include "templates/array2D.h"
#include "templates/array3D.h"
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

BENCHMARK(BM_Array_2d)->RangeMultiplier(4)->Range(1 << 10, 1 << 24);
BENCHMARK(BM_Array_3d)->RangeMultiplier(4)->Range(1 << 10, 1 << 24);
BENCHMARK(BM_Fourier)->RangeMultiplier(4)->Range(1 << 10, 1 << 18);
