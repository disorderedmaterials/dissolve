// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors
#include "benchmark/benchmark.h"
#include "math/histogram1d.h"
#include "math/histogram2d.h"
#include "math/histogram3d.h"
#include <random>

// Benchmark histograms

// Creates a histogram 'x' bytes of data
// e.g. in 1D:
// if we have 1000 bytes we have 1000/8 = 125 bins
// we then bin a random number into the histogram
// e.g benchmarking random access into the histogram
// outputs how many values we can process per second followed by the size of our histogram bins in memory

static void BM_HistogramBinning_1d(benchmark::State &state)
{
    int bytes = 1 << state.range(0);
    int numVals = bytes / sizeof(long int);
    Histogram1D histo;
    histo.initialise(0, 1, 1.00 / numVals);
    std::default_random_engine generator;
    std::uniform_real_distribution<double> distribution(0.0, 1.0);
    for (auto _ : state)
        benchmark::DoNotOptimize(histo.bin(distribution(generator)));
    state.SetBytesProcessed(long(state.iterations()) * sizeof(double));
    state.SetLabel(std::to_string(bytes / 1024) + "kb");
}

static void BM_HistogramBinning_2d(benchmark::State &state)
{
    int bytes = 1 << state.range(0);
    int numVals = (bytes / sizeof(long int)) / 2;
    Histogram2D histo;
    double binWidth = 1.00 / numVals;
    histo.initialise(0, 1, binWidth, 0, 1, binWidth);
    std::default_random_engine generator;
    std::uniform_real_distribution<double> distribution(0.0, 1.0);
    for (auto _ : state)
        benchmark::DoNotOptimize(histo.bin(distribution(generator), distribution(generator)));
    state.SetBytesProcessed(long(state.iterations()) * sizeof(double) * 2);
    state.SetLabel(std::to_string(bytes / 1024) + "kb");
}
static void BM_HistogramBinning_3d(benchmark::State &state)
{
    int bytes = 1 << state.range(0);
    int numVals = (bytes / sizeof(long int)) / 3;
    Histogram3D histo;
    double binWidth = 1.00 / numVals;
    histo.initialise(0, 1, binWidth, 0, 1, binWidth, 0, 1, binWidth);
    std::default_random_engine generator;
    std::uniform_real_distribution<double> distribution(0.0, 1.0);
    for (auto _ : state)
        benchmark::DoNotOptimize(histo.bin(distribution(generator), distribution(generator), distribution(generator)));
    state.SetBytesProcessed(long(state.iterations()) * sizeof(double) * 3);
    state.SetLabel(std::to_string(bytes / 1024) + "kb");
}

BENCHMARK(BM_HistogramBinning_1d)->DenseRange(10, 24);
BENCHMARK(BM_HistogramBinning_2d)->DenseRange(10, 16);
BENCHMARK(BM_HistogramBinning_3d)->DenseRange(10, 12);