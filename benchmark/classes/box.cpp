// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "classes/box.h"
#include "math/vector3.h"
#include <benchmark/benchmark.h>
#include <cmath>
#include <stdexcept>

namespace Benchmarks
{
Box createTestBox(Box::BoxType boxType)
{
    switch (boxType)
    {
        case (Box::BoxType::Cubic):
            return Box({1.0, 1.0, 1.0});
        case (Box::BoxType::Orthorhombic):
            return Box({1.0, 2.0, 3.0});
        case (Box::BoxType::MonoclinicAlpha):
            return Box({1.0, 1.0, 1.0}, {45.0, 90.0, 90.0});
        case (Box::BoxType::Triclinic):
            return Box({1.0, 1.0, 1.0}, {45.0, 60.0, 80.0});
        default:
            throw std::runtime_error("Invalid box type");
    }
}

static void BM_Box_MinimumImage(benchmark::State &state, Box::BoxType t)
{
    auto box = createTestBox(t);
    Vector3 p1 = box.randomCoordinate();
    Vector3 p2 = box.randomCoordinate();
    for (auto _ : state)
    {
        auto dist = box.minimumImage(p1, p2);
        benchmark::DoNotOptimize(dist);
    }
}

static void BM_Box_MinimumDistance(benchmark::State &state, Box::BoxType t)
{
    auto box = createTestBox(t);
    Vector3 p1 = box.randomCoordinate();
    Vector3 p2 = box.randomCoordinate();
    for (auto _ : state)
    {
        auto dist = box.minimumDistance(p1, p2);
        benchmark::DoNotOptimize(dist);
    }
}

static void BM_Box_MinimumDistanceSquared(benchmark::State &state, Box::BoxType t)
{
    auto box = createTestBox(t);
    Vector3 p1 = box.randomCoordinate();
    Vector3 p2 = box.randomCoordinate();
    for (auto _ : state)
    {
        auto dist = box.minimumDistanceSquared(p1, p2);
        benchmark::DoNotOptimize(dist);
    }
}

static void BM_Box_MinimumVector(benchmark::State &state, Box::BoxType t)
{
    auto box = createTestBox(t);
    Vector3 p1 = box.randomCoordinate();
    Vector3 p2 = box.randomCoordinate();
    for (auto _ : state)
    {
        auto dist = box.minimumVector(p1, p2);
        benchmark::DoNotOptimize(dist);
    }
}

static void BM_Box_RandomCoordinate(benchmark::State &state, Box::BoxType t)
{
    auto box = createTestBox(t);
    for (auto _ : state)
    {
        auto r = box.randomCoordinate();
        benchmark::DoNotOptimize(r);
    }
}

static void BM_Box_Fold(benchmark::State &state, Box::BoxType t)
{
    auto box = createTestBox(t);
    Vector3 p1 = box.randomCoordinate();
    for (auto _ : state)
    {
        auto r = box.fold(p1);
        benchmark::DoNotOptimize(r);
    }
}

static void BM_Box_FoldFrac(benchmark::State &state, Box::BoxType t)
{
    auto box = createTestBox(t);
    Vector3 p1 = box.randomCoordinate();
    for (auto _ : state)
    {
        auto r = box.foldFrac(p1);
        benchmark::DoNotOptimize(r);
    }
}

BENCHMARK_CAPTURE(BM_Box_MinimumDistance, Cubic, Box::BoxType::Cubic);
BENCHMARK_CAPTURE(BM_Box_MinimumDistanceSquared, Cubic, Box::BoxType::Cubic);
BENCHMARK_CAPTURE(BM_Box_MinimumImage, Cubic, Box::BoxType::Cubic);
BENCHMARK_CAPTURE(BM_Box_MinimumVector, Cubic, Box::BoxType::Cubic);
BENCHMARK_CAPTURE(BM_Box_RandomCoordinate, Cubic, Box::BoxType::Cubic);
BENCHMARK_CAPTURE(BM_Box_Fold, Cubic, Box::BoxType::Cubic);
BENCHMARK_CAPTURE(BM_Box_FoldFrac, Cubic, Box::BoxType::Cubic);
BENCHMARK_CAPTURE(BM_Box_MinimumDistance, Orthorhombic, Box::BoxType::Orthorhombic);
BENCHMARK_CAPTURE(BM_Box_MinimumDistanceSquared, Orthorhombic, Box::BoxType::Orthorhombic);
BENCHMARK_CAPTURE(BM_Box_MinimumImage, Orthorhombic, Box::BoxType::Orthorhombic);
BENCHMARK_CAPTURE(BM_Box_MinimumVector, Orthorhombic, Box::BoxType::Orthorhombic);
BENCHMARK_CAPTURE(BM_Box_RandomCoordinate, Orthorhombic, Box::BoxType::Orthorhombic);
BENCHMARK_CAPTURE(BM_Box_Fold, Orthorhombic, Box::BoxType::Orthorhombic);
BENCHMARK_CAPTURE(BM_Box_FoldFrac, Orthorhombic, Box::BoxType::Orthorhombic);
BENCHMARK_CAPTURE(BM_Box_MinimumDistance, MonoclinicAlpha, Box::BoxType::MonoclinicAlpha);
BENCHMARK_CAPTURE(BM_Box_MinimumDistanceSquared, MonoclinicAlpha, Box::BoxType::MonoclinicAlpha);
BENCHMARK_CAPTURE(BM_Box_MinimumImage, MonoclinicAlpha, Box::BoxType::MonoclinicAlpha);
BENCHMARK_CAPTURE(BM_Box_MinimumVector, MonoclinicAlpha, Box::BoxType::MonoclinicAlpha);
BENCHMARK_CAPTURE(BM_Box_RandomCoordinate, MonoclinicAlpha, Box::BoxType::MonoclinicAlpha);
BENCHMARK_CAPTURE(BM_Box_Fold, MonoclinicAlpha, Box::BoxType::MonoclinicAlpha);
BENCHMARK_CAPTURE(BM_Box_FoldFrac, MonoclinicAlpha, Box::BoxType::MonoclinicAlpha);
BENCHMARK_CAPTURE(BM_Box_MinimumDistance, Triclinic, Box::BoxType::Triclinic);
BENCHMARK_CAPTURE(BM_Box_MinimumDistanceSquared, Triclinic, Box::BoxType::Triclinic);
BENCHMARK_CAPTURE(BM_Box_MinimumImage, Triclinic, Box::BoxType::Triclinic);
BENCHMARK_CAPTURE(BM_Box_MinimumVector, Triclinic, Box::BoxType::Triclinic);
BENCHMARK_CAPTURE(BM_Box_RandomCoordinate, Triclinic, Box::BoxType::Triclinic);
BENCHMARK_CAPTURE(BM_Box_Fold, Triclinic, Box::BoxType::Triclinic);
BENCHMARK_CAPTURE(BM_Box_FoldFrac, Triclinic, Box::BoxType::Triclinic);
} // namespace Benchmarks

BENCHMARK_MAIN();
