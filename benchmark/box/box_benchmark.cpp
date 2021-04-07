// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors
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
        Vec3<double> lengths = {1.00, 1.00, 1.00};
        OrthorhombicBox box(lengths);
        return box;
    }
    else if constexpr (std::is_same_v<BoxType, MonoclinicBox>)
    {
        Vec3<double> lengths = {1.00, 1.00, 1.00};
        MonoclinicBox box(lengths, 45);
        return box;
    }
    else if constexpr (std::is_same_v<BoxType, TriclinicBox>)
    {
        Vec3<double> lengths = {1.00, 1.00, 1.00};
        Vec3<double> angles = {45, 45, 45};
        TriclinicBox box(lengths, angles);
        return box;
    }
    else
        return;
}

template <typename BoxType> static void BM_MIM(benchmark::State &state)
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

BENCHMARK_TEMPLATE(BM_MIM, CubicBox);
BENCHMARK_TEMPLATE(BM_MIM, OrthorhombicBox);
BENCHMARK_TEMPLATE(BM_MIM, MonoclinicBox);
BENCHMARK_TEMPLATE(BM_MIM, TriclinicBox);
