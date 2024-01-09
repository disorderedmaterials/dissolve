// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "benchmark/benchmark.h"
#include "classes/box.h"
#include "templates/vector3.h"
#include <cmath>

template <typename BoxType> BoxType createTestBox()
{
    if constexpr (std::is_same_v<BoxType, CubicBox>)
    {
        double lengths = 1.00;
        CubicBox box(lengths);
        return box;
    }
    else if constexpr (std::is_same_v<BoxType, OrthorhombicBox>)
    {
        Vec3<double> lengths(1.00, 1.00, 1.00);
        OrthorhombicBox box(lengths);
        return box;
    }
    else if constexpr (std::is_same_v<BoxType, MonoclinicAlphaBox>)
    {
        Vec3<double> lengths(1.00, 1.00, 1.00);
        MonoclinicAlphaBox box(lengths, 45);
        return box;
    }
    else if constexpr (std::is_same_v<BoxType, TriclinicBox>)
    {
        Vec3<double> lengths(1.00, 1.00, 1.00);
        Vec3<double> angles(45, 45, 45);
        TriclinicBox box(lengths, angles);
        return box;
    }
    else
        return;
}

template <typename BoxType> static void BM_Box_MinimumImage(benchmark::State &state)
{
    auto box = createTestBox<BoxType>();
    Vec3<double> p1 = box.randomCoordinate();
    Vec3<double> p2 = box.randomCoordinate();
    for (auto _ : state)
    {
        auto dist = box.minimumImage(p1, p2);
        benchmark::DoNotOptimize(dist);
    }
}

template <typename BoxType> static void BM_Box_MinimumDistance(benchmark::State &state)
{
    auto box = createTestBox<BoxType>();
    Vec3<double> p1 = box.randomCoordinate();
    Vec3<double> p2 = box.randomCoordinate();
    for (auto _ : state)
    {
        auto dist = box.minimumDistance(p1, p2);
        benchmark::DoNotOptimize(dist);
    }
}

template <typename BoxType> static void BM_Box_MinimumDistanceSquared(benchmark::State &state)
{
    auto box = createTestBox<BoxType>();
    Vec3<double> p1 = box.randomCoordinate();
    Vec3<double> p2 = box.randomCoordinate();
    for (auto _ : state)
    {
        auto dist = box.minimumDistanceSquared(p1, p2);
        benchmark::DoNotOptimize(dist);
    }
}

template <typename BoxType> static void BM_Box_MinimumVector(benchmark::State &state)
{
    auto box = createTestBox<BoxType>();
    Vec3<double> p1 = box.randomCoordinate();
    Vec3<double> p2 = box.randomCoordinate();
    for (auto _ : state)
    {
        auto dist = box.minimumVector(p1, p2);
        benchmark::DoNotOptimize(dist);
    }
}

template <typename BoxType> static void BM_Box_RandomCoordinate(benchmark::State &state)
{
    auto box = createTestBox<BoxType>();
    for (auto _ : state)
    {
        auto r = box.randomCoordinate();
        benchmark::DoNotOptimize(r);
    }
}

template <typename BoxType> static void BM_Box_Fold(benchmark::State &state)
{
    auto box = createTestBox<BoxType>();
    Vec3<double> p1 = box.randomCoordinate();
    for (auto _ : state)
    {
        auto r = box.fold(p1);
        benchmark::DoNotOptimize(r);
    }
}

template <typename BoxType> static void BM_Box_FoldFrac(benchmark::State &state)
{
    auto box = createTestBox<BoxType>();
    Vec3<double> p1 = box.randomCoordinate();
    for (auto _ : state)
    {
        auto r = box.foldFrac(p1);
        benchmark::DoNotOptimize(r);
    }
}

BENCHMARK_TEMPLATE(BM_Box_MinimumDistance, CubicBox);
BENCHMARK_TEMPLATE(BM_Box_MinimumDistanceSquared, CubicBox);
BENCHMARK_TEMPLATE(BM_Box_MinimumImage, CubicBox);
BENCHMARK_TEMPLATE(BM_Box_MinimumVector, CubicBox);
BENCHMARK_TEMPLATE(BM_Box_RandomCoordinate, CubicBox);
BENCHMARK_TEMPLATE(BM_Box_Fold, CubicBox);
BENCHMARK_TEMPLATE(BM_Box_FoldFrac, CubicBox);
BENCHMARK_TEMPLATE(BM_Box_MinimumDistance, OrthorhombicBox);
BENCHMARK_TEMPLATE(BM_Box_MinimumDistanceSquared, OrthorhombicBox);
BENCHMARK_TEMPLATE(BM_Box_MinimumImage, OrthorhombicBox);
BENCHMARK_TEMPLATE(BM_Box_MinimumVector, OrthorhombicBox);
BENCHMARK_TEMPLATE(BM_Box_RandomCoordinate, OrthorhombicBox);
BENCHMARK_TEMPLATE(BM_Box_Fold, OrthorhombicBox);
BENCHMARK_TEMPLATE(BM_Box_FoldFrac, OrthorhombicBox);
BENCHMARK_TEMPLATE(BM_Box_MinimumDistance, MonoclinicAlphaBox);
BENCHMARK_TEMPLATE(BM_Box_MinimumDistanceSquared, MonoclinicAlphaBox);
BENCHMARK_TEMPLATE(BM_Box_MinimumImage, MonoclinicAlphaBox);
BENCHMARK_TEMPLATE(BM_Box_MinimumVector, MonoclinicAlphaBox);
BENCHMARK_TEMPLATE(BM_Box_RandomCoordinate, MonoclinicAlphaBox);
BENCHMARK_TEMPLATE(BM_Box_Fold, MonoclinicAlphaBox);
BENCHMARK_TEMPLATE(BM_Box_FoldFrac, MonoclinicAlphaBox);
BENCHMARK_TEMPLATE(BM_Box_MinimumDistance, TriclinicBox);
BENCHMARK_TEMPLATE(BM_Box_MinimumDistanceSquared, TriclinicBox);
BENCHMARK_TEMPLATE(BM_Box_MinimumImage, TriclinicBox);
BENCHMARK_TEMPLATE(BM_Box_MinimumVector, TriclinicBox);
BENCHMARK_TEMPLATE(BM_Box_RandomCoordinate, TriclinicBox);
BENCHMARK_TEMPLATE(BM_Box_Fold, TriclinicBox);
BENCHMARK_TEMPLATE(BM_Box_FoldFrac, TriclinicBox);
