// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "math/sampledData1D.h"
#include "math/sampledDouble.h"
#include "math/sampledVector.h"
#include "templates/algorithms.h"
#include <cmath>
#include <gtest/gtest.h>
#include <vector>

namespace UnitTest
{
class SampledValuesTest : public ::testing::Test
{
    public:
    SampledValuesTest() = default;

    void testDouble(const std::vector<double> &values)
    {
        auto mean = 0.0, stDev = 0.0;

        // Two-pass computation of mean followed by standard deviation
        for (auto value : values)
            mean += value;
        mean /= values.size();
        for (auto value : values)
            stDev += (value - mean) * (value - mean);
        stDev /= values.size() - 1;
        stDev = sqrt(stDev);

        // One-pass with SampledDouble
        SampledDouble x;
        for (auto value : values)
            x += value;

        EXPECT_DOUBLE_EQ(x.value(), mean);
        EXPECT_DOUBLE_EQ(x.stDev(), stDev);
    }

    SampledVector testVector(const std::vector<std::vector<double>> &values)
    {
        // One-pass with SampledVector
        SampledVector v;
        for (auto &vs : values)
            v += vs;

        // Calculate / test mean values
        std::vector<double> mean;
        mean.assign(values.front().size(), 0.0);
        for (auto &vs : values)
            std::transform(vs.begin(), vs.end(), mean.begin(), mean.begin(), [=](auto x, auto m) { return m + x; });
        std::transform(mean.begin(), mean.end(), mean.begin(), [=](auto m) { return m / values.size(); });
        for (auto &&[a, b] : zip(v.values(), mean))
            EXPECT_DOUBLE_EQ(a, b);

        // Calculate / test standard deviations
        std::vector<double> stDev;
        stDev.assign(values.front().size(), 0.0);
        for (auto &vs : values)
            for (auto &&[x, m, st] : zip(vs, mean, stDev))
                st += (x - m) * (x - m);
        std::transform(stDev.begin(), stDev.end(), stDev.begin(), [=](auto st) { return sqrt(st / (values.size() - 1)); });
        for (auto &&[a, b] : zip(v.stDev(), stDev))
            EXPECT_DOUBLE_EQ(a, b);

        return v;
    }
};

TEST_F(SampledValuesTest, SampledDouble)
{
    testDouble({4, 7, 13, 16});
    testDouble({10e8 + 4, 10e8 + 7, 10e8 + 13, 10e8 + 16});
    testDouble({9.2, 4.456, 98.7, 42.2, 12.09, 55.62});
}

TEST_F(SampledValuesTest, SampledVector)
{
    // Data
    std::vector<std::vector<double>> data1 = {{73.478, 4.901,  48.52,  83.77,  60.356, 84.403, 70.622, 13.638, 92.007, 39.253,
                                               31.703, 65.619, 17.404, 27.647, 40.677, 19.453, 73.203, 25.609, 17.674, 70.748},
                                              {15.375, 21.281, 52.272, 98.805, 38.857, 79.573, 23.458, 57.381, 50.727, 41.206,
                                               3.116,  85.6,   82.309, 10.512, 87.126, 16.309, 62.894, 74.282, 63.45,  96.273}};
    std::vector<std::vector<double>> data2 = {{7.433,  66.709, 6.001,  63.37,  19.852, 41.264, 42.347, 90.447, 92.651, 96.1,
                                               34.733, 29.341, 52.652, 66.318, 84.095, 21.576, 48.902, 90.737, 31.463, 46.744},
                                              {58.251, 79.925, 98.289, 7.341,  95.626, 32.155, 75.387, 14.633, 63.938, 3.581,
                                               52.948, 22.764, 45.079, 48.432, 24.91,  29.928, 16.268, 79.915, 34.386, 95.738},
                                              {84.984, 1.896, 94.278, 6.464, 57.482, 58.584, 9.001, 62.737, 59.6,  20.782,
                                               49.915, 40.27, 1.154,  44.58, 4.285,  20.551, 9.449, 63.176, 8.468, 53.585}};
    auto data3 = data1;
    data3.insert(data3.end(), data2.begin(), data2.end());

    // Equivalent Sums
    testVector({{4, 7, 13, 16}, {7, 13, 16, 4}, {13, 16, 4, 7}, {16, 4, 7, 13}});

    // Five sets of Real Data
    auto sampled3 = testVector(data3);

    // Combination of two sets
    auto sampled1 = testVector(data1);
    auto sampled2 = testVector(data2);
    SampledVector combined = sampled1;
    combined += sampled2;
    for (auto &&[a, b] : zip(combined.values(), sampled3.values()))
        EXPECT_DOUBLE_EQ(a, b);
    for (auto &&[a, b] : zip(combined.stDev(), sampled3.stDev()))
        EXPECT_DOUBLE_EQ(a, b);
}

TEST_F(SampledValuesTest, SampledVectorAssertions)
{
    // Mismatched vector sizes
    SampledVector v;
    v += {1, 2, 3, 4, 5};
    EXPECT_ANY_THROW({ v += std::vector<double>({1, 2, 3, 4}); });
}

TEST_F(SampledValuesTest, SampledData1D)
{
    SampledData1D a;
    SampledDouble d;
    std::vector<double> ys = {73.478, 4.901,  48.52,  83.77,  60.356, 84.403, 70.622, 13.638, 92.007, 39.253,
                              31.703, 65.619, 17.404, 27.647, 40.677, 19.453, 73.203, 25.609, 17.674, 70.748};
    const auto nPoints = ys.size();
    a.initialise(nPoints);
    for (auto n = 0; n < nPoints; ++n)
    {
        // Add values to sampled data1d
        a += ys;

        // Shuffle values
        auto y = ys[0];
        ys.erase(ys.begin());
        ys.push_back(y);

        // Accumulate the expected result while we're here
        d += y;
    }

    // Check results
    for (auto &&[y, stDev] : zip(a.values(), a.errors()))
    {
        EXPECT_DOUBLE_EQ(y, d.value());
        EXPECT_DOUBLE_EQ(stDev, d.stDev());
    }
}

} // namespace UnitTest
